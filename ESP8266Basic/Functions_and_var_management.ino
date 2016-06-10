String GetMeThatVar(String VariableNameToFind)
{
  delay(0);
  return VarialbeLookup(VariableNameToFind);
}

String VarialbeLookup(String VariableNameToFind)
{
  VariableLocated = 0;
  String MyOut = VariableNameToFind;
  for (int i = 0; i < TotalNumberOfVariables; i++)
  {
    if (AllMyVariables[i].getName() == VariableNameToFind)
    {
      delay(0);
      MyOut =  AllMyVariables[i].getVar();
      LastVarNumberLookedUp = i;
      VariableLocated = 1;
      break;
    }
  }
  return MyOut;
}

int VariablePosition(String VariableNameToFind)
{
  for (int i = 0; i < TotalNumberOfVariables; i++)
  {
    if (AllMyVariables[i].getName() == VariableNameToFind)
      return i;
  }
  return -1;
}


void SetMeThatVar(String VariableNameToFind, String NewContents, int format)
{
  NewContents = GetRidOfurlCharacters(NewContents);
  for (int i = 0; i < TotalNumberOfVariables; i++)
  {
    if (AllMyVariables[i].getName() == VariableNameToFind)
    {
        AllMyVariables[i].setVar(NewContents);
        AllMyVariables[i].Format = format;
        WebSocketSend( "var~^`" + String(i) + "~^`" + String(AllMyVariables[i].getVar()));
        return;
    }
  }

  for (int i = 0; i < TotalNumberOfVariables; i++)
  {
    if (AllMyVariables[i].getName() == "")
    {
      AllMyVariables[i].setName(VariableNameToFind);
      AllMyVariables[i].setVar(NewContents);
      AllMyVariables[i].Format = format;
	  WebSocketSend( "var~^`" + String(i) + "~^`" + String(AllMyVariables[i].getVar()));
      return;
    }
  }

}

void PrintAllMyVars()
{
  PrintAndWebOut(F("Variable Dump"));
  for (int i = 0; i < TotalNumberOfVariables; i++)
  {
    PrintAndWebOut(AllMyVariables[i].getName() + String(F(" = ")) + (AllMyVariables[i].Format == PARSER_STRING ? "\"" :"") + 
                   AllMyVariables[i].getVar() +                     (AllMyVariables[i].Format == PARSER_STRING ? "\"" :"") );
  }
  return;
}


void deleteVariables()
{
  int i;
  for (i = 0; i < TotalNumberOfVariables; i++)
  {
    AllMyVariables[i].remove();
    AllMyVariables[i].Format = PARSER_FALSE;
  }
  for (i = 0; i < TotalNumberOfArrays; i++)
  {
    basic_arrays[i].remove();
  }
  return;    
}


