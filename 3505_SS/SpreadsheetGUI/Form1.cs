using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using SpreadsheetUtilities;
using System.IO;
using System.Threading;


namespace SS
{
    public partial class Form1 : Form
    {

        private SpreadsheetClient model;

        /// <summary>
        /// List of all possible column names. 
        /// </summary>
        private static List<string> colName = new List<string> { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };

        /// <summary>
        /// Spreadsheet for the given form.
        /// </summary>
        private Spreadsheet sheet;

        private bool crashed = false;

       
        private string IpAddress="localhost";
        private int port=2500;
        public int version;

        /// <summary>
        /// Constructior for the empty form.
        /// </summary>
        public Form1(string ipaddress, int _port, SpreadsheetClient _model, int _version)
        {
            InitializeComponent();
            model = _model;
         //   model = new SpreadsheetClient();
         //   model.IncomingLineEvent += MessageReceived;
            model.EditLineEvent += MessageReceived;
            model.ServerCrashedLineEvent += CloseSpreadsheet;
           // model.OpenNewLineEvent += OpenNewSS;

            
            sheet = new Spreadsheet(isValid, convertToUpper, "ps6");

            spreadsheetPanel1.SelectionChanged += displaySelection;
            spreadsheetPanel1.SetSelection(0, 0);
            this.cellName.Text = "A1";
            Thread.Sleep(2000);
          //  if (!connected)
          //  {
        //    IpAddress = ipaddress;
        //    port = _port;
            version = _version;
        //        model.Connect(IpAddress, port, "password");
           //     connected = true;
           // }
        }

        private void CloseSpreadsheet(string s)
        {
            crashed = true;
            Close();
        }

        /// <summary>
        /// Constructor for the form with a given spreadsheet.
        /// </summary>
        /// <param name="_sheet">given spreadsheet</param>
        public Form1(Spreadsheet _sheet)
        {

            InitializeComponent();
            model = new SpreadsheetClient();
            model.IncomingLineEvent += MessageReceived;
            model.EditLineEvent += MessageReceived;

            sheet = _sheet;
            this.Text = sheet.filePath;
            int column, row;
               List<String> cells = new List<String>();
            foreach(string s in sheet.GetNamesOfAllNonemptyCells())
            cells.Add(s);
                Regex r = new Regex("^([a-zA-Z])([1-9][0-9]?)$");
                Match m;


                for (int i = 0; i < cells.Count; i++)
                {
                    m = r.Match(cells[i]);
                    if (m.Success)
                    {
                        column = colName.IndexOf("" + m.Groups[1], 0);
                        row = Int32.Parse("" + m.Groups[2]) - 1;

                        this.spreadsheetPanel1.SetValue(column, row, (sheet.GetCellValue(cells[i])).ToString());
                    }
                }

            spreadsheetPanel1.SelectionChanged += displaySelection;
            spreadsheetPanel1.SetSelection(0, 0);

            this.cellName.Text = "A1";
            this.cellValue.Text = sheet.GetCellValue("A1").ToString();
            this.cellContent.Text = sheet.GetCellContents("A1").ToString();

            model.Connect(IpAddress, port, "PASSWORD");

        }

        
        /// <summary>
        /// Every time the selection changes, this method is called with the
        /// Spreadsheet as its parameter.  Displays contents and value of the selected cell.
        /// </summary>
        private void displaySelection(SpreadsheetPanel ss)
        {
            int row, col;
            String value;
            ss.GetSelection(out col, out row);
           
            ss.GetValue(col, row, out value);

            cellName.Text = "" + colName[col] + (row + 1);
            
            if(sheet.GetCellContents("" + colName[col] + (row + 1)) is SpreadsheetUtilities.Formula)
                 cellContent.Text = "="+(sheet.GetCellContents("" + colName[col] + (row + 1)));
            else cellContent.Text = sheet.GetCellContents("" + colName[col] + (row + 1)).ToString();

            cellValue.Text = "" + value;
            FormulaError er;
            if (sheet.GetCellValue("" + colName[col] + (row + 1)) is FormulaError)
            {
                er = (FormulaError)(sheet.GetCellValue("" + colName[col] + (row + 1)));
                cellValue.Text = "Error: " + er.Reason;
            }
        }

