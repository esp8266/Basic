void ExicuteTheCurrentLine()
{
  String Param0;
  String Param1;
  String Param2;
  String Param3;
  String Param4;
  String Param5;
  inData.trim();
  Param0 = getValue(inData, ' ', 0);
  Param1 = getValue(inData, ' ', 1);
  Param2 = getValue(inData, ' ', 2);
  Param3 = getValue(inData, ' ', 3);
  Param4 = getValue(inData, ' ', 4);
  Param5 = getValue(inData, ' ', 5);

  Param0.toLowerCase();


  int valParam0 = Param0.toInt();
  int valParam1 = Param1.toInt();
  int valParam2 = Param2.toInt();
  int valParam3 = Param3.toInt();
  int valParam4 = Param4.toInt();
  int valParam5 = Param5.toInt();


  if (BasicDebuggingOn == 1)
  {
    Serial.println("Exicuting line Debug Statement");
    Serial.println(String(String(RunningProgramCurrentLine) + " " + inData));
    Serial.println(Param0);
    Serial.println(Param1);
    Serial.println(Param2);
    Serial.println(Param3);
    Serial.println(Param4);
    Serial.println(Param5);
  }


  if ( Param0 == "") return;

  if ( Param0 == "if")
  {
    //Serial.print("Dofing if Statement");
    //Serial.println(DoMathForMe(GetMeThatVar(Param1), Param2, GetMeThatVar(Param3)));
    if (DoMathForMe(GetMeThatVar(Param1), Param2, GetMeThatVar(Param3)) == "1" )
    {

      Param0 = getValue(inData, ' ', 5);
      Param1 = getValue(inData, ' ', 6);
      Param2 = getValue(inData, ' ', 7);
      Param3 = getValue(inData, ' ', 8);
      Param4 = getValue(inData, ' ', 9);
      Param5 = getValue(inData, ' ', 10);
      inData = String(Param0 + " " + Param1 + " " + Param2 + " " + Param3 + " " + Param4 + " " + Param5 + " ");
      Param0.toLowerCase();

    }
    else
    {
      for (int i = 7; i <= 17; i++)
      {
        delay(0);
        //Serial.println(i);
        //Serial.println(Param0);
        Param0 = getValue(inData, ' ', i);
        Param0.toLowerCase();
        if ( Param0 == "else")
        {
          //Serial.println("Found else");
          Param0 = getValue(inData, ' ', i + 1);
          Param1 = getValue(inData, ' ', i + 2);
          Param2 = getValue(inData, ' ', i + 3);
          Param3 = getValue(inData, ' ', i + 4);
          Param4 = getValue(inData, ' ', i + 5);
          Param5 = getValue(inData, ' ', i + 6);
          inData = String(Param0 + " " + Param1 + " " + Param2 + " " + Param3 + " " + Param4 + " " + Param5 + " ");
          Param0.toLowerCase();
          break;
        }
      }
    }
  }



  if (Param0 == "for")
  {

//    for (int i = ForNextReturnLocations[0]; i <= 1; i--)
//    {
//      delay(0);
//      if (RunningProgramCurrentLine == ForNextReturnLocations[i])
//      {
//        break;
//      }
//    }

    ForNextReturnLocations[0]++;
    ForNextReturnLocations[ForNextReturnLocations[0]] = RunningProgramCurrentLine;


    Param0 = "let";
  }



  if (Param0 == "next")
  {
    //for x = 1 to 10

    for (int i = ForNextReturnLocations[0]; i >= 1; i--)
    {
      delay(0);
      String gotoTestFor = BasicProgram(ForNextReturnLocations[i]);
      gotoTestFor.trim();

      String VarTest = getValue(gotoTestFor, ' ', 1);

      String VarTestIfDone = getValue(gotoTestFor, ' ', 5);
      if (ForNextReturnLocations[i] == 0) return;


      if (VarTest == Param1)
      {
        float test1 = GetMeThatVar(VarTest).toFloat();
        float test2 = GetMeThatVar(VarTestIfDone).toFloat();

        //Serial.println(test1);
        //Serial.println(test2);

        if ( test1  !=  test2 )
        {
          RunningProgramCurrentLine = ForNextReturnLocations[i];
          Param0 = "let";
          Param3 = Param1;
          Param4 = "+";
          Param5 = "1";
          break;
        }
        else
        {
          ForNextReturnLocations[0]--;
          return;
        }
      }
    }

  }


  if (Param0 == "debugon")
  {
    BasicDebuggingOn = 1;
    return;
  }

  if (Param0 == "debugoff")
  {
    BasicDebuggingOn = 0;
    return;
  }


  if (Param0 == "run")
  {
    GraphicsEliments[0][0] = 0;
    RunningProgram = 1;
    numberButtonInUse = 0;
    RunningProgramCurrentLine = 0;
    HTMLout = "";
    TimerWaitTime = 0;
    //RunBasicProgram();
    return;
  }


  if (Param0 == "reboot" | Param0 == "restart")
  {
    ESP.restart();
    return;
  }


  if (Param0 == "vars")
  {
    //for debugging purpose to print out all of the basic variables
    PrintAllMyVars();
    return;
  }


  if (Param0 == "memclear")
  {
    for (byte i = 0; i <= 50; i++)
    {
      AllMyVaribles[i][1] = "";
      AllMyVaribles[i][2] = "";
    }
    return;
  }



  if (Param0 == "list")
  {
    for (int i = 0; i <= TotalNumberOfLines; i++)
    {
      if (BasicProgram(i).length() > 0)
      {
        delay(0);
        //PrintAndWebOut(BasicProgram[i].length());
        PrintAndWebOut(String(String(i) + " " + BasicProgram(i)));
      }
    }
    return;
  }


  if (Param0 == "dir")
  {
    //SPIFFS.begin();
    Dir dir = SPIFFS.openDir(String("/" ));
    while (dir.next()) {
      File f = dir.openFile("r");
      PrintAndWebOut(String(dir.fileName() + "       " + f.size()));
    }
    return;
  }

  if (Param0 == "del")
  {
    //SPIFFS.begin();

    PrintAndWebOut(String("/" + Param1));
    PrintAndWebOut(String(SPIFFS.remove(String("/" + Param1))));
    return;
  }





  //Commnads to controll pins

  if ( Param0 == "pi")
  {

    SetMeThatVar(Param2, String(UniversalPinIO("pi", GetMeThatVar(Param1), 0)));
    return;
  }

  if ( Param0 == "po")
  {

    valParam2 = GetMeThatVar(Param2).toInt();

    UniversalPinIO("po", GetMeThatVar(Param1), valParam2);
    return;
  }


  if ( Param0 == "pwi")
  {
    SetMeThatVar(Param2, String(UniversalPinIO("pwi", GetMeThatVar(Param1), 0)));
    return;
  }

  if ( Param0 == "pwo")
  {

    valParam2 = GetMeThatVar(Param2).toInt();

    UniversalPinIO("pwo", GetMeThatVar(Param1), valParam2);
    return;
  }


  if ( Param0 == "temp" | Param0 == "ti")
  {
    valParam1 = GetMeThatVar(Param1).toInt();
    // call sensors.requestTemperatures() to issue a global temperature
    // request to all devices on the bus
    sensors.requestTemperatures(); // Send the command to get temperatures

    SetMeThatVar(Param2, String(sensors.getTempCByIndex(valParam1)));
    return;
  }


  if ( Param0 == "ai")
  {
    SetMeThatVar(Param1, String(analogRead(A0)));
    return;
  }

  if ( Param0 == "servo")
  {
    valParam2 = GetMeThatVar(Param2).toInt();

    UniversalPinIO("servo", GetMeThatVar(Param1), valParam2);
    return;
  }




  //Feading and writing variables to flash memory
  if ( Param0 == "read")
  {
    SetMeThatVar(GetMeThatVar(Param2), LoadDataFromFile(GetMeThatVar(Param1)));
    return;
  }


  if ( Param0 == "write")
  {
    SaveDataToFile(GetMeThatVar(Param1), GetMeThatVar(Param2));
    return;
  }







  if ( Param0 == "delay")
  {
    valParam1 = GetMeThatVar(Param1).toInt();
    delay(valParam1);
    return;
  }


  if ( Param0 == "timer")
  {
    TimerWaitTime = GetMeThatVar(Param1).toInt();
    TimerBranch = Param2;
    return;
  }


  if ( Param0 == "sleep")
  {

    ESP.deepSleep(GetMeThatVar(Param1).toInt() * 1000000, WAKE_RF_DEFAULT);
    return;
  }



  if (Param0 == "print")
  {
    PrintAndWebOut(GetMeThatVar(Param1));
    return;
  }


  if (Param0 == "serialprint")
  {
    Serial.print(GetMeThatVar(Param1));
    return;
  }

  if (Param0 == "serialprintln")
  {
    Serial.println(GetMeThatVar(Param1));
    return;
  }



  if (Param0 == "baudrate")
  {
    Serial.begin(GetMeThatVar(Param1).toInt());
    return;
  }





  //i2c led display
  if (Param0 == "oledprint")
  {
    Param1 = GetMeThatVar(Param1);

    int str_len = Param1.length() + 1;
    char OLEDTString[str_len];
    Param1.toCharArray(OLEDTString, str_len);
    sendStrXY(OLEDTString, GetMeThatVar(Param3).toInt(), GetMeThatVar(Param2).toInt());
    return;
  }


  if (Param0 == "oledcls")
  {
    clear_display();
    return;
  }



  if (Param0 == "oledsend")
  {
    sendcommand(GetMeThatVar(Param1).toInt());
    return;
  }





  //end i2c display code

  //i2c 1602 lcd display
  if (Param0 == "lcdprint")
  {
    Param1 = GetMeThatVar(Param1);

    int str_len = Param1.length() + 1;
    char LCDTString[str_len];
    Param1.toCharArray(LCDTString, str_len);
    lcd.setCursor(GetMeThatVar(Param2).toInt(), GetMeThatVar(Param3).toInt());
    lcd.print(LCDTString);
    return;
  }

  if (Param0 == "lcdcls")
  {
    lcd.clear();
    return;
  }
  
  if (Param0 == "lcdbl")
  {
    if (GetMeThatVar(Param1).toInt() == 1) 
    {
      lcd.backlight();
    } 
    else 
    {
      lcd.noBacklight();
    }
    return;
  }


  if (Param0 == "lcdsend")
  {
    //Param1 is the value to send
    //Param2 is MODE: 0=COMMAND, 1=DATA, 2=FOUR_BITS
    lcd.send(GetMeThatVar(Param1).toInt(), GetMeThatVar(Param2).toInt());
    return;
  }

  //end i2c 1602 lcd display code




  //Web Browser output commands

  if (Param0 == "wprint" | Param0 == "html")
  {
    HTMLout += GetMeThatVar(Param1);
    //Serial.print(HTMLout);
    return;
  }



  if (Param0 == "image")
  {
    String tempInfo = GenerateIDtag(normalImage);
    tempInfo.replace("name", GetMeThatVar(Param1));
    HTMLout += tempInfo;
    //Serial.print(HTMLout);
    return;
  }


  if (Param0 == "javascript")
  {
    String tempInfo = javascript;
    tempInfo.replace("name", GetMeThatVar(Param1));
    HTMLout += tempInfo;
    //Serial.print(HTMLout);
    return;
  }


  if (Param0 == "css")
  {
    String tempInfo = CSSscript;
    tempInfo.replace("name", GetMeThatVar(Param1));
    HTMLout += tempInfo;
    //Serial.print(HTMLout);
    return;
  }


  if (Param0 == "button")
  {
    numberButtonInUse++;
    String tempButton = GenerateIDtag(GOTObutton);
    tempButton.replace("gotonotext",  GetMeThatVar(Param1));
    //Serial.println(String(String(numberButtonInUse) + " = numberButtonInUse"));

    tempButton.replace("gotonobranch",  String("goto" + String(numberButtonInUse)));

    ButtonsInUse[numberButtonInUse] = Param2;
    //Serial.println(ButtonsInUse[numberButtonInUse]);
    HTMLout = String(HTMLout + tempButton);
    return;
  }



  if (Param0 == "imagebutton")
  {
    numberButtonInUse++;
    String tempButton = GenerateIDtag(GOTOimagebutton);
    tempButton.replace("gotonotext",  GetMeThatVar(Param1));
    //Serial.println(String(String(numberButtonInUse) + " = numberButtonInUse"));

    tempButton.replace("gotonobranch",  String("goto" + String(numberButtonInUse)));

    ButtonsInUse[numberButtonInUse] = Param2;
    //Serial.println(ButtonsInUse[numberButtonInUse]);
    HTMLout = String(HTMLout + tempButton);
    return;
  }


  if (Param0 == "textbox")
  {
    String tempTextBox = GenerateIDtag(TextBox);
    GetMeThatVar(Param1);
    if (VariableLocated == 0)
    {
      SetMeThatVar(Param1, "");
      GetMeThatVar(Param1);
    }

    tempTextBox.replace("variablevalue",  String("VARS|" + String(LastVarNumberLookedUp)));
    tempTextBox.replace("variablenumber",  String(LastVarNumberLookedUp));

    HTMLout = String(HTMLout + tempTextBox);
    return;
  }


  if (Param0 == "slider")
  {
    String tempSlider = GenerateIDtag(Slider);
    GetMeThatVar(Param1);
    if (VariableLocated == 0)
    {
      SetMeThatVar(Param1, "");
      GetMeThatVar(Param1);
    }
    tempSlider.replace("variablevalue",  String("VARS|" + String(LastVarNumberLookedUp)));
    tempSlider.replace("variablenumber",  String(LastVarNumberLookedUp));

    tempSlider.replace("minval",  GetMeThatVar(Param2));
    tempSlider.replace("maxval",  GetMeThatVar(Param3));

    HTMLout = String(HTMLout + tempSlider);
    return;
  }




  if (Param0 == "dropdown" | Param0 == "listbox")
  {
    String tempDropDownList = GenerateIDtag(DropDownList);
    String tempDropDownListOpptions  = DropDownListOpptions;
    String TempItems;
    String TempBla;

    Param1 = GetMeThatVar(Param1);

    for (int i = 0; i <= 20; i++)
    {
      tempDropDownListOpptions  = DropDownListOpptions;
      TempBla = getValue(String(Param1 + ","), ',', i);
      TempBla.replace(",", "");
      if (TempBla != "") {

        tempDropDownListOpptions.replace("item",  TempBla);
        TempItems = String( TempItems + tempDropDownListOpptions);
      }
      delay(0);
    }

    Param2 = GetMeThatVar(Param2);
    if (VariableLocated == 0)
    {
      SetMeThatVar(Param2, "");
      GetMeThatVar(Param2);
    }

    tempDropDownList.replace("variablenumber",  String(LastVarNumberLookedUp));
    tempDropDownList.replace("options",  TempItems);
    if (Param3.toInt() > 1 | Param0 == "dropdown") Param3 = "1";
    tempDropDownList.replace("theSize", String(Param3.toInt()));

    HTMLout = String(HTMLout + tempDropDownList);
    return;
  }







  if (Param0 == "onload")
  {
    refreshBranch = Param1;
    return;
  }





  if (Param0 == "wait")
  {
    //HTMLout = String(HTMLout + "<hr>" + GetMeThatVar(Param1));
    WaitForTheInterpertersResponse = 1;
    return;
  }
  //PrintAndWebOut("Just Passed the Wait Command");

  if (Param0 == "cls")
  {
    numberButtonInUse = 0;

    for (int i = 0; i <= 10; i++) {
      ButtonsInUse[i] = "";
    }
    HTMLout = "";
    return;
  }



  //All of my graphis engeine commands

  if (Param0 == "graphics")
  {
    GraphicsEliments[0][1] = GetMeThatVar(Param1).toInt();
    GraphicsEliments[0][2] = GetMeThatVar(Param2).toInt();
    HTMLout += "**graphics**";
    return;
  }

  if (Param0 == "gcls")
  {
    GraphicsEliments[0][0] = 0;
    return;
  }

  if (Param0 == "line")
  {
    int i;
    GraphicsEliments[0][0] += 1;
    i = GraphicsEliments[0][0];
    GraphicsEliments[i][0] = 1;
    GraphicsEliments[i][1] = GetMeThatVar(Param1).toInt();
    GraphicsEliments[i][2] = GetMeThatVar(Param2).toInt();
    GraphicsEliments[i][3] = GetMeThatVar(Param3).toInt();
    GraphicsEliments[i][4] = GetMeThatVar(Param4).toInt();
    GraphicsEliments[i][5] = GetMeThatVar(Param5).toInt();
    return;
  }

  if (Param0 == "circle")
  {
    int i;
    GraphicsEliments[0][0] += 1;
    i = GraphicsEliments[0][0];
    GraphicsEliments[i][0] = 2;
    GraphicsEliments[i][1] = GetMeThatVar(Param1).toInt();
    GraphicsEliments[i][2] = GetMeThatVar(Param2).toInt();
    GraphicsEliments[i][3] = GetMeThatVar(Param3).toInt();
    GraphicsEliments[i][5] = GetMeThatVar(Param4).toInt();
    return;
  }

  if (Param0 == "ellipse")
  {
    int i;
    GraphicsEliments[0][0] += 1;
    i = GraphicsEliments[0][0];
    GraphicsEliments[i][0] = 3;
    GraphicsEliments[i][1] = GetMeThatVar(Param1).toInt();
    GraphicsEliments[i][2] = GetMeThatVar(Param2).toInt();
    GraphicsEliments[i][3] = GetMeThatVar(Param3).toInt();
    GraphicsEliments[i][4] = GetMeThatVar(Param4).toInt();
    GraphicsEliments[i][5] = GetMeThatVar(Param5).toInt();
    return;
  }

  if (Param0 == "rect")
  {
    int i;
    GraphicsEliments[0][0] += 1;
    i = GraphicsEliments[0][0];
    GraphicsEliments[i][0] = 4;
    GraphicsEliments[i][1] = GetMeThatVar(Param1).toInt();
    GraphicsEliments[i][2] = GetMeThatVar(Param2).toInt();
    GraphicsEliments[i][3] = GetMeThatVar(Param3).toInt();
    GraphicsEliments[i][4] = GetMeThatVar(Param4).toInt();
    GraphicsEliments[i][5] = GetMeThatVar(Param5).toInt();
    return;
  }



  if (Param0 == "input")
  {
    if (Param2 == "")
    {
      SetMeThatVar(Param1, getSerialInput());
    }
    else
    {
      Serial.print(GetMeThatVar(Param1));
      SetMeThatVar(Param2, getSerialInput());
    }
    //PrintAndWebOut("");
    return;
  }




  //branching commands

  if (Param0 == "goto")
  {
    for (int i = 0; i <= TotalNumberOfLines; i++) {
      String gotoTest = BasicProgram(i);
      gotoTest.trim();

      if (gotoTest == Param1 | String(gotoTest + ":") == Param1)
      {
        RunningProgramCurrentLine = i - 1;
        break;
      }
    }
    return;
  }


  if (Param0 == "gosub")
  {
    for (int i = 0; i <= TotalNumberOfLines; i++) {
      String gotoTest = BasicProgram(i);
      gotoTest.trim();

      if (gotoTest == Param1 | String(gotoTest + ":") == Param1)
      {
        NumberOfReturns = NumberOfReturns + 1;
        ReturnLocations[NumberOfReturns] = RunningProgramCurrentLine;
        RunningProgramCurrentLine = i - 1;
        i = TotalNumberOfLines + 1;
        break;
      }
    }

    return;
  }



  if (Param0 == "return")
  {
    RunningProgramCurrentLine = ReturnLocations[NumberOfReturns];
    NumberOfReturns = NumberOfReturns - 1;
    return;
  }

  if (Param0 == "end")
  {

    for (int i = 0; i <= 255; i++)
    {
      ForNextReturnLocations[i] = 0;
    }
    RunningProgram = 0;
    WaitForTheInterpertersResponse = 1;
    TimerWaitTime = 0;
    PrintAndWebOut("Done...");
    return;
  }


  if (Param0 == "load")
  {
    for (int i = 0; i <= 255; i++)
    {
      ForNextReturnLocations[i] = 0;
    }
    GraphicsEliments[0][0] = 0;
    PrintAndWebOut(String("Loading . . . . " + Param1));
    ProgramName = Param1;
    numberButtonInUse = 0;
    RunningProgramCurrentLine = 0;
    HTMLout = "";
    TimerWaitTime = 0;
    return;
  }




  //Wifi Commands

  if (Param0 == "connect")
  {
    ConnectToTheWIFI(GetMeThatVar(Param1), GetMeThatVar(Param2), GetMeThatVar(Param3), GetMeThatVar(Param4), GetMeThatVar(Param5));
    return;
  }

  if (Param0 == "ap")
  {
    CreateAP(GetMeThatVar(Param1), GetMeThatVar(Param2));
    return;
  }

  if (Param0 == "wget")
  {
    SetMeThatVar(Param1, FetchWebUrl(GetMeThatVar(Param2)));
    return;
  }



  if (Param0 == "setupemail")
  {
    EmailServer = GetMeThatVar(Param1);
    Emailport = GetMeThatVar(Param2).toInt();
    EmailSMTPuser = GetMeThatVar(Param3);
    EmailSMTPpassword = GetMeThatVar(Param4);
    return;
  }


  if (Param0 == "email" | Param0 == "sendemail" )
  {
    //To, From, Subject, MsgBody
    sendEmail(GetMeThatVar(Param1), GetMeThatVar(Param2), GetMeThatVar(Param3), GetMeThatVar(Param4) );
    return;
  }



  //Code to handle MSG Branch


  if (Param0 == "msgbranch")
  {
    msgbranch = Param1;
    return;
  }

  if (Param0 == "msgreturn")
  {
    MsgBranchRetrnData = GetMeThatVar(Param1);
    return;
  }


  if (Param0 == "msgget")
  {

    Param1 = GetMeThatVar(Param1);
    int str_len = Param1.length() + 1;
    char MgetToTest[str_len];
    Param1.toCharArray(MgetToTest, str_len);
    SetMeThatVar(Param2, GetRidOfurlCharacters(server.arg( MgetToTest  )));
    return;
  }


  // let command down here for a reason


  if ( Param1 == "=")
  {
    Param0 = getValue(inData, ' ', 0);
    Param5 = Param4;
    Param4 = Param3;
    Param3 = Param2;
    Param2 = Param1;
    Param1 = Param0;
    Param0 = "let";
  }

  if ( Param0 == "let")
  {
    SetMeThatVar(Param1, DoMathForMe(GetMeThatVar(Param3), Param4, GetMeThatVar(Param5)));
    return;
  }


  GetMeThatVar(Param0); //will exicure any functions if no other commands were found.
  return;
}
