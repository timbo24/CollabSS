using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SpreadsheetUtilities;
using System.Xml;
using System.IO;
using System.Text.RegularExpressions;

namespace SS
{
    /// <summary>
    /// A Spreadsheet object represents the state of a simple spreadsheet.  A 
    /// spreadsheet consists of an infinite number of named cells.
    /// 
    /// A string is a valid cell name if and only if:
    ///   (1) its first character is an underscore or a letter
    ///   (2) its remaining characters (if any) are underscores and/or letters and/or digits
    /// Note that this is the same as the definition of valid variable from the PS3 Formula class.
    /// 
    /// For example, "x", "_", "x2", "y_15", and "___" are all valid cell  names, but
    /// "25", "2x" are not.  Cell names are case sensitive, so "x" and "X" are
    /// different cell names.
    /// 
    /// A spreadsheet contains a cell corresponding to every possible cell name.  (This
    /// means that a spreadsheet contains an infinite number of cells.)  In addition to 
    /// a name, each cell has a contents and a value.  The distinction is important.
    /// 
    /// The contents of a cell can be (1) a string, (2) a double, or (3) a Formula.  If the
    /// contents is an empty string, we say that the cell is empty.  (By analogy, the contents
    /// of a cell in Excel is what is displayed on the editing line when the cell is selected.)
    /// 
    /// In a new spreadsheet, the contents of every cell is the empty string.
    ///  
    /// The value of a cell can be (1) a string, (2) a double, or (3) a FormulaError.  
    /// (By analogy, the value of an Excel cell is what is displayed in that cell's position
    /// in the grid.)
    /// 
    /// If a cell's contents is a string, its value is that string.
    /// 
    /// If a cell's contents is a double, its value is that double.
    /// 
    /// If a cell's contents is a Formula, its value is either a double or a FormulaError,
    /// as reported by the Evaluate method of the Formula class.  The value of a Formula,
    /// of course, can depend on the values of variables.  The value of a variable is the 
    /// value of the spreadsheet cell it names (if that cell's value is a double) or 
    /// is undefined (otherwise).
    /// 
    /// Spreadsheets are never allowed to contain a combination of Formulas that establish
    /// a circular dependency.  A circular dependency exists when a cell depends on itself.
    /// For example, suppose that A1 contains B1*2, B1 contains C1*2, and C1 contains A1*2.
    /// A1 depends on B1, which depends on C1, which depends on A1.  That's a circular
    /// dependency.
    /// </summary>
    public class Spreadsheet : AbstractSpreadsheet
    {
       
        /// <summary>
        /// Spreadsheet consists of a dictionary of named cells.
        /// </summary>
        Dictionary<String, Cell> dict;
        
        /// <summary>
        /// //Spreadsheet has a dependency graph to represent connections between cells.
        /// </summary>
        private DependencyGraph dg;

        /// <summary>
        /// //represents the name of a file, from which the spreadsheet will be created.
        /// </summary>
        public string filePath;

       

        /// <summary>
        /// Zero-argument constructor. Creates an empty spreadsheet that imposes no extra validity conditions, 
        /// normalizes every cell name to itself, and has version "default". 
        /// </summary>
        public Spreadsheet()
            : this(s => true, s => s, "default")
        {
           
        }
        /// <summary>
        /// Three-argument constructor to the Spreadsheet class. It creates an empty spreadsheet and allows the user 
        /// to provide a validity delegate (first parameter), a normalization delegate (second parameter), and a version (third parameter).
        /// </summary>
        public Spreadsheet(Func<string, bool> isValid, Func<string, string> normalize, string version): base(isValid, normalize, version)
        {
            dict = new Dictionary<String, Cell>();
            dg = new DependencyGraph();
            this.Changed = false;
        }

        /// <summary>
        /// Four-argument constructor to the Spreadsheet class. It allows the user to provide a string representing a path to a file (first parameter), 
        /// a validity delegate (second parameter), a normalization delegate (third parameter), and a version (fourth parameter). 
        /// It reads a saved spreadsheet from a file (see the Save method) and uses it to construct a new spreadsheet. 
        /// The new spreadsheet uses the provided validity delegate, normalization delegate, and version.
        /// </summary>
        public Spreadsheet(string filePath, Func<string, bool> isValid, Func<string, string> normalize, string version)
            : base(isValid, normalize, version)
            