        /// <summary>
        /// Forwards received message to value box
        /// </summary>
        /// <param name="line"></param>
        private void MessageReceived(String line)
        {
            if (!(line == null))
            {
                string[] tokens = line.Split((Char)27);
               // cellValue.Invoke(new Action(() => { cellValue.Text = line + "\r\n"; }));
                if (tokens[0] == "UPDATE" || tokens[0] == "SYNC")
                {
                    string vnumber = tokens[1];
                    int vnum = 0;
                   
                    try
                    {
                        vnum = Convert.ToInt32(vnumber);
                    }
                    catch
                    {
                        Console.WriteLine("Input string for a version number is not a sequence of digits.");
                    }

                    if (tokens.Length < 5)
                    {
                        Console.WriteLine("Client Version: " + version);
                        Console.WriteLine("Server Version: " + vnumber);
                    }

		    //______________________________________________________________________Start Mark's Edits
		    //If the message is a sync, clear the sheet
                        if (tokens[0] == "SYNC")
                        {
                            this.sheet = new Spreadsheet(isValid, convertToUpper, vnumber);
                            this.spreadsheetPanel1.Clear();
                        }

                        //get the client version number as an int
                        //int cvnum = Convert.ToInt32(sheet.Version);

                        //Check the version number.
                        if (tokens.Length < 5 && vnum != (1 + version) && tokens[0] != "SYNC")
			            {
                                model.SendMessageSync("RESYNC");
			            }
		                else
		                {
		    	            version = vnum;	
 
		                //______________________________________________________________________________End Mark's Edits   

                                for (int k = 2; k < tokens.Length - 1; k += 2)
                                {
                       
                                        string part1 = tokens[k];
                                        string part2 = tokens[k+1];
                                        // cellValue.Invoke(new Action(() => { cellValue.Text = part1+part2 + "\r\n"; }));
                                        //  sheet.SetContentsOfCell(part1, part2);

                                        Regex r = new Regex("^([a-zA-Z])([1-9][0-9]?)$");
                                        Match m;
                                        int col = 0, row = 0;

                                        m = r.Match(part1);
                                        if (m.Success)
                                        {
                                            col = colName.IndexOf("" + m.Groups[1], 0);
                                            row = Int32.Parse("" + m.Groups[2]) - 1;
                                        }

                                        //this.spreadsheetPanel1.SetValue(col, row, part2);


                                        List<String> names = new List<String>(); //list of names of a cell's dependents

                                        try
                                        {
                                            //Setting contents of a cell and adding all its dependents to the names list.
                                            foreach (String s in sheet.SetContentsOfCell("" + colName[col] + (row + 1), part2))
                                            {
                                                names.Add(s);
                                            }

                                            //Display error message if the value is a FormulaError.
                                            FormulaError er;
                                            if (sheet.GetCellValue("" + colName[col] + (row + 1)) is FormulaError)
                                            {
                                                er = (FormulaError)(sheet.GetCellValue("" + colName[col] + (row + 1)));
                                                cellValue.Text = "Error: " + er.Reason;
                                                //set the cell's value to a FormulaError
                                                this.spreadsheetPanel1.SetValue(col, row, "FormulaError");
                                            }
                                            //Display a cell's value if it is not a FormulaError.
                                            else
                                            {
                                                this.spreadsheetPanel1.SetValue(col, row, (sheet.GetCellValue("" + colName[col] + (row + 1)).ToString()));

                                                //Displaying the value of edited cell in the cellValue text box.
                                                String value;
                                                this.spreadsheetPanel1.GetValue(col, row, out value);
                                                cellValue.Text = "" + value;
                                            }

                                            int colDependent, rowDependent;

                                            String dependCells = names[0] + ", ";
                                            //Update all the dependent cells.
                                            for (int i = 1; i < names.Count; i++)
                                            {
                                                dependCells = dependCells + names[i] + ", ";
                                                m = r.Match(names[i]);
                                                if (m.Success)
                                                {
                                                    colDependent = colName.IndexOf("" + m.Groups[1], 0);
                                                    rowDependent = Int32.Parse("" + m.Groups[2]) - 1;

                                                    //Display FromulaError if the value of the dependent cell is FormulaError.
                                                    if (sheet.GetCellValue(names[i]) is FormulaError)
                                                    {
                                                        er = (FormulaError)(sheet.GetCellValue(names[i]));
                                                        cellValue.Text = "Error: " + er.Reason;
                                                        this.spreadsheetPanel1.SetValue(colDependent, rowDependent, "FormulaError");
                                                    }
                                                    //Display the dependent cell value if it is not FormulaError.
                                                    else this.spreadsheetPanel1.SetValue(colDependent, rowDependent, (sheet.GetCellValue(names[i])).ToString());
                                                }

                                            }
                                            changedCells.Text = dependCells;


                                        }
                                        catch (Exception ex)
                                        {
                                            cellValue.Text = ex.Message;
                                        }

                                    }
                        
                    
		                        }
                }
                
            }
        }

       
        /// <summary>
        /// Deals with the New menu
        /// </summary>
        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // Tell the application context to run the form on the same
            // thread as the other forms.
            DemoApplicationContext.getAppContext().RunForm(new Form1(IpAddress, port, model, version));
        }

