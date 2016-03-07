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
//  String Param5;
//  String Param6;
//  String Param7;

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
    //    Param5 = GetMeThatVar(getValue(String(VariableNameToFind + ","), ',', 5));
    //    Param6 = GetMeThatVar(getValue(String(VariableNameToFind + ","), ',', 6));
    //    Param7 = GetMeThatVar(getValue(String(VariableNameToFind + ","), ',', 7));


    Param0.replace(",", "");
    Param1.replace(",", "");
    Param2.replace(",", "");
    Param3.replace(",", "");
    Param4.replace(",", "");
    //    Param5.replace(",", "");
    //    Param6.replace(",", "");
    //    Param7.replace(",", "");

    Param0 = VarialbeLookup(Param0);
    Param1 = VarialbeLookup(Param1);
    Param2 = VarialbeLookup(Param2);
    Param3 = VarialbeLookup(Param3);
    Param4 = VarialbeLookup(Param4);
    //    Param5 = VarialbeLookup(Param5);
    //    Param6 = VarialbeLookup(Param6);
    //    Param7 = VarialbeLookup(Param7);

    FunctionName.toLowerCase();
    FunctionName.trim();
  }
  delay(0);

  String MyOut;
  MyOut = VarialbeLookup(VariableNameToFind);

  if (FunctionName == F("millis")) MyOut = String(millis());


  if (FunctionName == F("htmlvar"))
  {
    VarialbeLookup(Param0);
    MyOut = String(F("VARS|")) + String(LastVarNumberLookedUp);
  }



  if (FunctionName == F("wifi.scan")) MyOut =  String(WiFi.scanNetworks() + 1);

  if (FunctionName == F("wifi.ssid")) MyOut =  String(WiFi.SSID(MyOut.toInt() - 1));
  if (FunctionName == F("wifi.rssi")) MyOut =  String(WiFi.RSSI(MyOut.toInt() - 1));



  if (FunctionName == F("instr"))
  {
    int junk = Param0.indexOf(Param1);
    junk++;
    MyOut = String(junk );
  }
  if (FunctionName == F("len"))     MyOut = FloatToString(MyOut.length());

  if (FunctionName == F("upper")  | FunctionName == F("upper$"))     MyOut.toUpperCase();
  if (FunctionName == F("lower")  | FunctionName == F("lower$"))     MyOut.toLowerCase();


  if (FunctionName == F("mid")   | FunctionName == F("mid$"))     MyOut = Mid(Param0, Param1.toInt() - 1, Param2.toInt());
  if (FunctionName == F("right") | FunctionName == F("right$"))   MyOut = Right(Param0, Param1.toInt());
  if (FunctionName == F("left")  | FunctionName == F("left$"))    MyOut = Left(Param0, Param1.toInt());
  if (FunctionName == F("replace") | FunctionName == F("replace$"))
  {
    MyOut = Param0;
    MyOut.replace(Param1,   Param2);
  }

  if (FunctionName == F("chr") | FunctionName == F("chr$"))
  {
    MyOut = char(Param0.toInt());
  }
  if (FunctionName == F("asc"))
  {
    char bla = Param0.charAt(0);
    int blabla = bla ;
    MyOut = String(blabla);
  }
  if (FunctionName == F("int"))
  {
    MyOut = FloatToString(Param0.toInt());
  }

  if (FunctionName == F("wget"))
  {
    MyOut = FetchWebUrl(Param0);
  }

  if (FunctionName == F("readopenweather"))
  {
    MyOut = FetchOpenWeatherMapApi(Param0,Param1);
  }
  
  if (FunctionName == F("sendts"))
  {
    //ThingsSpeekBasicSendData(String myChannelNumber, String myWriteAPIKey, String BasicTSf1,String BasicTSf2, String BasicTSf3, String BasicTSf4 )
    FetchWebUrl(String(F("api.thingspeak.com/update?key=")) + Param0 + "&field" + Param1 + "=" + Param2);
  }

  if (FunctionName == F("readts"))
  {
    MyOut =  FetchWebUrl(String(F("api.thingspeak.com/channels/")) + Param1 + "/field/" + Param2 + "/last.xml?api_key=" + Param0);
    MyOut = MyOut.substring(MyOut.indexOf(String("<field" + Param2 + ">") ) + 8, MyOut.indexOf(String("</field" + Param2 + ">")));
    //MyOut = MyOut.substring(0, );

  }

  // make an "evaluation" of the argument; it's calculated in double precision but shown in single!; 
  // it can include functions and variables but, it shall not contains spaces between (this is not a a limitation of the parser) but the way the lines are handled
  // the function is included in the file Eval.ino
  if (FunctionName == F("eval"))
  {
    // here we try to isolate properly the argument for eval as Param0 don't give a good result
    // several steps :
    // step 1: remove "eval"
    Line_For_Eval = GetRidOfurlCharacters(Line_For_Eval.substring(Line_For_Eval.indexOf("eval")+4));
//    Serial.print("line_eval:");
//    Serial.println(Line_For_Eval);
    // step 2: count the parenthesis until the difference between opened and closed is 0 ( or when we reach the end of the line -> in that case error!)
    int i;
    int cnt = 0;
    for (i = 0; i < Line_For_Eval.length(); i++)
    {
      if (Line_For_Eval[i] == '(')
        cnt++;
      else
        if (Line_For_Eval[i] == ')')
          cnt--;
      if (cnt == 0) 
        break;      
    }
    if (cnt == 0)
    {
      String argument = Line_For_Eval.substring(0, i+1);
      double r = evaluate(argument);
      if (_parser_failed == true)
      {
        PrintAndWebOut(String(_parser_error_msg)); 
       return F("error");
      }
      else
        return FloatToString(r);      
    }
    else
      return F("Incorrect parenthesys");  
  }
  
  if (FunctionName == F("json"))
  {
      MyOut = Parsifal(Param0,Param1);
  }


  if (FunctionName == F("id"))
  {
    MyOut = String(ESP.getChipId());
  }


  if (FunctionName == F("htmlid"))
  {
    MyOut = LastElimentIdTag;
  }

  if ((FunctionName == F("unixtime")) || (FunctionName == F("time")))
  {
    time_t now;
    if (FunctionName == F("time"))
    {
      now = time(nullptr);
    }
    else
    {
      now = Param0.toInt();
      Param0 = Param1;
    }
    
    MyOut = String(ctime(&now));
    Param1 = MyOut;
    Param0.toUpperCase();
    Param0.trim();

    if (Param0 != "")
    {
      MyOut = Param0 ;
      MyOut.replace(F("TIME"),  String(Mid(Param1, 11, 8)));
      MyOut.replace(F("DOW"),   String(Mid(Param1, 0, 3)));
      MyOut.replace(F("MONTH"), String(Mid(Param1, 4, 3)));
      MyOut.replace(F("DATE"),  String(Mid(Param1, 8, 2)));
      MyOut.replace(F("HOUR"),  String(Mid(Param1, 11, 2)));
      MyOut.replace(F("MIN"),   String(Mid(Param1, 14, 2)));
      MyOut.replace(F("SEC"),   String(Mid(Param1, 17, 2)));
      MyOut.replace(F("YEAR"),  String(Mid(Param1, 20, 4)));
    }


    //    if (Param0 == "TIME")  MyOut = String(Mid(MyOut, 11, 8));
    //    if (Param0 == "DOW")   MyOut = String(Mid(MyOut, 0, 3));
    //    if (Param0 == "MONTH") MyOut = String(Mid(MyOut, 4, 3));
    //    if (Param0 == "DATE")  MyOut = String(Mid(MyOut, 8, 2));
    //    if (Param0 == "HOUR")  MyOut = String(Mid(MyOut, 11, 2));
    //    if (Param0 == "MIN")   MyOut = String(Mid(MyOut, 14, 2));
    //    if (Param0 == "SEC")   MyOut = String(Mid(MyOut, 17, 2));
    //    if (Param0 == "YEAR")  MyOut = String(Mid(MyOut, 20, 4));
  }



  if (FunctionName == F("timesetup"))
  {

    SaveDataToFile("TimeZone", Param0);
    SaveDataToFile("DaylightSavings", Param1);
    configTime(LoadDataFromFile("TimeZone").toFloat() * 3600, LoadDataFromFile("DaylightSavings").toInt(), "pool.ntp.org", "time.nist.gov");
  }


  if (FunctionName == F("io"))
  {
    MyOut = UniversalPinIO(Param0, Param1,  Param2.toFloat());
  }




  if (FunctionName == "neo")
  {
    pixels.setPixelColor(Param0.toInt(), pixels.Color(Param1.toInt(), Param2.toInt(), Param3.toInt()));

    pixels.show();
  }


  if (FunctionName == "neostripcolor")
  {
    for (int LedNo = Param0.toInt(); LedNo <= Param1.toInt()& LedNo < 255  ; LedNo++) {
      pixels.setPixelColor(LedNo, pixels.Color(Param2.toInt(), Param3.toInt(), Param4.toInt()));
      delay(0);
    }
    pixels.show();
  }



  if (FunctionName == "neocls")
  {
    for (int LedNo = 0; LedNo < 255  ; LedNo++) {
      pixels.setPixelColor(LedNo, pixels.Color(0, 0, 0));
      delay(0);
    }
    pixels.show();
  }



  if (FunctionName == F("flashfree"))
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




  if (FunctionName == F("ramfree"))
  {
    MyOut = String(ESP.getFreeHeap());
  }




  if (FunctionName == F("i2c.begin"))
  {
    i2cPinNo = Param0.toInt();
    MyOut = "";
    Wire.beginTransmission(i2cPinNo);
  }
  if (FunctionName == F("i2c.write"))       MyOut = String(Wire.write(Param0.toInt()));
  //if (FunctionName == "i2c.write")       MyOut =  String(Wire.write(Param0.c_str()));
  if (FunctionName == F("i2c.end"))         MyOut =  String(Wire.endTransmission());
  if (FunctionName == F("i2c.requestfrom"))
  {
    i2cPinNo = Param0.toInt();
    byte i2cParamB = Param1.toInt();
    MyOut =  String(Wire.requestFrom(i2cPinNo, i2cParamB));
  }
  if (FunctionName == F("i2c.available"))   MyOut =  String(Wire.available());
  if (FunctionName == F("i2c.read"))        MyOut =  String(Wire.read());

  if (FunctionName == F("hextoint"))        MyOut =  String(StrToHex(MyOut ));



  if (FunctionName == F("sqr"))   MyOut = FloatToString(sqrt(MyOut.toFloat()));
  if (FunctionName == F("sin"))   MyOut = FloatToString(sin(MyOut.toFloat()));
  if (FunctionName == F("cos"))   MyOut = FloatToString(cos(MyOut.toFloat()));
  if (FunctionName == F("tan"))   MyOut = FloatToString(tan(MyOut.toFloat()));
  if (FunctionName == F("log"))   MyOut = FloatToString(log(MyOut.toFloat()));

  if (FunctionName == F("hex") | FunctionName == F("hex$"))   MyOut = String(MyOut.toInt(), HEX);
  if (FunctionName == F("oct") | FunctionName == F("oct$"))   MyOut = String(MyOut.toInt(), OCT);


  if (FunctionName == F("ip"))    MyOut = String(WiFi.localIP().toString());



  if (FunctionName == F("rnd"))
  {
    randomSeed(millis());
    MyOut = FloatToString(random(MyOut.toFloat()));
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



String FloatToString(float d)
{
  //Convert a double to string with 5 decimals and then removes the trailing zeros (and eventually the '.')
  String z = String(d, 5); 
  char *p;
  p = (char*) z.c_str()+ z.length() -1; 
  while (*p == '0') 
  {
    *p-- = '\0';
  }
  if (*p == '.')
    *p = '\0';
  return z;
}

