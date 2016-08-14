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
  String WhatIwillReturn ="";
  //SPIFFS.begin();
  File f = SPIFFS.open(String("/data/" + fileNameForSave + ".dat"), "r");
  if (!f)
  {
    fileOpenFail = 1;
    //Serial.print("file open failed  :");
    Serial.println(fileNameForSave);
  }
  else
  {
    fileOpenFail = 0;
    WhatIwillReturn =  f.readStringUntil('\r');
    WhatIwillReturn.replace("\n", "");
    f.close();  
  }
  Serial.println(String("Data Read " + WhatIwillReturn));
  return WhatIwillReturn;
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
//static int  program_nb_lines = 0;
//static uint16_t  line_seeks[256];
static File BasicFileOpened;
static File BasicFileLinePos;

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

  //char buff[200];
  String ret;
  delay(0);
  //BasicFileOpened.seek(line_seeks[linenum-1], SeekSet);
  //BasicFileOpened.readBytes(buff, line_seeks[linenum] - line_seeks[linenum-1]);
  //buff[line_seeks[linenum] - line_seeks[linenum-1]] = '\0';
  //ret = String(buff);
  uint16_t pos;
  uint8_t *p = (uint8_t *) &pos; 
  BasicFileLinePos.seek((linenum-1) *2, SeekSet);
  BasicFileLinePos.read(p,2);
  //Serial.println(pos);
  BasicFileOpened.seek(pos, SeekSet);
  ret = BasicFileOpened.readStringUntil('\n');
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
  // clear the List of the goto/gosub
  JumpList.clear();
  // clear the List of the If then else endif positions
  IfBlockList.clear();
  
  String ret;
  String ret1;
  int i = 0;
  program_nb_lines = 0;
  //SPIFFS.begin();
  BasicFileOpened.close();
  BasicFileLinePos.close();
  SPIFFS.remove(F("/linepos.bin"));
  File f = SPIFFS.open(fileNameForRead, "r");
  BasicFileLinePos = SPIFFS.open(F("/linepos.bin"), "w");
  BasicFileOpened = f;
  if (!f)
  {
    Serial.println(F("file open failed"));
  }
  else
  {
    for (i = 0; i <= TotalNumberOfLines; i++)
    {
      program_nb_lines++;      
      if (f.available() == 0) break;
      //line_seeks[program_nb_lines] = f.position();
      uint16_t pos = f.position();
      uint8_t *p = (uint8_t *) &pos;   
      BasicFileLinePos.write( p, 2);  // write the position of the line in the file as 2 bytes (max 65536 bytes)
      //Serial.println(pos);      
      ret = f.readStringUntil('\n');
      ret.replace("\n","");
      ret.replace("\r","");      
      ret.trim();
	  ret1 = ret;
	  ret.toLowerCase();
      if (ret1[0] == '[') // if starts with '['  // this is a label
      {
        // looks for the closing ']'
        int k = ret1.indexOf(']');
        if (k != -1)
          JumpList.add(ret1.substring(0, k+1), program_nb_lines);
      }

      if ( (ret.startsWith(F("if "))) && (ret.endsWith(F(" then"))) )
        IfBlockList.setIf(program_nb_lines);
      if (ret.startsWith(F("else")))
        IfBlockList.setElse(program_nb_lines);
      if ( (ret.startsWith(F("endif"))) || (ret.startsWith(F("end if"))) )
        IfBlockList.setEndif(program_nb_lines);
      
    }
//    f.close();
      BasicFileLinePos.close();
      BasicFileLinePos = SPIFFS.open(F("/linepos.bin"), "r");
      IfBlockList.check();  // check that all the endif complete the if
      JumpList.check();  // check that all jumps are completed
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

