parser_data pd;
int num_args;
PARSER_PREC args[PARSER_MAX_ARGUMENT_COUNT];
String *args_str[PARSER_MAX_ARGUMENT_COUNT];
extern String args_var[PARSER_MAX_ARGUMENT_COUNT];
extern int args_var_pos;
extern int args_var_level;

void InitCommandParser()
{
  //parser_data_init( &pd, NULL, variable_callback, function_callback, NULL );
  pd.str = NULL;
  pd.len = 0;
  pd.pos = 0;
  pd.error = NULL;
  pd.variable_cb = variable_callback;
  pd.function_cb = function_callback;
}

int ExtractArguments(String &inData)
{
  int r;
  inData.concat(')'); // add a parenthesys at the end
  pd.pos = inData.indexOf(' ') + 1;    // starts just after the command
  pd.str = inData.c_str();
  pd.len = inData.length() + 1; // important the +1 as this permit to touch the '\0'
  pd.error = NULL; // reset the previous error
  args_var_pos = args_var_pos = 0;
  for (int i = 0; i < PARSER_MAX_ARGUMENT_COUNT; i++)
    args_var[i] = "";
  r = parser_read_argument_list( &pd, &num_args, args, args_str);
  if (pd.error != NULL)
    PrintAndWebOut(String(pd.error));

  return r;
}

void DeAllocateArguments()
{
  for (int i = 0; i < num_args; i++)
    delete args_str[i];
}


void clear_stacks()
{
  return_Stack.clear(); // clear the return stack
  forNextStack.clear(); // clear the for next stack
  doLoopStack.clear(); // clear the do loop stack
  form1.clear();
}

void HaltBasic(String err_mess)
{
  clear_stacks();
  RunningProgram = 0;
  WaitForTheInterpertersResponse = 1;
  TimerWaitTime = 0;
  TimerCBtime = 0;
  PrintAndWebOut(err_mess);
  PrintAndWebOut(String(F("Halted at line ")) + String(RunningProgramCurrentLine));
}

