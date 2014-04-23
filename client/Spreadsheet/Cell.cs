using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SpreadsheetUtilities;

namespace SS
{
    /// <summary>
    /// Class for representing a cell for the Spreadsheet. The cell has two fields: contents and value.
    /// </summary>
    public class Cell
    {
        /// <summary>
        /// Contents of a cell, which can be a string, double or a formula.
        /// </summary>
        public Object contents;
        
        /// <summary>
        /// Value of a cell, which can be a string, double or FormulaError.
        /// </summary>
        public Object value;
       
      /// <summary>
      /// Constructor for a new empty cell.  
      /// </summary>
       public Cell()
        {
            this.contents = "";
            this.value = "";
        }
        
       /// <summary>
       /// Constructor for a text cell.
       /// </summary>
       public Cell(String s)
       {
           this.contents = s;
                    
           this.value = s;

       }

        /// <summary>
        /// Constructor for a number cell.
        /// </summary>
       public Cell(double s)
       {
           this.contents = s;

           this.value = s;

       }

       /// <summary>
       /// Constructor for a formula cell.
       /// </summary>
       public Cell(Formula s, Func<string, double> lookup)
       {    
           this.contents = s;
           this.value = s.Evaluate(lookup);
       }

       /// <summary>
       /// Get contents of the cell.
       /// </summary>
        public Object getContents()
        {
            return this.contents;
        }

        /// <summary>
        /// Get contents of the String cell.
        /// </summary>
        public void setContents(String s)
        {
            this.contents = s;
            this.value = s;
        }

        /// <summary>
        /// Get contents of the Double cell.
        /// </summary>
        public void setContents(Double s)
        {
            this.contents = s;
            this.value = s;
        }

        /// <summary>
        /// Get contents of the Formula cell.
        /// </summary>
        public void setContents(Formula s, Func<string, double> lookup)
        {
            this.contents = s;
            this.value = s.Evaluate(lookup);
        }

        /// <summary>
        /// Get value of the cell.
        /// </summary>
        public Object getValue()
        {
            return this.value;
        }

        /// <summary>
        /// Set value of the cell.
        /// </summary>
        public void setValue(Formula s, Func<string, double> lookup)
        {
            this.value = s.Evaluate(lookup);
        }
        
    }
}
