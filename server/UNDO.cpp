std::stack<std::string> *UndoStack = new std::stack<std::string>();

/*Returns a string that can be used as a spreadsheet update to undo the last change made to the spreadsheet.
 *
 *This change should not have registerOld called on it like a normal spreadsheet update or you will just 
 *keep looping back over the same changes.
 */
std::string Undoer::undo(std::string version)
{
  //save the top element
  std::string toReturn = "UPDATE\e" + version;
  toReturn += std::string(UndoStack->top());

  //pop the stack
  UndoStack->pop();

  //return the element
  return toReturn;
  
}

/*Registers the values that used to be stored in cells that are about to be changed so that they can later be
 *undone.
 *
 *Parameter is a list of variable names to register the old value of.
 */
void Undoer::registerOld(std::set<std::string> cells, std::string sheet)
{
  //variable to store the final update command to push
  std::string toPush = "";
  
  //query the old contents of each cell
  for (auto addIt = cells.begin() ; addIt != cells.end() ; ++ addIt)
  {
    //create an sql select statement
    std::string undo_query = "SELECT contents FROM cell WHERE ssname = '" + sheet + "' AND cell = '" + *addIt + "'";

    //variable to store the old cell contents
    std::string contents = "";

    //if there was an error, do something
    if ( mysql_query (con, undo_query.c_str()) )
    {
      //__________________________________________________________________________handle the error______________________________________________________________________________________________________________________________________
    }

    MYSQL_RES *cellCon;
    MYSQL_ROW resultRow;

    // Get the result of the query
    cellCon = mysql_store_result(con);
    int foundCell = mysql_num_rows(cellCon);

    //if the cell was there, save the contents
    if (foundCell == 1)
    {
      if (((resultRow = mysql_fetch_row(cellCon)) != NULL))
	{
	  contents = resultRow[0];
	}
    }

    //now append the cell name and contents to the string
    toPush += "\e" + *addIt + "\e" + contents;
    
  }
  


  //push the string of cells and contents to the stack
  if (contents != "")
  {
    UndoStack->push(toPush);
  }

}