void ExicuteTheCurrentLine()
{
  int r;

  String Param0;
  inData.trim();
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

  // Line_For_Eval = inData; /////////////////////////////////////////////////////////////////////////////////////////////////
  String Params[10]; // max 10 parameters are allowed

  int valParam0 = Param0.toInt();
  int valParam1 = Param1.toInt();
  int valParam2 = Param2.toInt();


  if (BasicDebuggingOn == 1)
  {
    Serial.println(F("Executing line Debug Statement"));
    Serial.println(String(String(RunningProgramCurrentLine) + " " + inData));
    //    Serial.println(Param0);
    //    Serial.println(Param1);
    //    Serial.println(Param2);
    //    Serial.println(Param3);
    //    Serial.println(Param4);
    //    Serial.println(Param5);
  }

  Param0.trim();
  if ( Param0 == "") return;
  if (Param0.startsWith("["))  return;
  if (Param0.startsWith("'"))  return;

  r = parserotto(inData.substring(inData.indexOf(' ') + 1), Params);
  if (r != -1)
  {
    delay(0);
    if (r >= 1) Param1 = Params[0];
    if (r >= 2) Param2 = Params[1];
    if (r >= 3) Param3 = Params[2];
    if (r >= 4) Param4 = Params[3];
    if (r >= 5) Param5 = Params[4];
  }



  if ( Param0 == F("if"))
  {
    // the goal here is to find if there is a "then" or an "else"
    // so the best is to take the positions of each one
    // we need to take care if these words are included into the commands or arguments
    // so, for the moment, we should avoid to use these "words" inside the line
    // we can just define that these words should be surrounded by spaces (" then " " else ")
    //
    int then_pos, else_pos;

    then_pos = inData.indexOf(F(" then "));
    else_pos = inData.indexOf(F(" else "));

    if (inData.endsWith(F(" then")))
    {
      // this is considered as a IF THEN {ELSE} ENDIF | END IF

      String Comparaison = inData.substring(2, inData.length() - 5);  // from the 'if' and 'then' (length - 5 chars ' then')
      Comparaison.trim();
      //Serial.println(Comparaison);

      if (evaluate(Comparaison) != "-1") // if expression is false goto else or, if not defined, to endif
      {
        else_pos = IfBlockList.getElse(RunningProgramCurrentLine);
        if (else_pos == 0)
        {
          HaltBasic(F("Else / End If not found"));
          return;
        }
        RunningProgramCurrentLine = else_pos;
        return;
      }
      else    // if the expression is true, continue
        return;
    }

    // works as before when the command is style : if a = b then print a else print d

    if (then_pos == -1) // the "then " is not found
    {
      HaltBasic(F("Syntax error in if command"));
      return;
    }

    String Comparaison = inData.substring(2, then_pos);
    Comparaison.trim();
    //Serial.println(Comparaison);

    if (evaluate(Comparaison) == "-1")
      inData = inData.substring(then_pos + 6, else_pos);
    else if (else_pos > 0)
      inData = inData.substring(else_pos + 6);
    else
      return; // there is no else command

    inData.trim();
    //Serial.println(inData);
    //Param0 = inData.substring(0, inData.indexOf(' '));    // recover the new command
    Param0 = getValue(inData, ' ', 0);// recover the new command
	  r = parserotto(inData.substring(inData.indexOf(' ') + 1), Params);
	  if (r != -1)
	  {
		delay(0);
		if (r >= 1) Param1 = Params[0];
		if (r >= 2) Param2 = Params[1];
		if (r >= 3) Param3 = Params[2];
		if (r >= 4) Param4 = Params[3];
		if (r >= 5) Param5 = Params[4];
	  }

    Param0.toLowerCase();
  }

  if (Param0 == F("else"))
  {
    int endif_pos = IfBlockList.getEndIf(RunningProgramCurrentLine);
    if (endif_pos == 0)
    {
      HaltBasic(F("End If not found"));
      return;
    }
    RunningProgramCurrentLine = endif_pos;
    return;
  }

  if ( (Param0 == F("endif")) || (inData == F("end if")) )
  {
    // no action
    return;
  }

  if (Param0 == F("for"))
  {
    if (forNextStack.setFor(inData, RunningProgramCurrentLine) == false)
      HaltBasic(F("Syntax error in For "));

    return;

  }



  if (Param0 == F("next"))
  {
    r = forNextStack.checkNext(inData);
    if ( r > 0 )
    {
      RunningProgramCurrentLine = r;
      return;
    }

    if (r == 0)
    {
      return;
    }

    if ( r == -1)
    {
      HaltBasic(F("Next without for "));
    }

  }


  if (Param0 == F("do"))
  {
    doLoopStack.add(RunningProgramCurrentLine);
    return;
  }

  if (Param0 == F("loop"))
  {
    r = doLoopStack.checkLoop(inData);
    if ( r > 0 )
    {
      RunningProgramCurrentLine = r;
      return;
    }

    if (r == 0)
    {
      return;
    }

    if ( r == -1)
    {
      HaltBasic(F("loop without do "));
      return;
    }

    if ( r == -2)
    {
      HaltBasic(F("loop {until | while} not valid "));
      return;
    }


  }


  if (Param0 == F("debugon"))
  {
    BasicDebuggingOn = 1;
    return;
  }

  if (Param0 == F("debugoff"))
  {
    BasicDebuggingOn = 0;
    return;
  }


  if (Param0 == F("run"))
  {
    ProgramName = GetMeThatVar(Param1);
    GraphicsEliments[0][0] = 0;
    RunningProgram = 1;
    numberButtonInUse = 0;
    RunningProgramCurrentLine = 0;
    HTMLout = "";
    TimerWaitTime = 0;
    //RunBasicProgram();
    return;
  }


  if (Param0 == F("reboot") | Param0 == F("restart"))
  {
    ESP.restart();
    return;
  }


  if (Param0 == F("vars"))
  {
    //for debugging purpose to print out all of the basic variables
    PrintAllMyVars();
    return;
  }


  if (Param0 == F("memclear"))
  {
    deleteVariables();
    return;
  }







  if (Param0 == F("del"))
  {
    //SPIFFS.begin();
    Param1 = evaluate(Param1);
    PrintAndWebOut(String("/" + Param1));
    PrintAndWebOut(String(SPIFFS.remove(String("/" + Param1))));
    return;
  }





  //Commnads to controll pins


  if ( Param0 == F("interrupt"))
  {
    UniversalPinIO(Param2, evaluate(Param1), 0);
    return;
  }




  if ( Param0 == F("pwmfreq"))
  {

    Param1 = inData.substring(Param0.length() + 1);    // starts just after the command
    valParam1 = evaluate(Param1).toInt();
    if ((valParam1 > 0) && (valParam1 <= 8000))
      analogWriteFreq(valParam1);
    return;
  }







  //time and delay stuff

  if ( Param0 == F("delay"))
  {
    // this is an example of extraction using the ExtractArgument function just for one parameter
    // the best way, with a single parameter, is to use directly the eval command
    r = ExtractArguments(inData);
    //valParam1 = GetMeThatVar(Param1).toInt();
    delay(args[0]);
    DeAllocateArguments();  // don't forget to call this function after each ExtractArguments
    return;
  }

  if ( Param0 == F("timer"))
  {
    // this is another way to separate the arguments
    // we can state that the label name is separated from the 1st argument by a ',';
    // we can so find the ',' from the end of the string and take the argument space trimmed for the label name
    // the 1st argument will be so the text between this ',' and the end of the command
    Param1 = inData.substring(inData.indexOf(' ') + 1);    // starts just after the command
    r = Param1.lastIndexOf(',');
    if (r == -1)
    {
      PrintAndWebOut(F("Syntax Error; Label or argument missing"));
      return;
    }
    Param2 = Param1.substring(r + 1);
    Param2.trim();
    Param1 = Param1.substring(0, r);
    TimerWaitTime = evaluate(Param1).toInt();
    TimerBranch = Param2;
    //TimerWaitTime = GetMeThatVar(Param1).toInt();
    //TimerBranch = Param2;
    return;
  }

  if ( Param0 == F("timercb"))
  {
    TimerCBtime = 0;
    int i;
    if ((i = JumpList.getPos(Param2)) != -1)
    {
      TimerCBBranchLine = i - 1;
      TimerCBtime = evaluate(Param1).toInt();
      return;
    }
    PrintAndWebOut(F("timercb line not found!"));
    return;
  }

  if ( Param0 == F("sleep"))
  {
    // this command needs to be checked!
    Param1 = inData.substring(Param0.length() + 1);    // starts just after the command
    ESP.deepSleep(evaluate(Param1).toInt() * 1000000, WAKE_RF_DEFAULT);
    return;
  }



  if (Param0 == F("print"))
  {
    // this is an example of extraction using directly the evaluate function taking into account all the text after the command (so after the ' ' )
    Param1 = inData.substring(Param0.length() + 1);    // starts just after the command
    //PrintAndWebOut(GetMeThatVar(Param1));
    PrintAndWebOut(evaluate(Param1));
    return;
  }


  if (Param0 == F("serialprint"))
  {
    Param1 = inData.substring(Param0.length() + 1);    // starts just after the command
    //Serial.print(GetMeThatVar(Param1));
    Serial.print(evaluate(Param1));
    return;
  }

  if (Param0 == F("serialprintln"))
  {
    //Serial.println(GetMeThatVar(Param1));
    Param1 = inData.substring(Param0.length() + 1);    // starts just after the command
    Serial.println(evaluate(Param1));
    return;
  }

  if (Param0 == F("websockprint"))
  {
    //Serial.println(GetMeThatVar(Param1));
    Param1 = inData.substring(Param0.length() + 1);    // starts just after the command
    WebSocketSend(  evaluate(Param1).c_str());
    return;
  }

  if (Param0 == F("webobj"))   // change the property of an object in the webpage
  {
    //Serial.println(GetMeThatVar(Param1));
    Param1 = inData.substring(Param0.length() + 1);    // starts just after the command
    WebSocketSend(  evaluate(Param1).c_str());
    return;
  }

  if (Param0 == F("serial2begin"))  // new command serial2begin baudrate, pin TX, pin RX
  {
    r = ExtractArguments(inData);
    if (args[0] <= 0)
    {
      PrintAndWebOut(F("Serial2Begin: baudrate must be > 0"));
      return;
    }
    delete swSer; // close eventually the previous instance of the sw serial port
    switch (num_args)
    {
      case 1:
        swSer = new SoftwareSerial(12, 2, false, 256);  // pin 12 RX, pin 2 TX, no inverse logic, 256 bytes buffer
        swSer->begin(args[0]);
        break;

      case 2:
        swSer = new SoftwareSerial(12, args[1], false, 256);  // pin 12 RX, pin x TX, no inverse logic, 256 bytes buffer
        swSer->begin(args[0]);
        break;

      case 3:
        swSer = new SoftwareSerial(args[2], args[1], false, 256);  // pin y RX, pin x TX, no inverse logic, 256 bytes buffer
        swSer->begin(args[0]);
        break;
    }
    return;
  }

  if (Param0 == F("serial2end"))
  {
    delete swSer;
    swSer = NULL;
    return;
  }

  if (Param0 == F("serial2print"))
  {
    if (swSer == NULL) return;
    Param1 = inData.substring(Param0.length() + 1);    // starts just after the command
    swSer->print(evaluate(Param1));
    return;
  }

  if (Param0 == F("serial2println"))
  {
    if (swSer == NULL) return;
    Param1 = inData.substring(Param0.length() + 1);    // starts just after the command
    swSer->println(evaluate(Param1));
    return;
  }

  if (Param0 == F("baudrate"))
  {
    //Serial.begin(GetMeThatVar(Param1).toInt());
    Param1 = inData.substring(Param0.length() + 1);    // starts just after the command
    Serial.begin(evaluate(Param1).toInt());
    return;
  }





  //i2c led display
  if (Param0 == "oledprint")
  {
    Param1 = evaluate(Param1);

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
    sendcommand(evaluate(Param1).toInt());
    return;
  }





  //end i2c display code

  //i2c 1602 lcd display
  if (Param0 == "lcdprint")
  {
    Param1 = evaluate(Param1);

    int str_len = Param1.length() + 1;
    char LCDTString[str_len];
    Param1.toCharArray(LCDTString, str_len);
    lcd.setCursor(evaluate(Param2).toInt(), evaluate(Param3).toInt());
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
    lcd.send(evaluate(Param1).toInt(), evaluate(Param2).toInt());
    return;
  }

  //end i2c 1602 lcd display code




  //Web Browser output commands

  if (Param0 == F("wprint") | Param0 == F("html"))
  {
    Param1 = inData.substring(Param0.length() + 1);    // starts just after the command
    Param1 = evaluate(Param1);
    //HTMLout += Param1;
    AddToWebOut(Param1);
    //Serial.print(HTMLout);
    return;
  }



  if (Param0 == F("image"))
  {

    String tempInfo = GenerateIDtag(normalImage);
    tempInfo.replace(F("name"), evaluate(Param1));
    AddToWebOut(tempInfo);
    //Serial.print(HTMLout);
    return;
  }


  if (Param0 == F("javascript"))
  {
    NewGuiItemAddedSinceLastWait = 1;
    String tempInfo = javascript;
    tempInfo.replace(F("name"), evaluate(Param1));
    HTMLout += tempInfo;
    //Serial.print(HTMLout);
    return;
  }


  if (Param0 == F("css"))
  {
    NewGuiItemAddedSinceLastWait = 1;
    String tempInfo = CSSscript;
    tempInfo.replace(F("name"), evaluate(Param1));
    HTMLout += tempInfo;
    //Serial.print(HTMLout);
    return;
  }



  if (Param0 == F("textbox"))
  {
    NewGuiItemAddedSinceLastWait = 1;
    String tempTextBox = GenerateIDtag(TextBox);
    VarialbeLookup(Param1);
    if (VariableLocated == 0)
    {
      SetMeThatVar(Param1, "", PARSER_STRING);
      GetMeThatVar(Param1);
    }

    tempTextBox.replace(F("variablevalue"),  String(F("VARS|")) + String(LastVarNumberLookedUp));
    tempTextBox.replace(F("variablenumber"),  String(LastVarNumberLookedUp));

    HTMLout += tempTextBox;
    return;
  }


  if (Param0 == F("passwordbox"))
  {
    NewGuiItemAddedSinceLastWait = 1;
    String tempTextBox = GenerateIDtag(passwordbox);
    VarialbeLookup(Param1);
    if (VariableLocated == 0)
    {
      SetMeThatVar(Param1, "", PARSER_STRING);
      GetMeThatVar(Param1);
    }

    tempTextBox.replace(F("variablevalue"),  String(F("VARS|")) + String(LastVarNumberLookedUp));
    tempTextBox.replace(F("variablenumber"),  String(LastVarNumberLookedUp));

    HTMLout += tempTextBox;
    return;
  }


  if (Param0 == F("slider"))
  {
    NewGuiItemAddedSinceLastWait = 1;
    r = ExtractArguments(inData);
    String tempSlider = GenerateIDtag(Slider);
    VarialbeLookup(args_var[0]);
    if (VariableLocated == 0)
    {
      SetMeThatVar(args_var[0], "", PARSER_TRUE);
      GetMeThatVar(args_var[0]);
    }
    tempSlider.replace(F("variablevalue"),  String(F("VARS|")) + String(LastVarNumberLookedUp));
    tempSlider.replace(F("variablenumber"),  String(LastVarNumberLookedUp));

    tempSlider.replace(F("minval"), FloatToString(args[1]));
    tempSlider.replace(F("maxval"), FloatToString(args[2]));

    HTMLout += tempSlider;
    DeAllocateArguments();  // don't forget to call this function after each ExtractArguments
    return;
  }




  if (Param0 == F("dropdown") | Param0 == F("listbox"))
  {
    r = ExtractArguments(inData);
    NewGuiItemAddedSinceLastWait = 1;
    String tempDropDownList = GenerateIDtag(DropDownList);
    String tempDropDownListOpptions  = DropDownListOpptions;
    String TempItems;
    String TempBla;


    Param3 = String(args[2]);


    for (int i = 0; i <= 20; i++)
    {
      tempDropDownListOpptions  = DropDownListOpptions;
      TempBla = getValue(String(*args_str[1] + ","), ',', i);
      TempBla.replace(",", "");
      if (TempBla != "")
      {
        tempDropDownListOpptions.replace(F("item"),  TempBla);
        TempItems = String( TempItems + tempDropDownListOpptions);
      }
      delay(0);
    }

    VarialbeLookup(args_var[0]);
    if (VariableLocated == 0)
    {
      SetMeThatVar(args_var[0], "", PARSER_STRING);
      GetMeThatVar(args_var[0]);
    }

    tempDropDownList.replace(F("variablenumber"),  String(LastVarNumberLookedUp));
    tempDropDownList.replace(F("options"),  TempItems);
    if (Param3.toInt() < 1 | Param0 == F("dropdown")) Param3 = "1";
    tempDropDownList.replace(F("theSize"), String(Param3.toInt()));

    HTMLout += tempDropDownList;
    DeAllocateArguments();
    return;
  }





  if (Param0 == F("button"))
  {
    //r = ExtractArguments(inData);
    NewGuiItemAddedSinceLastWait = 1;
    r = parserotto(inData.substring(inData.indexOf(' ') + 1), Params);
    if (r == -1) {
      HaltBasic(F("Syntax Error!"));
      return;
    }
    if (r != 2) {
      HaltBasic(F("The arguments must be 2!"));
      return;
    }
    String tempButton = GenerateIDtag(GOTObutton);
    tempButton.replace(F("gotonotext"),  evaluate(Params[0]));

    tempButton.replace(F("gotonobranch"),  String(JumpList.getPos(Params[1])));

    ButtonsInUse[numberButtonInUse] = Params[1];
    //Serial.println(ButtonsInUse[numberButtonInUse]);
    HTMLout = String(HTMLout + tempButton);
    return;
  }



  if (Param0 == F("imagebutton"))
  {
    numberButtonInUse++;
	Param1 = evaluate(Param1);
    String tempButton = GenerateIDtag(GOTOimagebutton);
    if (Param1.startsWith(F("http://")) | Param1.startsWith(F("HTTP://")) )tempButton.replace(F("/file?file="), "");
    tempButton.replace(F("gotonotext"),  Param1);

    tempButton.replace(F("gotonobranch"),  String(JumpList.getPos(Param2)));

    HTMLout = String(HTMLout + tempButton);
    return;
  }




  if (Param0 == F("onload"))
  {
    refreshBranch = Param1;
    return;
  }





  if (Param0 == F("wait"))
  {
    WaitForTheInterpertersResponse = 1;
    return;
  }

  if (Param0 == F("returngui"))
  {
    //HTMLout = String(HTMLout + "<hr>" + GetMeThatVar(Param1));
    server->send(200, "text/html", RunningProgramGui());
    server->handleClient();
    WaitForTheInterpertersResponse = 0;
    return;
  }




  //PrintAndWebOut("Just Passed the Wait Command");

  if (Param0 == F("cls"))
  {
    numberButtonInUse = 0;

    for (int i = 0; i <= 10; i++) {
      ButtonsInUse[i] = "";
    }
    WebSocketSend(  "guicls");
    HTMLout = "";
    return;
  }



  //All of my graphis engeine commands

  if (Param0 == F("graphics"))
  {
    GraphicsEliments[0][1] = evaluate(Param1).toInt();
    GraphicsEliments[0][2] = evaluate(Param2).toInt();
    HTMLout += F("**graphics**");
    return;
  }

  if (Param0 == F("gcls"))
  {
    NewGraphicItemAddedSinceLastWait = 1;
    GraphicsEliments[0][0] = 0;
    return;
  }

  if (Param0 == F("line"))
  {
    NewGraphicItemAddedSinceLastWait = 1;
    int i;
    GraphicsEliments[0][0] += 1;
    i = GraphicsEliments[0][0];
    GraphicsEliments[i][0] = 1;
    GraphicsEliments[i][1] = evaluate(Param1).toInt();
    GraphicsEliments[i][2] = evaluate(Param2).toInt();
    GraphicsEliments[i][3] = evaluate(Param3).toInt();
    GraphicsEliments[i][4] = evaluate(Param4).toInt();
    GraphicsEliments[i][5] = evaluate(Param5).toInt();
    return;
  }

  if (Param0 == F("circle"))
  {
    NewGraphicItemAddedSinceLastWait = 1;
    int i;
    GraphicsEliments[0][0] += 1;
    i = GraphicsEliments[0][0];
    GraphicsEliments[i][0] = 2;
    GraphicsEliments[i][1] = evaluate(Param1).toInt();
    GraphicsEliments[i][2] = evaluate(Param2).toInt();
    GraphicsEliments[i][3] = evaluate(Param3).toInt();
    GraphicsEliments[i][5] = evaluate(Param4).toInt();
    return;
  }

  if (Param0 == F("ellipse"))
  {
    NewGraphicItemAddedSinceLastWait = 1;
    int i;
    GraphicsEliments[0][0] += 1;
    i = GraphicsEliments[0][0];
    GraphicsEliments[i][0] = 3;
    GraphicsEliments[i][1] = evaluate(Param1).toInt();
    GraphicsEliments[i][2] = evaluate(Param2).toInt();
    GraphicsEliments[i][3] = evaluate(Param3).toInt();
    GraphicsEliments[i][4] = evaluate(Param4).toInt();
    GraphicsEliments[i][5] = evaluate(Param5).toInt();
    return;
  }

  if (Param0 == F("rect"))
  {
    NewGraphicItemAddedSinceLastWait = 1;
    int i;
    GraphicsEliments[0][0] += 1;
    i = GraphicsEliments[0][0];
    GraphicsEliments[i][0] = 4;
    GraphicsEliments[i][1] = evaluate(Param1).toInt();
    GraphicsEliments[i][2] = evaluate(Param2).toInt();
    GraphicsEliments[i][3] = evaluate(Param3).toInt();
    GraphicsEliments[i][4] = evaluate(Param4).toInt();
    GraphicsEliments[i][5] = evaluate(Param5).toInt();
    return;
  }



  if (Param0 == F("input"))
  {

    if (Param2 == "")
    {
      SetMeThatVar(Param1, getSerialInput(), PARSER_STRING);
    }
    else
    {
      Serial.print(evaluate(Param1));
      SetMeThatVar(Param2, getSerialInput(), PARSER_STRING);
    }
    //PrintAndWebOut("");
    return;
  }

  if (Param0 == F("serialinput"))
  {
    Param2 = "";
    delay(10);
    //Serial.println("serialinput passage");
    while (Serial.available() > 0)
    {
      char received = Serial.read();
      Param2.concat(received);
      delay(0);
    }
    SetMeThatVar(Param1, Param2, PARSER_STRING);
    return;
  }

  if (Param0 == F("serial2input"))
  {
    Param2 = "";
    delay(10);
    //Serial.println("serialinput passage");
    while (swSer->available() > 0)
    {
      char received = swSer->read();
      Param2.concat(received);
      delay(0);
    }
    SetMeThatVar(Param1, Param2, PARSER_STRING);
    return;
  }

  if (Param0 == F("serialflush"))
  {

    serialFlush();
    return;
  }


  if (Param0 == F("serialtimeout"))
  {

    SerialTimeOut = evaluate(Param1).toInt();
    return;
  }

  //branching commands

  if (Param0 == F("goto"))
  {
    if ((r = JumpList.getPos(Param1)) != -1)
    {
      RunningProgramCurrentLine = r - 1;
      return;
    }
    PrintAndWebOut(String(F("Goto Label not found:")) + Param1);
    return;
  }


  if (Param0 == F("gosub"))
  {
    if ((r = JumpList.getPos(Param1)) != -1)
    {
      return_Stack.push(RunningProgramCurrentLine);
      RunningProgramCurrentLine = r - 1;
      return;
    }
    PrintAndWebOut(String(F("Gosub Label not found:")) + Param1);
    return;
  }



  if (Param0 == F("return"))
  {
    if ( (r = return_Stack.pop()) == -1 )
    {
      HaltBasic(F("return without gosub "));
      return;
    }
    RunningProgramCurrentLine = r;
    UdpBranchLine = abs(UdpBranchLine); // restore the udpBranch command
    SerialBranchLine  = abs(SerialBranchLine);  // restore the serialbranch command
    Serial2BranchLine = abs(Serial2BranchLine); // restore the serial2branch command
    IRBranchLine = abs(IRBranchLine); // restore the IRbranch command
    TimerCBBranchLine = abs(TimerCBBranchLine); // restore the timercb command
    TouchBranchLine = abs(TouchBranchLine); // restore the TouchBranch command
    WebSockEventBranchLine = abs(WebSockEventBranchLine); // restore the websockevent command
    WebSockChangeBranchLine = abs(WebSockChangeBranchLine); // restore the websockchange command
    webSocket.loop();
    return;
  }

  if (Param0 == F("end"))
  {
    clear_stacks();
    RunningProgram = 0;
    WaitForTheInterpertersResponse = 1;
    TimerWaitTime = 0;
    TimerCBtime = 0;
    PrintAndWebOut(F("Done..."));
    return;
  }


  if (Param0 == F("load"))
  {
	Param1 =  evaluate(Param1);
    clear_stacks();
    GraphicsEliments[0][0] = 0;
    PrintAndWebOut(String(F("Loading . . . . ")) + Param1);
    ProgramName = Param1;
    numberButtonInUse = 0;
    RunningProgramCurrentLine = 0;
    HTMLout = "";
    TimerWaitTime = 0;
    TimerCBtime = 0;
    return;
  }




  //Wifi Commands

  if (Param0 == F("connect"))
  {
    ConnectToTheWIFI(evaluate(Param1), evaluate(Param2), evaluate(Param3), evaluate(Param4), evaluate(Param5));
    return;
  }

  if (Param0 == F("ap"))
  {
    //CreateAP(GetMeThatVar(Param1), GetMeThatVar(Param2));
    CreateAP(evaluate(Param1), evaluate(Param2), LoadDataFromFile("ipaddress"), LoadDataFromFile("gateway"), LoadDataFromFile("subnetmask"));
    return;
  }

  if (Param0 == F("wifioff"))
  {
    //WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    return;
  }



  if (Param0 == F("setupemail"))
  {
    EmailServer = evaluate(Param1);
    Emailport = evaluate(Param2).toInt();
    EmailSMTPuser = evaluate(Param3);
    EmailSMTPpassword = evaluate(Param4);
    return;
  }


  if (Param0 == F("email") | Param0 == F("sendemail" ))
  {
    //To, From, Subject, MsgBody
    sendEmail(evaluate(Param1), evaluate(Param2), evaluate(Param3), evaluate(Param4) );
    return;
  }



  //Code to handle MSG Branch


  if (Param0 == F("msgbranch"))
  {
    msgbranch = Param1;
    return;
  }

  if (Param0 == F("msgreturn"))
  {
    MsgBranchRetrnData = evaluate(Param1);
    return;
  }





  // CiccioCB UDP commands
  //////////////////////////////////////////////////////////////////////////////////
  if (Param0 == F("udpbegin"))  // the command is : udpbegin port
  {
    String ar = inData.substring(Param0.length() + 1);    // starts just after the command
    int v = evaluate(ar).toInt();
    udp.begin(v);

    return;
  }


  if (Param0 == F("udpbeginmulticast"))  // the command is : udpbeginmulticast multicast_ip, port
  {
    r = ExtractArguments(inData);
    IPAddress RemoteIP;
    if (num_args == 2)
    {
      //1st arg is the multicast IP address
      // convert it from String to int
      int st = 0;
      int en;
      for (int i = 0; i < 4; i++)
      {
        en = args_str[0]->indexOf('.', st);
        RemoteIP[i] =  args_str[0]->substring(st, en).toInt();
        st = en + 1;
      }
      udp.beginMulticast(WiFi.localIP(), RemoteIP, args[1]);
    }

    DeAllocateArguments();
    return;
  }


  if (Param0 == F("udpwrite"))  // the command is : udpwrite ip_address, port, message
  {
    r = ExtractArguments(inData);
    IPAddress RemoteIP;
    if (num_args == 3)
    {
      //1st arg is the IP address
      // convert it from String to int
      int st = 0;
      int en;
      for (int i = 0; i < 4; i++)
      {
        en = args_str[0]->indexOf('.', st);
        RemoteIP[i] =  args_str[0]->substring(st, en).toInt();
        st = en + 1;
      }
      udp.beginPacket(RemoteIP, args[1]);
      udp.write(args_str[2]->c_str());
      udp.endPacket();
    }
    DeAllocateArguments();
    return;
  }


  if (Param0 == F("udpwritemulticast"))  // the command is : udpwritemulticast, ip_address_multi, port, message
  {
    r = ExtractArguments(inData);
    IPAddress RemoteIP;
    if (num_args == 3)
    {
      //1st arg is the IP address
      // convert it from String to int
      int st = 0;
      int en;
      for (int i = 0; i < 4; i++)
      {
        en = args_str[0]->indexOf('.', st);
        RemoteIP[i] =  args_str[0]->substring(st, en).toInt();
        st = en + 1;
      }
      udp.beginPacketMulticast(RemoteIP, args[1], WiFi.localIP());
      udp.write(args_str[2]->c_str());
      udp.endPacket();
    }
    DeAllocateArguments();
    return;
  }

  if (Param0 == F("udpreply"))  // the command is : udpreply message
  {
    String ar = inData.substring(Param0.length() + 1);    // starts just after the command
    String rep = evaluate(ar);
    udp.beginPacket(UdpRemoteIP, UdpRemotePort);
    //    Serial.println(UdpRemoteIP);
    //    Serial.println(UdpRemotePort);

    udp.write(rep.c_str());
    udp.endPacket();
    return;
  }



  if (Param0 == F("udpstop"))
  {
    udp.stop();
    return;
  }

  if (Param0 == F("udpbranch"))
  {
    UdpBranchLine = 0;
    int i;
    if ((i = JumpList.getPos(Param1)) != -1)
    {
      UdpBranchLine = i - 1;
      return;
    }
    PrintAndWebOut(F("UdpBranch line not found!"));
    //    Serial.print("udpbranch");
    //    Serial.println(UdpBranchLine);
    return;
  }

  if (Param0 == F("serialbranch"))
  {
    SerialBranchLine = 0;
    int i;
    if ((i = JumpList.getPos(Param1)) != -1)
    {
      SerialBranchLine = i - 1;
      return;
    }
    PrintAndWebOut(F("SerialBranch line not found!"));
    return;
  }

  if (Param0 == F("serial2branch"))
  {
    Serial2BranchLine = 0;
    int i;
    if ((i = JumpList.getPos(Param1)) != -1)
    {
      Serial2BranchLine = i - 1;
      return;
    }
    PrintAndWebOut(F("Serial2Branch line not found!"));
    return;
  }

  if (Param0 == F("irbranch"))
  {
    IRBranchLine = 0;
    int i;
    if ((i = JumpList.getPos(Param1)) != -1)
    {
      IRBranchLine = i - 1;
      return;
    }
    PrintAndWebOut(F("IRBranch line not found!"));
    return;
  }

  if (Param0 == F("websockeventbranch"))
  {
    WebSockEventBranchLine = 0;
    int i;
    if ((i = JumpList.getPos(Param1)) != -1)
    {
      WebSockEventBranchLine = i - 1;
      return;
    }
    PrintAndWebOut(F("WebSockEventBranch line not found!"));
    return;
  }

  if (Param0 == F("websockchangebranch"))
  {
    WebSockChangeBranchLine = 0;
    int i;
    if ((i = JumpList.getPos(Param1)) != -1)
    {
      WebSockChangeBranchLine = i - 1;
      return;
    }
    PrintAndWebOut(F("WebSockChangeBranch line not found!"));
    return;
  }

  if (Param0 == F("touchbranch"))
  {
    TouchBranchLine = 0;
    int i;
    if ((i = JumpList.getPos(Param1)) != -1)
    {
      TouchBranchLine = i - 1;
      return;
    }
    PrintAndWebOut(F("TouchBranch line not found!"));
    return;
  }
  ////////////////////////////

  /////// NEW mid STUFF //////
  if ( Param0.startsWith(F("mid(")) )
  {
    // find the closing paranthesys
    int num_par = 1;
    int i;
    for (i = 4; i < inData.length(); i++)
    {
      if ( inData[i] == '(' ) num_par++;
      if ( inData[i] == ')' ) num_par--;
      if (num_par == 0) break;
    }
    if (num_par != 0)  // no closing parenthesys ->Error
    {
      PrintAndWebOut(F("Mid: missing closing parenthesys"));
      return;
    }
    // check if the '=' follow
    int eq = inData.indexOf('=', i);
    if (eq == -1) // missing = on the line
    {
      PrintAndWebOut(F("Mid: missing = on the line"));
      return;
    }
    Param1 = inData.substring(4 , i);
    r = ExtractArguments(Param1);
    DeAllocateArguments();
    if ( (num_args != 2) && (num_args != 3) )
    {
      PrintAndWebOut(F("Mid: number of arguments not valid"));
      return;
    }


    // we need to check if the args are valid
    // first we must extract the variable name
    int com = inData.indexOf(',');
    Param2 = inData.substring(4, com);
    Param2.trim();
    r = VariablePosition(Param2);
    if (r == -1)
    {
      PrintAndWebOut(F("Mid: destination variable not defined"));
      return;
    }

    Param3 = evaluate(inData.substring( eq + 1 ));
    if (parser_result != PARSER_STRING)
    {
      PrintAndWebOut(F("Mid: set value must be string"));
      return;
    }

    if  (num_args == 2)
      args[2] = Param3.length();
    Param4 = AllMyVariables[r].getVar();
    Param4 = Param4.substring(0, args[1] - 1) + Param3.substring(0, args[2]) + Param4.substring(args[1] - 1 + args[2]);
    AllMyVariables[r].setVar(Param4);
    return;
  }


  /// DIM STUFF //////////

  if (Param0 == F("dim"))
  {
    if ((r = inData.indexOf('(')) != -1) // there is a parenthesys in the command; maybe it's dim command
    {
      // find the closing paranthesys
      int num_par = 1;
      int i;
      for (i = r + 1 ; i < inData.length(); i++)
      {
        if ( inData[i] == '(' ) num_par++;
        if ( inData[i] == ')' ) num_par--;
        if (num_par == 0) break;
      }
      if (num_par != 0)  // no closing parenthesys ->Error
      {
        PrintAndWebOut(F("DIM: missing closing parenthesys"));
        return;
      }

      Param1 = inData.substring(3, r); // array name
      Param1.trim();
      if (Param1 == "")
      {
        PrintAndWebOut(F("DIM: the array name is missing"));
        return;
      }
      Param2 = inData.substring(r + 1 , i); // arguments
      r = ExtractArguments(Param2);
      DeAllocateArguments();
      if (num_args != 1)
      {
        PrintAndWebOut(F("DIM: number of arguments must be 1"));
        return;
      }

      // here we are OK, we can create the array; for the moment array with $ are string, without float
      // we should check before if the same array name exists
      if ((r = Search_Array(Param1)) != -1)
        basic_arrays[r].remove();

      if ( (r = Search_First_Available_Array()) == -1)
      {
        PrintAndWebOut(F("DIM: no more array space available"));
        return;
      }

      // determine if the arrays is number or string
      if ((Param1.indexOf('$') != -1) || (inData.indexOf(F("as string")) != -1))
        basic_arrays[r].dim(Param1, Param2.toInt(), PARSER_STRING);
      else
        basic_arrays[r].dim(Param1, Param2.toInt(), PARSER_TRUE);
    }
    return;
  }

  if (Param0 == F("undim"))
  {
    // remove an already dimensioned array; maybe the name "undim" should be modified
    if ( (r = Search_Array(Param1)) == -1)
    {
      PrintAndWebOut(F("UNDIM: array not defined"));
      return;
    }

    basic_arrays[r].remove();
    return;
  }


  /// NEW array identification //
  if ((r = Param0.indexOf('(')) != -1) // there is a parenthesys in the command; maybe it's an array
  {
    // find the closing parenthesys
    int num_par = 1;
    int i;
    for (i = r + 1 ; i < inData.length(); i++)
    {
      if ( inData[i] == '(' ) num_par++;
      if ( inData[i] == ')' ) num_par--;
      if (num_par == 0) break;
    }
    if (num_par != 0)  // no closing parenthesys ->Error
    {
      PrintAndWebOut(F("Array: missing closing parenthesys"));
      return;
    }
    // check if the '=' follow
    int eq = inData.indexOf('=', i);
    if (eq != -1) // the '=' is present on the line; so this should be an array init
    {
      //{  PrintAndWebOut(F("Array: missing = on the line"));  return; }
      Param1 = Param0.substring(0, r); // array name
      Param1.trim();
      Param2 = inData.substring(r + 1 , i); // arguments
      r = ExtractArguments(Param2);
      DeAllocateArguments();
      if (num_args != 1)
      {
        PrintAndWebOut(F("Array: number of arguments must be 1"));
        return;
      }

      Param3 = evaluate(inData.substring( eq + 1 ));

      if ( (r = Search_Array(Param1)) == -1)
      {
        PrintAndWebOut(F("Array not defined"));
        return;
      }

      if (basic_arrays[r].Format == PARSER_STRING) // string format
      {
        basic_arrays[r].setString(args[0], Param3);
      }
      else
      {
        basic_arrays[r].setFloat(args[0], atof(Param3.c_str()));
      }
      return;
    }
  }

  // let command down here for a reason

  if ( Param1.startsWith(F("=")))
  {
    Param0 = "let";
  }

  if ( Param0.indexOf('=') > 1 )
  { //Serial.println("Found the  = sign");
    inData.replace(F("="), F(" = "));
    Param0 = "let";
  }




  if ( Param0 == F("let"))
  {
    //Serial.println("Evaluating . .. .");
    //Serial.println(inData);
    //SetMeThatVar(Param1, DoMathForMe(GetMeThatVar(Param3), Param4, GetMeThatVar(Param5)));

    // we should use a more "scientific" way to recognize the line;
    // tipically a let line is composed of :
    // 1) an optional 'let' followed by space(s)
    // 2) the variable name followed by space(s) or '='
    // 3) the "=" sign
    // 4) the math operation

    // we assume that the line has already been 'trimmed' with leading and trailing spaces removed
    // a new function is required to identify more clearly each element of the line!!!!
    // this is just a test
    // step 1
    if (inData.substring(0, 3) == "let")
      inData = inData.substring(4);

    //step2
    int equal_pos = inData.indexOf('=');
    if (equal_pos == -1)
    {
      HaltBasic(F("syntax error. Missing the '=' on the line!"));
      return;
    }
    Param1 = inData.substring(0, equal_pos);
    Param1.trim();
    //Serial.println(Param1);
    Param2 = inData.substring(equal_pos + 1);
    Param2.trim();
    //Serial.println(Param2);
    Param3 = evaluate(Param2);
    //    Serial.print("risultato ");
    //    Serial.println(Param3);
    SetMeThatVar(Param1, Param3, parser_result);
    return;
  }
  //Serial.println(RunningProgramCurrentLine);
  //Param0 = getValue(inData, ' ', 0);

  if ( inData != "") {
    evaluate(inData);//will execute any functions if no other commands were found.
    // if the result is PARSER_FALSE, this means that the function hasn't been recognised
    if (parser_result != PARSER_FALSE)
      return;
  }
  HaltBasic(F("Syntax error"));
  //RunningProgram = 0;
  //WaitForTheInterpertersResponse = 1;
  return;
}