        {
            this.filePath = filePath;
            dict = new Dictionary<String, Cell>();
            dg = new DependencyGraph();
            this.Changed=false;

            //Creating the spreadsheet from a file.
            try
            {
                using (XmlReader reader = XmlReader.Create(filePath))
                {
                    string cellName="";
                    string cellContent="";
                    Formula cellFormula = null;
                    bool isFormula = false;
                    double number;
                    //Searching for the "spreadsheet" tag.
                    while ((reader.Read()) && (!((reader.IsStartElement()) && (reader.Name.Equals("spreadsheet")))))
                    {}
                    //If there is no "spreadsheet" tag then there is no version info; then throw an exception.
                    if (!((reader.IsStartElement()) && (reader.Name.Equals("spreadsheet"))))
                        throw new SpreadsheetReadWriteException("The file doesn't have a version info.");
                    //If version is not right throw an exception.
                    else if (!(reader["version"].Equals(version)))
                        throw new SpreadsheetReadWriteException("The file's version doesn't match the allowed version.");
                    //Go through all the tags and add cells to the spreadsheet.
                    while (reader.Read())
                    {
                        if (reader.IsStartElement())
                        {
                            switch (reader.Name)
                            {
                                case "cell":
                                    break;
                            //record a cell's name
                                case "name":
                                reader.Read();
                                cellName=reader.Value;
                                break;
                            //record a cell's content
                                case "contents":
                                reader.Read();
                                cellContent = reader.Value;
                            //If an element is a formula:
                                if (cellContent.StartsWith("=")) 
                                {
                                    cellFormula = new Formula(cellContent.Substring(1));
                                    isFormula = true;
                                }
                                    bool isNumber = double.TryParse(cellContent, out number);

                            //If an element is a number:
                            if (isNumber)
                                SetCellContents(cellName, number);
                            //If it is a formula:
                            else if (isFormula)
                                SetCellContents(cellName, cellFormula);
                            //If it is a string:
                            else SetCellContents(cellName, cellContent);
                                break;
                                
                            }
                            
                        }
                    }

                    this.Changed = false;
                }
            }

            catch (SpreadsheetReadWriteException e)
            {
                throw new SpreadsheetReadWriteException(e.Message);
            }
            
             catch (IOException)
            {
                throw new SpreadsheetReadWriteException("The file with the name " + filePath + " doesn't exist");
            }

            
             catch 
            {
                throw new SpreadsheetReadWriteException("The file can't be read as XML file. Please check the XML tags inside the file.");
            }
           

        }

        public Dictionary<String, Cell> GetDictionary()
        {
            return this.dict;
        }

        /// <summary>
        /// True if this spreadsheet has been modified since it was created or saved                  
        /// (whichever happened most recently); false otherwise.
        /// </summary>
        public override bool Changed { get; protected set; }
        



        // ADDED FOR PS5
        /// <summary>
        /// Returns the version information of the spreadsheet saved in the named file.
        /// If there are any problems opening, reading, or closing the file, the method
        /// should throw a SpreadsheetReadWriteException with an explanatory message.
        /// </summary>
        public override string GetSavedVersion(string filename)
        {
            try
            {
                using (XmlReader reader = XmlReader.Create(filename))
                {

                    while ((reader.Read()) && (!reader.IsStartElement()))
                    { }
                    if ((!reader.IsStartElement()) || (!(reader.Name.Equals("spreadsheet"))))
                        throw new SpreadsheetReadWriteException("The file doesn't have a version info.");

                    return reader["version"];
                }
            }
            catch 
            {
                throw new SpreadsheetReadWriteException("The file with the name " + filename + " doesn't exist");
            }
        }

