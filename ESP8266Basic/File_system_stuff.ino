//File System Stuff

void SaveDataToFile(String fileNameForSave, String DataToSave)
{
  Serial.println(fileNameForSave);
  //SPIFFS.begin();
  File f = SPIFFS.open(String(" /data/" + fileNameForSave + ".dat"), "w");
  if (!f)
  {
    PrintAndWebOut("file open failed");
  }
  else
  {
    f.println(String(DataToSave + String('\r')));
    f.close();
  }
  return;
}



String LoadDataFromFile(String fileNameForSave)
{
  String WhatIwillReturn;
  //SPIFFS.begin();
  File f = SPIFFS.open(String(" /data/" + fileNameForSave + ".dat"), "r");
  if (!f)
  {
    fileOpenFail = 1;
    //Serial.print("file open failed  :");
    //Serial.println(fileNameForSave);
  }
  else
  {
    fileOpenFail = 0;
    WhatIwillReturn =  f.readStringUntil('\r');
    WhatIwillReturn.replace("\n", "");
    f.close();
    return WhatIwillReturn;
  }
}




String BasicProgram(int LineNumberToLookUp)
{
  if (ProgramName == "")
  {
    ProgramName = "default";
  }
  delay(0);

  return LoadDataFromFile(String(ProgramName  + "/" + String(LineNumberToLookUp))) ;
  delay(0);
}


void BasicProgramWriteLine(int LineNumberToLookUp, String DataToWriteForProgramLine)
{
  if (ProgramName == "")
  {
    ProgramName = "default";
  }
  delay(0);
  SaveDataToFile(String(ProgramName  + "/" +  String(LineNumberToLookUp)), DataToWriteForProgramLine);
  delay(0);
}