        /// <summary>
        ///  Deals with the Close menu
        /// </summary>
        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        /// <summary>
        /// Sets contents of a cell when user enters it by "Enter" key; calculates and displays the cell's and its dependents values.
        /// </summary>
        private void cellContent_KeyPress(object sender, KeyPressEventArgs e)
        {
            //If a user pressed Enter key while in a cell content editing area:
            if (e.KeyChar == 13)
            {
                //To remove a sound when Enter key is pressed:
                e.Handled = true;

                // Get the cell coordinates
                int col, row;
                this.spreadsheetPanel1.GetSelection(out col, out row);

                string cellName = "" + colName[col] + (row + 1);

               // if (!connected)
               // {
                    // Send the cell and value
               //     model.Connect("localhost", 2000, "PASSWORD");
               //     connected = true;
                    
              //  }
               // cellValue.Invoke(new Action(() => { cellValue.Text = col+row + "\r\n"; }));
                model.SendMessageEdit("ENTER" + (Char)27 + version + (Char)27 + cellName + (Char)27 + cellContent.Text+ "\n");
            }
        }

        private void undo_Click(object sender, EventArgs e)
        {
            model.SendUndo("UNDO" + (Char)27 + version + "\n");
        }

        /// <summary>
        /// Checks if a given name is a valid cell name - starts with a letter, followed by one or 2 digits.
        /// </summary>
        /// <param name="s">string name of a cell</param>
        /// <returns>boolean</returns>
        private bool isValid(string s)
        {
            Regex r = new Regex("^([a-zA-Z])([1-9][0-9]?)$");
            Match m;
            m = r.Match(s);
            if (m.Success)
                return true;
            else return false;

        }

        /// <summary>
        /// Method for normalizing cell's names by converting them to an upper case. This way, the names are not case sensitive.
        /// </summary>
        private string convertToUpper(string s)
        {
            return s.ToUpper();
        }

        /// <summary>
        /// Saves the form. If the form was saved before, it saves it with the same name. If it wasn't saved before, opens SaveAs dialog.
        /// </summary>
        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Text = "saving ...";
          
            Thread.Sleep(300);
         

