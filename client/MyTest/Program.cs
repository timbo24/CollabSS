using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SS;
using SpreadsheetUtilities;
namespace MyTest
{
    class Program
    {
        static void Main(string[] args)
        {
           // AbstractSpreadsheet sheet = new Spreadsheet("sh1.xml", s => true, s => s, "basic");
           // Console.WriteLine(sheet.GetSavedVersion("sh1.xml"));
           //Console.WriteLine(sheet.GetCellContents("A1"));
           //foreach(string ss in sheet.GetNamesOfAllNonemptyCells())
          // Console.WriteLine(ss);
          // sheet.Save("c:/MyDocuments/copy1");

            //Console.WriteLine(sheet.GetCellValue("ss1"));

           // AbstractSpreadsheet sheet1 = new Spreadsheet("copy.xml", s => true, s => s, "basic");
           // sheet1.SetContentsOfCell("Cell1", "3");
           // sheet1.SetContentsOfCell("Cell2", "wow");
           // sheet1.SetContentsOfCell("Cell3", "=g1-5");
            //Console.WriteLine(sheet1.GetCellContents("A1"));
           // Console.WriteLine(sheet1.GetCellValue("Cell2"));
           // Console.WriteLine(sheet1.GetCellValue("Cell3"));
            //sheet1.SetContentsOfCell("g1", "3");
            //sheet1.SetContentsOfCell("g1", "=gn1");
           // sheet1.GetCellValue("g1");
            //sheet1.GetSavedVersion("ssh3.xml");
            //AbstractSpreadsheet sheet2 = new Spreadsheet("copy.xml", s => true, s => s, "adv");
           // AbstractSpreadsheet sheet = new Spreadsheet();
           
            //sheet.SetContentsOfCell("a1", "= a2 + a3");
            //sheet.SetContentsOfCell("a2", "= b1 + b2");

            //sheet.SetContentsOfCell("a3", "5.0");
            //sheet.SetContentsOfCell("b1", "2.0");
            //sheet.SetContentsOfCell("b2", "3.0");
            //Console.WriteLine(sheet.GetCellValue("a1"));
            //Console.WriteLine(sheet.GetCellValue("a2"));
            //sheet.SetContentsOfCell("b2", "4.0");
           // Console.WriteLine(sheet.GetCellValue("a1"));
           // Console.WriteLine(sheet.GetCellValue("a2"));

            AbstractSpreadsheet s = new Spreadsheet();
            s.SetContentsOfCell("sum1", "= a1 + a2");
            int i;
            int depth = 2;
            for (i = 1; i <= depth * 2; i += 2)
            {
                s.SetContentsOfCell("a" + i, "= a" + (i + 2) + " + a" + (i + 3));
               
                s.SetContentsOfCell("a" + (i + 1), "= a" + (i + 2) + "+ a" + (i + 3));
            }
            s.SetContentsOfCell("a" + i, "1");
            s.SetContentsOfCell("a" + (i + 1), "1");
            Console.WriteLine((double)s.GetCellValue("a1"));
            Console.WriteLine((double)s.GetCellValue("a2"));
            Console.WriteLine((double)s.GetCellValue("a3"));
            Console.WriteLine((double)s.GetCellValue("sum1"));

            Console.ReadLine();

           

        }
    }
}
