String GetMeThatVar(String VariableNameToFind)
{
  //PrintAndWebOut(String("Looking for variable " + VariableNameToFind));
  byte i2cPinNo;
  String FunctionName;

  String Param0;
  String Param1;
  String Param2;
  String Param3;
  String Param4;
  String Param5;
  String Param6;
  String Param7;

  if (VariableNameToFind.endsWith(")") == 1)
  {
    int FirstParenthasy = VariableNameToFind.indexOf('(');
    int LastParenthasy = VariableNameToFind.indexOf(')');

    FunctionName = VariableNameToFind.substring(0, FirstParenthasy);
    VariableNameToFind = VariableNameToFind.substring(FirstParenthasy + 1, LastParenthasy );

    Param0 = GetMeThatVar(getValue(String(VariableNameToFind + ","), ',', 0));
    Param1 = GetMeThatVar(getValue(String(VariableNameToFind + ","), ',', 1));
    Param2 = GetMeThatVar(getValue(String(VariableNameToFind + ","), ',', 2));
    Param3 = GetMeThatVar(getValue(String(VariableNameToFind + ","), ',', 3));
    Param4 = GetMeThatVar(getValue(String(VariableNameToFind + ","), ',', 4));
    Param5 = GetMeThatVar(getValue(String(VariableNameToFind + ","), ',', 5));
    Param6 = GetMeThatVar(getValue(String(VariableNameToFind + ","), ',', 6));
    Param7 = GetMeThatVar(getValue(String(VariableNameToFind + ","), ',', 7));


    Param0.replace(",", "");
    Param1.replace(",", "");
    Param2.replace(",", "");
    Param3.replace(",", "");
    Param4.replace(",", "");
    Param5.replace(",", "");
    Param6.replace(",", "");
    Param7.replace(",", "");

    Param0 = VarialbeLookup(Param0);
    Param1 = VarialbeLookup(Param1);
    Param2 = VarialbeLookup(Param2);
    Param3 = VarialbeLookup(Param3);
    Param4 = VarialbeLookup(Param4);
    Param5 = VarialbeLookup(Param5);
    Param6 = VarialbeLookup(Param6);
    Param7 = VarialbeLookup(Param7);

    FunctionName.toLowerCase();
    FunctionName.trim();
  }
  delay(0);

  String MyOut;
  MyOut = VarialbeLookup(VariableNameToFind);

  if (FunctionName == "millis") MyOut = String(millis());


  if (FunctionName == "htmlvar")
  {
    VarialbeLookup(Param0);
    MyOut = String("VARS|" + String(LastVarNumberLookedUp));
  }



  if (FunctionName == "instr")
  {
    int junk = Param0.indexOf(Param1);
    junk++;
    MyOut = String(junk );
  }
  if (FunctionName == "len")     MyOut = String(MyOut.length());

  if (FunctionName == "upper"  | FunctionName == "upper$")     MyOut.toUpperCase();
  if (FunctionName == "lower"  | FunctionName == "lower$")     MyOut.toLowerCase();


  if (FunctionName == "mid"   | FunctionName == "mid$")     MyOut = Mid(Param0, Param1.toInt() - 1, Param2.toInt());
  if (FunctionName == "right" | FunctionName == "right$")   MyOut = Right(Param0, Param1.toInt());
  if (FunctionName == "left"  | FunctionName == "left$")    MyOut = Left(Param0, Param1.toInt());
  if (FunctionName == "replace" | FunctionName == "replace$")
  {
    MyOut = Param0;
    MyOut.replace(Param1,   Param2);
  }

  if (FunctionName == "chr" | FunctionName == "chr$")
  {
    MyOut = char(Param0.toInt());
  }
  if (FunctionName == "asc")
  {
    char bla = Param0.charAt(0);
    int blabla = bla ;
    MyOut = String(blabla);
  }
  if (FunctionName == "int")
  {
    MyOut = String(Param0.toInt());
  }

  if (FunctionName == "wget")
  {
    MyOut = FetchWebUrl(Param0);
  }

  if (FunctionName == "sendts")
  {
    //ThingsSpeekBasicSendData(String myChannelNumber, String myWriteAPIKey, String BasicTSf1,String BasicTSf2, String BasicTSf3, String BasicTSf4 )
    MyOut =  FetchWebUrl(String("api.thingspeak.com/update?key=" + Param0 + "&field" + Param1 + "=" + Param2));
  }

  if (FunctionName == "readts")
  {
    MyOut =  FetchWebUrl(String("api.thingspeak.com/channels/" + Param1 + "/field/" + Param2 + "/last.xml?api_key=" + Param0));
    MyOut = MyOut.substring(MyOut.indexOf(String("<field" + Param2 + ">") ) + 8, MyOut.indexOf(String("</field" + Param2 + ">")));
    //MyOut = MyOut.substring(0, );

  }



  if (FunctionName == "id")
  {
    MyOut = String(ESP.getChipId());
  }


  if (FunctionName == "htmlid")
  {
    MyOut = LastElimentIdTag;
  }


  if (FunctionName == "time")
  {

    time_t now = time(nullptr);
    MyOut = String(ctime(&now));
  }


  if (FunctionName == "timesetup")
  {
    timezone = Param0.toInt();
    int dst = Param1.toInt();
  }


  if (FunctionName == "io")
  {
    MyOut = UniversalPinIO(Param0, Param1,  Param2.toFloat());
  }





  if (FunctionName == "flashfree")
  {
    struct FSInfo {
      size_t totalBytes;
      size_t usedBytes;
      size_t blockSize;
      size_t pageSize;
      size_t maxOpenFiles;
      size_t maxPathLength;
    };

    SPIFFS.info(fs_info);
    int Flashfree = fs_info.totalBytes - fs_info.usedBytes;
    MyOut = String(Flashfree );
  }




  if (FunctionName == "ramfree")
  {
    MyOut = String(ESP.getFreeHeap());
  }




  if (FunctionName == "i2c.begin")
  {
    i2cPinNo = Param0.toInt();
    MyOut = "";
    Wire.beginTransmission(i2cPinNo);
  }
  if (FunctionName == "i2c.write")       MyOut = String(Wire.write(Param0.toInt()));
  //if (FunctionName == "i2c.write")       MyOut =  String(Wire.write(Param0.c_str()));
  if (FunctionName == "i2c.end")         MyOut =  String(Wire.endTransmission());
  if (FunctionName == "i2c.requestfrom")
  {
    i2cPinNo = Param0.toInt();
    byte i2cParamB = Param1.toInt();
    MyOut =  String(Wire.requestFrom(i2cPinNo, i2cParamB));
  }
  if (FunctionName == "i2c.available")   MyOut =  String(Wire.available());
  if (FunctionName == "i2c.read")        MyOut =  String(Wire.read());



  if (FunctionName == "sqr")   MyOut = String(sqrt(MyOut.toFloat()));
  if (FunctionName == "sin")   MyOut = String(sin(MyOut.toFloat()));
  if (FunctionName == "cos")   MyOut = String(cos(MyOut.toFloat()));
  if (FunctionName == "tan")   MyOut = String(tan(MyOut.toFloat()));
  if (FunctionName == "log")   MyOut = String(log(MyOut.toFloat()));

  if (FunctionName == "hex" | FunctionName == "hex$")   MyOut = String(MyOut.toInt(), HEX);
  if (FunctionName == "oct" | FunctionName == "oct$")   MyOut = String(MyOut.toInt(), OCT);


  if (FunctionName == "ip")    MyOut = String(WiFi.localIP().toString());



  if (FunctionName == "rnd")
  {
    randomSeed(millis());
    MyOut = String(random(MyOut.toFloat()));
  }


  delay(0);

  //  Serial.println("VariableNameToFind=");
  //  Serial.println(VariableNameToFind);


  return MyOut;
}