        // ADDED FOR PS5
        /// <summary>
        /// Writes the contents of this spreadsheet to the named file using an XML format.
        /// The XML elements should be structured as follows:
        /// 
        /// <spreadsheet version="version information goes here">
        /// 
        /// <cell>
        /// <name>
        /// cell name goes here
        /// </name>
        /// <contents>
        /// cell contents goes here
        /// </contents>    
        /// </cell>
        /// 
        /// </spreadsheet>
        /// 
        /// There should be one cell element for each non-empty cell in the spreadsheet.  
        /// If the cell contains a string, it should be written as the contents.  
        /// If the cell contains a double d, d.ToString() should be written as the contents.  
        /// If the cell contains a Formula f, f.ToString() with "=" prepended should be written as the contents.
        /// 
        /// If there are any problems opening, writing, or closing the file, the method should throw a
        /// SpreadsheetReadWriteException with an explanatory message.
        /// </summary>
        public override void Save(string filename)
        {
            try
            {
                using (XmlWriter writer = XmlWriter.Create(filename))
                {
                    writer.WriteStartDocument();
                    writer.WriteStartElement("spreadsheet");
                    writer.WriteAttributeString("version", this.Version);

                    //Go through each cell and record it in a file.
                    foreach (KeyValuePair<string, Cell> pair in this.dict)
                    {
                        writer.WriteStartElement("cell");
                        writer.WriteElementString("name", pair.Key);
                        if (pair.Value.contents is string)
                            writer.WriteElementString("contents", (string)pair.Value.contents);
                        else if (pair.Value.contents is Double)
                            writer.WriteElementString("contents", pair.Value.contents.ToString());
                        else
                            writer.WriteElementString("contents", "=" + pair.Value.contents.ToString());
                        writer.WriteEndElement();



                    }
                    writer.WriteEndElement();
                    writer.WriteEndDocument();
                    this.Changed = false;
                }
            }
            catch
            {
                throw new SpreadsheetReadWriteException("The directory " + filename + " doesn't exist");
            }
        }

        /// <summary>
        /// If name is null or invalid, throws an InvalidNameException.
        /// 
        /// Otherwise, returns the value (as opposed to the contents) of the named cell.  The return
        /// value should be either a string, a double, or a SpreadsheetUtilities.FormulaError.
        /// </summary>
        public override object GetCellValue(string name)
        {
            if (!isValidName(name))
                throw new InvalidNameException();

            //Normilize the name.
            string nameNorm = this.Normalize(name);
            //Check if it is still valid.
            if (!isValidName(nameNorm))
                throw new InvalidNameException();

            //If this dictionary already has the name, then return the corresponding cell's value.
            if (this.dict.ContainsKey(nameNorm))
            {
                if ((this.dict[nameNorm].contents is Formula)&&(!(this.dict[nameNorm].value is double)))
                {
                    Formula f = (Formula)this.dict[nameNorm].contents;
                    this.dict[nameNorm].setValue(f, lookup);
                }
                return this.dict[nameNorm].value;
            }
            //If this dictionary doesn't have the name, then return "", like it is an empty cell.
            else return "";

        }

        /// <summary>
        /// Lookup function for getting a value for a variable. (for calculating formula)
        /// </summary>
        /// <param name="s">string s is a variable and a name of a cell</param>
        /// <returns>double</returns>
        public double lookup(string s)
        {
            if(!(this.dict.ContainsKey(s)))
                throw new ArgumentException();
            if(this.dict[s].contents is string)
                throw new ArgumentException();
            if (this.dict[s].value is double)
                return (double)this.dict[s].value;
            
               
            //if the cell has not calculated formula, calculate it if possible and return the value.
            if (this.dict[s].contents is Formula)
            {

                Formula f = (Formula)dict[s].contents;
                try
                {
                    double x = (double)f.Evaluate(lookup);
                    return x;
                }
                catch
                {
                    throw new ArgumentException();
                }
            }

            else
                throw new ArgumentException();

        }
      /// <summary>
      /// Checks if the name is not null and if it starts with 1 or more letters, followed by one or more numbers.
      /// </summary>
      /// <param name="name"></param>
      /// <returns></returns>
        private bool isValidName(String name)
        {
            
            if (name == null)
                return false;
            Regex r;
            Match m;
            r = new Regex(@"^([a-zA-Z]+)([1-9]\d*)$");
            m = r.Match(name);
            if ((m.Success)&&(this.IsValid(name)))
                return true;
            else return false;
        }