            model.SaveRequest("SAVE" + (Char)27 + version + "\n");
            this.Text = "";

        }

        /// <summary>
        /// Creates a spreadsheet from a specified XML file and displays the sheet in a Form.
        /// </summary>
        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            String file;
           
            DialogResult result = openFileDialog1.ShowDialog();

            if (result == DialogResult.OK)
            {
                file = openFileDialog1.FileName;

                //Try to create a spreadsheet from the file and run the form.
                try
                {
                    Spreadsheet newSheet = new Spreadsheet(file, isValid, convertToUpper, "ps6");
                    
                    DemoApplicationContext.getAppContext().RunForm(new Form1(newSheet));
                    
                }
                //Catch any errors while opening the file and creating the spreadsheet.
                catch (SpreadsheetReadWriteException rw)
                {
                    MessageBox.Show(rw.Message);
                }
            }
        }

        /// <summary>
        /// When a user closes a form and the form was changed, he is asked to confirm to close or first save it.
        /// If he says yes to save, but then clicks cancel, he will be asked again if he wants to save it.
        /// </summary>
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (crashed)
            {
                switch (MessageBox.Show("The Server has crashed would you like to connect again?", "Spreadsheet Utility",
                        MessageBoxButtons.YesNo, MessageBoxIcon.Warning))
                {
                    case DialogResult.Yes:
                        return;
                    case DialogResult.No:
                        model.CloseLogin();
                        return;
                        break;
                }
                // String file;
                if (sheet.Changed)
                {
                    //If a user said cancel while in the save dialog box, we need to keep asking him if he still wants to save.
                    //We do the loop while the user keeps saying cancel.
                    Boolean isCanceled = true;
                    while (isCanceled)
                    {
                        switch (MessageBox.Show("Would you like to save your changes?", "Spreadsheet Utility",
                            MessageBoxButtons.YesNoCancel, MessageBoxIcon.Warning))
                        {


                            case DialogResult.Yes:
                                {

                                    model.SaveRequest("SAVE" + (Char)27 + version + "\n");
                                    switch (MessageBox.Show("Do you still want to exit?", "Spreadsheet Utility",
                           MessageBoxButtons.YesNo, MessageBoxIcon.Warning))
                                    {
                                        case DialogResult.Yes:
                                            return;

                                        case DialogResult.No:
                                            e.Cancel = true;
                                            model.DisconnectRequest("DISCONNECT\n");
                                            return;
                                    }

                                }
                                break;
                            case DialogResult.No:
                                model.DisconnectRequest("DISCONNECT\n");
                                return;
                            case DialogResult.Cancel:
                                e.Cancel = true;
                                return;
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Opens SaveAs dialog; user chooses a name for the file and saves it. He is asked to confirm to overwrite a file or create a new one.
        /// </summary>
        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            String file;
            
            DialogResult result = saveFileDialog1.ShowDialog();

            if (result == DialogResult.OK)
            {


                file = saveFileDialog1.FileName;
                
                sheet.Save(file);

                this.Text = file;

            }
        }

        /// <summary>
        /// Opens a help window.
        /// </summary>
        private void helpToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show("Please move from cell to cell using a mouse. After entering a cell's contents inside the contents box, press Enter on your keyboard.\n\n"+
                        "The spreadsheet has the following fields:\n\n" +
                         "cell's name: displays the name of a selected cell;\n\n" +
                         "value: displays a value of a selected cell or an Error message if the value can't be calculated;\n\n" +  
                         "contents: click on this field, type a value for the selected cell, and click Enter on your keyboard;\n\n" +  
                         "changed cells: displays names of all the cells, whose value was changed during the last Enter of any cell's contents.\n\n"+
                         "The file menu has the folowing items:\n\n"+
                         "New: open a new form in a separate window;\n\n"+
                         "Open: open an XML file in a separate window;\n\n"+
                         "Close: close the current spreadsheet. If there is an unsaved information, it will ask you if you want to save it. If you clicked yes, but then clicked cancel in a Save As dialog box, your information will not be lost; you will be returned to your form. You can also close your spreadsheet by using X(top right) button. \n" +
                         "It will also give you a choice of saving your work.\n\n"+
                         "Save: saves your spreadsheet if it was saved before or opens Save As dialog for you to enter a name for it.\n\n"+
                         "SaveAs: opens Save As dialog for you to choose a name for your spreadsheet.\n\n"+
                         "If your spreadsheet is saved, its name will be shown on top of your form."
            , "Spreadsheet help");
        }

        private void cellValue_TextChanged(object sender, EventArgs e)
        {

        }

        private void fileToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void spreadsheetPanel1_Load(object sender, EventArgs e)
        {

        }

       

       

    }
}