IPAddress StringToIP(String x)
{
  IPAddress _IP;
  // convert it from String to int
  int st = 0;
  int en;
  for (int i = 0; i < 4; i++)
  {
    en = x.indexOf('.', st);
    _IP[i] =  x.substring(st, en).toInt();
    st = en + 1;
  }
  return _IP;
}

int parserotto(String st, String res[10])
{
  int i;
  bool quotes = false;
  bool vertbars = false;
  int parenthesys = 0;
  int positions[10]; // position of the commas for 10 arguments max
  int j = 0;
  char c;
  for (i = 0; i < st.length(); i++)
  {
    c = st[i];
    if ( (c == '"') && (vertbars == false) )
      quotes = !quotes;
    if ( (c == '|') && (quotes == false) )
      vertbars = !vertbars;

    if (c == '(') parenthesys++;
    if (c == ')') parenthesys--;

    if (c == ',')  // if the comma is found
    { // if the quotes, vertical bars and parenthesys are closed
      if ( (quotes == false) && (vertbars == false) && (parenthesys == 0) )
      {
        // this is a valid split point
        positions[j++] = i;
      }
    }
  }
  positions[j++] = st.length();  // last arguments goes until the end
  if ( (quotes == false) && (vertbars == false) && (parenthesys == 0) )
  {
    int pos_prev = -1;
    for (i = 0; i < j; i++)
    {
      res[i] = st.substring(pos_prev + 1, positions[i]);
      res[i].trim();
      pos_prev = positions[i];
    }
    return j;
  }
  else
  {
    return -1;
  }
}
