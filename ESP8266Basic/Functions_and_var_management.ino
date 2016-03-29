String GetMeThatVar(String VariableNameToFind)
{
  delay(0);
  return VarialbeLookup(VariableNameToFind);
}

String VarialbeLookup(String VariableNameToFind)
{
  VariableLocated = 0;
  String MyOut = VariableNameToFind;
  for (byte i = 0; i < 50; i++)
  {
    if (AllMyVaribles[i][0] == VariableNameToFind)
    {
      delay(0);
      MyOut =  AllMyVaribles[i][1];
      LastVarNumberLookedUp = i;
      VariableLocated = 1;
      break;
    }
  }
  return MyOut;
}

void SetMeThatVar(String VariableNameToFind, String NewContents)
{
  NewContents = GetRidOfurlCharacters(NewContents);
  byte varnotset = 1;
  for (byte i = 0; i < 50; i++)
  {
    if (AllMyVaribles[i][0] == VariableNameToFind)
    {
      AllMyVaribles[i][1] = NewContents;
      varnotset = 0;
    }
  }

  if (varnotset == 1)
  {
    for (byte i = 0; i < 50; i++)
    {
      if (AllMyVaribles[i][0] == "")
      {
        AllMyVaribles[i][0] = VariableNameToFind;
        AllMyVaribles[i][1] = NewContents;
        i = 51;
      }
    }
  }
}


String Mid(String str, int pos1, int pos2)
{
  if (str.length() < pos1 + pos2) pos2 = str.length() - pos1;
  int i;
  String temp = "";
  for (i = pos1; i < pos1 + pos2; i++)
  {
    temp += str.charAt(i);
  }

  return temp;
}



String Left(String str, int pos)
{
  int i;
  String temp = "";
  for (i = 0; i < pos; i++)
  {
    temp += str.charAt(i);
  }

  return temp;
}


String Right(String str, int pos)
{
  int i;
  String temp = "";
  for (i = strlen(str.c_str()) - pos; i < strlen(str.c_str()); i++)
  {
    temp += str.charAt(i);
  }
  return temp;
}




int StrToHex( String StringToConvert )

{
  int str_len = StringToConvert.length() + 1;
  char input[str_len];
  StringToConvert.toCharArray(input, str_len);
  return (int) strtol( input, 0, 16);
}



void PrintAllMyVars()
{
  PrintAndWebOut(F("Variable Dump"));
  for (byte i = 0; i < 50; i++)
  {
    PrintAndWebOut(String(AllMyVaribles[i][0] + " = " + AllMyVaribles[i][1]));
  }
  return;
}