        /// <summary>
        /// If content is null, throws an ArgumentNullException.
        /// 
        /// Otherwise, if name is null or invalid, throws an InvalidNameException.
        /// 
        /// Otherwise, if content parses as a double, the contents of the named
        /// cell becomes that double.
        /// 
        /// Otherwise, if content begins with the character '=', an attempt is made
        /// to parse the remainder of content into a Formula f using the Formula
        /// constructor.  There are then three possibilities:
        /// 
        ///   (1) If the remainder of content cannot be parsed into a Formula, a 
        ///       SpreadsheetUtilities.FormulaFormatException is thrown.
        ///       
        ///   (2) Otherwise, if changing the contents of the named cell to be f
        ///       would cause a circular dependency, a CircularException is thrown.
        ///       
        ///   (3) Otherwise, the contents of the named cell becomes f.
        /// 
        /// Otherwise, the contents of the named cell becomes content.
        /// 
        /// If an exception is not thrown, the method returns a set consisting of
        /// name plus the names of all other cells whose value depends, directly
        /// or indirectly, on the named cell.
        /// 
        /// For example, if name is A1, B1 contains A1*2, and C1 contains B1+A1, the
        /// set {A1, B1, C1} is returned.
        /// </summary>
        public override ISet<string> SetContentsOfCell(string name, string content)
        {
            
            if (content == null)
                throw new ArgumentNullException();
            if (!isValidName(name))
                throw new InvalidNameException();
            double number;
            Formula f;
            bool isNumber = double.TryParse(content, out number);
            if (isNumber)
                return SetCellContents(this.Normalize(name), number);
            else if (content.StartsWith("="))
            {
                try
                {
                    f = new Formula(content.Substring(1), this.Normalize, this.IsValid);
                }
                catch (FormulaFormatException e)
                {
                    throw new SpreadsheetUtilities.FormulaFormatException(e.Message);
                }

               
               // foreach (string s in f.GetVariables())
               
                //    if (!isValidName(s))
                //        throw new SpreadsheetUtilities.FormulaFormatException("The formula has an invalid variable name. It should start with one or more letters, followed by one or more numbers.");

                try
                {

                    return SetCellContents(this.Normalize(name), f);
                }

                catch (CircularException)
                {
                    throw new CircularException();
                }

            }

            else return SetCellContents(this.Normalize(name), content);
        }

        /// <summary>
        /// Enumerates the names of all the non-empty cells in the spreadsheet.
        /// </summary>
        public override IEnumerable<string> GetNamesOfAllNonemptyCells()
        {
            List<string> e = new List<String>();
            List<string> l = new List<String>();
            e = dict.Keys.ToList(); //puts all the non-null keys of this dictionary into the list
            //Check if any of the cells have an empty string.
            foreach (string s in e)
                if (!dict[s].contents.Equals(""))
                    l.Add(s);
            return l;
        }

        /// <summary>
        /// If name is null or invalid, throws an InvalidNameException.
        /// 
        /// Otherwise, returns the contents (as opposed to the value) of the named cell.  The return
        /// value should be either a string, a double, or a Formula.
       /// </summary>
        public override object GetCellContents(string name)
        {
            if (!isValidName(name))
                throw new InvalidNameException();
            string normName = this.Normalize(name);
            if (!isValidName(normName))
                throw new InvalidNameException();
            //If this dictionary already has the name, then return the corresponding cell's contents.
            if (this.dict.ContainsKey(normName))
            return this.dict[normName].getContents();
            //If this dictionary doesn't have the name, then return "", like it is an empty cell.
            else return "";
                
        }

