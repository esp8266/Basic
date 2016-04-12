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
    if (AllMyVariables[i].Name == VariableNameToFind)
    {
      delay(0);
       MyOut =  AllMyVariables[i].Content;
      LastVarNumberLookedUp = i;
      VariableLocated = 1;
      break;
    }
  }
  return MyOut;
}

void SetMeThatVar(String VariableNameToFind, String NewContents, int format)
{
  NewContents = GetRidOfurlCharacters(NewContents);
  for (int i = 0; i < TotalNumberOfVariables; i++)
  {
    if (AllMyVariables[i].Name == VariableNameToFind)
    {
        AllMyVariables[i].Content = NewContents;
        AllMyVariables[i].Format = format;
        return;
    }
  }

  for (int i = 0; i < TotalNumberOfVariables; i++)
  {
    if (AllMyVariables[i].Name == "")
    {
      AllMyVariables[i].Name = VariableNameToFind;
      AllMyVariables[i].Content = NewContents;
      AllMyVariables[i].Format = format;
      return;
    }
  }

}

void PrintAllMyVars()
{
  PrintAndWebOut(F("Variable Dump"));
  for (int i = 0; i < TotalNumberOfVariables; i++)
  {
    PrintAndWebOut(AllMyVariables[i].Name + String(F(" = ")) +  (AllMyVariables[i].Format == PARSER_STRING ? "\"" :"") + 
                   AllMyVariables[i].Content +                  (AllMyVariables[i].Format == PARSER_STRING ? "\"" :"") );
  }
  return;
}