String VarialbeLookup(String VariableNameToFind)
{
  VariableLocated = 0;
  String MyOut = VariableNameToFind;
  for (byte i = 0; i <= 50; i++)
  {
    if (AllMyVaribles[i][1] == VariableNameToFind)
    {
      delay(0);
      MyOut =  AllMyVaribles[i][2];
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
  for (byte i = 0; i <= 50; i++)
  {
    if (AllMyVaribles[i][1] == VariableNameToFind)
    {
      AllMyVaribles[i][2] = NewContents;
      varnotset = 0;
    }
  }

  if (varnotset == 1)
  {
    for (byte i = 0; i <= 50; i++)
    {
      if (AllMyVaribles[i][1] == "")
      {
        AllMyVaribles[i][1] = VariableNameToFind;
        AllMyVaribles[i][2] = NewContents;
        i = 51;
      }
    }
  }
}


String Mid(String str, int pos1, int pos2)
{
  //  Serial.println("Doing mid");
  //  Serial.println(pos1);
  //  Serial.println(pos2);
  //  Serial.println(str);

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
  for (i = strlen(str.c_str()) - pos; i <= strlen(str.c_str()); i++)
  {
    temp += str.charAt(i);
  }
  return temp;
}



void PrintAllMyVars()
{
  PrintAndWebOut("Variable Dump");
  for (byte i = 0; i <= 50; i++)
  {
    PrintAndWebOut(String(AllMyVaribles[i][1] + " = " + AllMyVaribles[i][2]));
  }
  return;
}