        /// <summary>
        /// If name is null or invalid, throws an InvalidNameException.
        /// 
        /// Otherwise, the contents of the named cell becomes number.  The method returns a
        /// set consisting of name plus the names of all other cells whose value depends, 
        /// directly or indirectly, on the named cell.
        /// 
        /// For example, if name is A1, B1 contains A1*2, and C1 contains B1+A1, the
        /// set {A1, B1, C1} is returned.
        /// </summary>
        protected override ISet<string> SetCellContents(string name, double number)
        {
            if (!isValidName(name))
                throw new InvalidNameException();
            
            //If this dictionary already has a cell with this name:
            if (this.dict.ContainsKey(name))
            {
                //If the cell has a formula in it, then we save the dependents from the formula in a list
                //and remove the dependencies from "this"  Dependency Graph.
                if (this.dict[name].contents is Formula)
                {
                List<String> dependees1 = new List<String>();
                Formula f = (Formula)this.dict[name].contents;
                foreach (String s in f.GetVariables())
                    dependees1.Add(s);
                foreach (String ss in dependees1)
                    dg.RemoveDependency(ss, name);
                }
                //Assosiate the new number cell with this name in the dictionary.
                this.dict[name] = new Cell(number);
            }
                
            //Else, if the name doesn't exists or exists, but it is not a formula cell, then we just add 
            //new number cell.
            else
            {
                this.dict.Add(name, new Cell(number));

            }

            //Get all the dependents of the added number cell.
            ISet<string> dependentsOfNumber = new HashSet<string>();
            foreach (String depend in this.GetCellsToRecalculate(name))
            {   
                
                dependentsOfNumber.Add(depend);
                if (depend.Equals(name))
                    continue;
                Formula f = (Formula)this.dict[depend].contents;
                    this.dict[depend].setContents(f, lookup);
               
            }

            this.Changed = true;
            return dependentsOfNumber;

        }

