//File System Stuff

void SaveDataToFile(String fileNameForSave, String DataToSave)
{
  Serial.println(fileNameForSave);
  //SPIFFS.begin();
  File f = SPIFFS.open(String("/data/" + fileNameForSave + ".dat"), "w");
  if (!f)
  {
    PrintAndWebOut(F("file open failed"));
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
  File f = SPIFFS.open(String("/data/" + fileNameForSave + ".dat"), "r");
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



/*
String BasicProgram___(int LineNumberToLookUp)
{
  if (ProgramName == "")
  {
    ProgramName = F("default");
  }
  delay(0);

  return LoadDataFromFile(String(ProgramName  + "/" + String(LineNumberToLookUp))) ;
  delay(0);
}


void BasicProgramWriteLine(int LineNumberToLookUp, String DataToWriteForProgramLine)
{
  if (ProgramName == "")
  {
    ProgramName = F("default");
  }
  delay(0);
  SaveDataToFile(String(ProgramName  + "/" +  String(LineNumberToLookUp)), DataToWriteForProgramLine);
  delay(0);
}


void LoadBasicProgramFromFlash__(String fileNameForSave)
{
  String lineToBeWrittenToFile;
  SPIFFS.begin();
  File f = SPIFFS.open(String("uploads/" + fileNameForSave), "r");
  if (!f)
  {
    PrintAndWebOut(F("file open failed"));
  }
  else
  {
    for (int i = 0; i <= TotalNumberOfLines; i++)
    {

      lineToBeWrittenToFile = f.readStringUntil('\r');
      lineToBeWrittenToFile.replace("\n", "");
      BasicProgramWriteLine(i,lineToBeWrittenToFile );
    }
    f.close();
  }
  return;
}
*/
//////////// New file stuff by CiccioCB /////////////

static File BasicFileToSave;
static int  program_nb_lines = 0;
static uint16_t  line_seeks[256];
static File BasicFileOpened;

String BasicProgram(int linenum)
{
  if (linenum >= program_nb_lines)
  {
	fileOpenFail = 1; 
	return "";
  }
  else
	fileOpenFail = 0; 
  
  if (linenum == 0)
		return "";

  char buff[200];
  String ret;
  BasicFileOpened.seek(line_seeks[linenum-1], SeekSet);
  BasicFileOpened.readBytes(buff, line_seeks[linenum] - line_seeks[linenum-1]);
  buff[line_seeks[linenum] - line_seeks[linenum-1]] = '\0';
  ret = String(buff);
  ret.replace("\n","");
  ret.replace("\r","");
  
  return ret;
}

bool OpenToWriteOnFlash(String fileNameForWrite)
{
  BasicFileToSave = SPIFFS.open(fileNameForWrite, "w");
  if (!BasicFileToSave)
  {
    Serial.println(F("file write open failed"));
	return false;
  }
  return true;
}

bool WriteBasicLineOnFlash(String codeLine)
{
	int ret;
//	codeLine.replace("\r","");
//	codeLine.replace("\n","");
//Serial.println(codeLine);
	ret = BasicFileToSave.println(codeLine);
	if (ret == 0)
	{
		Serial.println(F("file write println failed"));
		return false;	
	}
	else
	{
	   return true;
	}	   

}

void CloseWriteOnFlash(void)
{
	BasicFileToSave.close();
}


void LoadBasicProgramFromFlash(String fileNameForRead)
{
  int i = 0;
  program_nb_lines = 0;
  //SPIFFS.begin();
  BasicFileOpened.close();
  File f = SPIFFS.open(fileNameForRead, "r");
  BasicFileOpened = f;
  if (!f)
  {
    Serial.println(F("file open failed"));
  }
  else
  {
    for (i = 0; i <= TotalNumberOfLines; i++)
    {
      if (f.available() == 0) break;
      line_seeks[program_nb_lines] = f.position();
      f.readStringUntil('\n');

      program_nb_lines++;
    }
//    f.close();
  }
  return;
}


String MakeSureFileNameStartsWithSlash(String FileNameToCheckForSlash)
{
  if (FileNameToCheckForSlash.startsWith("/"))
  {
    return FileNameToCheckForSlash;
  }
  else
  {
    return String("/" + FileNameToCheckForSlash);
  }
}