        /// <summary>
        /// If text is null, throws an ArgumentNullException.
        /// 
        /// Otherwise, if name is null or invalid, throws an InvalidNameException.
        /// 
        /// Otherwise, the contents of the named cell becomes text.  The method returns a
        /// set consisting of name plus the names of all other cells whose value depends, 
        /// directly or indirectly, on the named cell.
        /// 
        /// For example, if name is A1, B1 contains A1*2, and C1 contains B1+A1, the
        /// set {A1, B1, C1} is returned.
        /// </summary>
        protected override ISet<string> SetCellContents(string name, string text)
        {
            if (text == null)
                throw new ArgumentNullException();
            if (!isValidName(name))
                throw new InvalidNameException();
            
            //If this dictionary already has a cell with this name:
            if (this.dict.ContainsKey(name))
                
            {
                //If the cell has a formula in it, then we save the dependents from the formula in a list
                //and remove the dependencies from "this"  Dependency Graph.
                if (this.dict[name].contents is Formula)
                {
                List<String> dependees1 = new List<String>();
                Formula f = (Formula)this.dict[name].contents;
                foreach (String s in f.GetVariables())
                    dependees1.Add(s);
                foreach (String ss in dependees1)
                    dg.RemoveDependency(ss, name);
                }
                //Assosiate the new text cell with this name in the dictionary.
                this.dict[name] = new Cell(text);
            }

            //Else, if the name doesn't exists or exists, but it is not a formula cell, then we just add 
            //new text cell.
            else this.dict.Add(name, new Cell(text));

            //Get all the dependents of the added text cell.
            ISet<string> dependentsOfNumber = new HashSet<string>();
            foreach (String depend in this.GetCellsToRecalculate(name))
            {
                dependentsOfNumber.Add(depend);
                if (depend.Equals(name))
                    continue;
                
                this.dict[depend].value = new FormulaError();
            }

            this.Changed = true;
            return dependentsOfNumber;

        }
        /// <summary>
        /// If the formula parameter is null, throws an ArgumentNullException.
        /// 
        /// Otherwise, if name is null or invalid, throws an InvalidNameException.
        /// 
        /// Otherwise, if changing the contents of the named cell to be the formula would cause a 
        /// circular dependency, throws a CircularException.  (No change is made to the spreadsheet.)
        /// 
        /// Otherwise, the contents of the named cell becomes formula.  The method returns a
        /// Set consisting of name plus the names of all other cells whose value depends,
        /// directly or indirectly, on the named cell.
        /// 
        /// For example, if name is A1, B1 contains A1*2, and C1 contains B1+A1, the
        /// set {A1, B1, C1} is returned.
        /// </summary>
        protected override ISet<string> SetCellContents(string name, SpreadsheetUtilities.Formula formula)
        {
            //If formula is null, throw an exception.
            if (formula==null)
                throw new ArgumentNullException("The formula can't be null");

            if (!isValidName(name))
                throw new InvalidNameException();
            

            ISet<string> dependentsOfFormula = new HashSet<string>();

            //If the name already exists, we need to keep track of it in case of the circular error and we need
            //to restore the Dependency Graph.
            bool nameExists = false; 

            List<String> dependees = new List<String>();
            //If this dictionary already has a cell with this name and it is of type of Formula:
            if ((this.dict.ContainsKey(name)) && (this.dict[name].contents is Formula)) 
            {
                nameExists = true;
               
                //Save all the dependees of this name to be able to restore them later.
                Formula f = (Formula)this.dict[name].contents;
                foreach (String s in f.GetVariables())
                    dependees.Add(s);
                //Remove the dependees.
                foreach (String ss in dependees)
                    dg.RemoveDependency(ss, name);
                //Add new dependees from the new formula.
                foreach (String str in formula.GetVariables())
                {

                    dg.AddDependency(str, name);
                }

            }

            //Else, if the cell with this name doesn't exists or it is not of type Formula,
            //then add the dependees from the new Formula.
            else 

            foreach (String s in formula.GetVariables())
            
            {
                dg.AddDependency(s, name);

            }

            //Check for circular dependency by running GetCellsToRecalculate method.
            try
            {

                foreach (String depend in this.GetCellsToRecalculate(name))
                    dependentsOfFormula.Add(depend);
            }

            catch (CircularException)
            {
                //If the cell was a Fromula cell before, then restore the original dependees.
                if (nameExists == true)
                {
                    foreach (String str in formula.GetVariables())
                        dg.RemoveDependency(str, name);

                    foreach (String ss in dependees)
                        dg.AddDependency(ss, name);
                }

                //else, remove the new dependees
                else
                {
                    foreach (String str in formula.GetVariables())
                        dg.RemoveDependency(str, name);
                }

                //re-throw the exception
                throw new CircularException();
            }

            //If there is no the circular exception, then add the formula cell to this name in the dictionary.
            if (this.dict.ContainsKey(name))
            {
                this.dict[name] = new Cell(formula, lookup);
               
            }
            else
            {
                this.dict.Add(name, new Cell(formula, lookup));
                
            }

            //Calculate the value of dependent cells only if the value of a new cell is not FormulaError:
            if (!(dict[name].value is FormulaError))
            {
                foreach (String depend in dependentsOfFormula)
                {
                    //Formula f = (Formula)this.dict[depend].contents;
                    // this.dict[depend].setContents((Formula)this.dict[depend].contents, lookup);
                    this.dict[depend].setValue((Formula)this.dict[depend].contents, lookup);
                }
            }
            else
            {
                foreach (String depend in dependentsOfFormula)
                {
                   
                    this.dict[depend].value = new FormulaError();
                }
            }

            this.Changed = true;
            return dependentsOfFormula;
        }

        /// <summary>
        /// If name is null, throws an ArgumentNullException.
        /// 
        /// Otherwise, if name isn't a valid cell name, throws an InvalidNameException.
        /// 
        /// Otherwise, returns an enumeration, without duplicates, of the names of all cells whose
        /// values depend directly on the value of the named cell.  In other words, returns
        /// an enumeration, without duplicates, of the names of all cells that contain
        /// formulas containing name.
        /// 
        /// For example, suppose that
        /// A1 contains 3
        /// B1 contains the formula A1 * A1
        /// C1 contains the formula B1 + A1
        /// D1 contains the formula B1 - C1
        /// The direct dependents of A1 are B1 and C1
        /// </summary>
        protected override IEnumerable<string> GetDirectDependents(string name)
        {
            //if (name==null)
             //   throw new ArgumentNullException();
            //if (!isValidName(name))
            //    throw new InvalidNameException();
            //string normName = this.Normalize(name);
            //if (!isValidName(normName))
            //    throw new InvalidNameException();

            HashSet<String> hs=new HashSet<String>();
            foreach (String s in this.dg.GetDependents(name))
                hs.Add(s);

            return hs;

        }

        
    }
}
