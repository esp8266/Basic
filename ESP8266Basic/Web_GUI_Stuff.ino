String RunningProgramGui()
{

  //PrintAndWebOut(WebArgumentsReceivedInput );

  //Serial.println("Program is running. CHecking for goto statemets");
  CheckFOrWebVarInput();
  if (CheckFOrWebGOTO() == 1)
  {
    WaitForTheInterpertersResponse = 0;
    RunningProgram == 1;
    RunBasicTillWait();
    WaitForTheInterpertersResponse = 1;
  }


  if (refreshBranch != "")
  {

    inData = String(" goto " + refreshBranch + " ");
    WaitForTheInterpertersResponse = 0;
    ExicuteTheCurrentLine();
    runTillWaitPart2();
    RunBasicTillWait();
  }


  String WebOut = String(MobileFreindlyWidth) + String(F("<form action='input'>"))  + HTMLout + String(F("</form>"));


  if (BasicDebuggingOn == 1)
  {
    Serial.println(F("Web out first"));
    Serial.println( WebOut);
    Serial.println(F("HTML out"));
    Serial.println( HTMLout);
  }

  for (int i = 50; i >= 0; i--)
  {
    delay(0);
    WebOut.replace(String(F("VARS|")) + String(i), AllMyVaribles[i][1]);
  }


  WebOut.replace(F("**graphics**"), BasicGraphics());

  if (BasicDebuggingOn == 1)
  {
    Serial.println( WebOut);
  }
  return WebOut;
}



void PrintAndWebOut(String itemToBePrinted)
{
  Serial.println(itemToBePrinted);
  HTMLout = String(HTMLout + "<hr>" + itemToBePrinted);
  return;
}



String BasicGraphics()
{
  String BasicGraphicsOut;
  BasicGraphicsOut = GraphicsStartCode;
  BasicGraphicsOut.replace(F("*wid*"),  String(GraphicsEliments[0][1]));
  BasicGraphicsOut.replace(F("*hei*"),  String(GraphicsEliments[0][2]));

  String GraphicsEliment;
  for (int i = 1; i <= GraphicsEliments[0][0]; i++)
  {
    if (GraphicsEliments[i][0] == 1) //For Line
    {
      GraphicsEliment = GraphicsLineCode;
    }

    if (GraphicsEliments[i][0] == 2) //For Circle
    {
      GraphicsEliment = GraphicsCircleCode;
    }


    if (GraphicsEliments[i][0] == 3) //For ELLIPSE
    {
      GraphicsEliment = GraphicsEllipseCode;
    }

    if (GraphicsEliments[i][0] == 4) //For rectangle
    {
      GraphicsEliment = GraphicsRectangleCode;
    }


    GraphicsEliment.replace(F("*x1*"),  String(GraphicsEliments[i][1]));
    GraphicsEliment.replace(F("*y1*"),  String(GraphicsEliments[i][2]));
    GraphicsEliment.replace(F("*x2*"),  String(GraphicsEliments[i][3]));
    GraphicsEliment.replace(F("*y2*"),  String(GraphicsEliments[i][4]));

    if (GraphicsEliments[i][5] == 0 ) GraphicsEliment.replace("*collor*", F("black"));
    if (GraphicsEliments[i][5] == 1 ) GraphicsEliment.replace("*collor*", F("Navy"));
    if (GraphicsEliments[i][5] == 2 ) GraphicsEliment.replace("*collor*", F("Green"));
    if (GraphicsEliments[i][5] == 3 ) GraphicsEliment.replace("*collor*", F("Teal"));
    if (GraphicsEliments[i][5] == 4 ) GraphicsEliment.replace("*collor*", F("Maroon"));
    if (GraphicsEliments[i][5] == 5 ) GraphicsEliment.replace("*collor*", F("Purple"));
    if (GraphicsEliments[i][5] == 6 ) GraphicsEliment.replace("*collor*", F("Olive"));
    if (GraphicsEliments[i][5] == 7 ) GraphicsEliment.replace("*collor*", F("Silver"));
    if (GraphicsEliments[i][5] == 8 ) GraphicsEliment.replace("*collor*", F("Gray"));
    if (GraphicsEliments[i][5] == 9 ) GraphicsEliment.replace("*collor*", F("Blue"));
    if (GraphicsEliments[i][5] == 10 ) GraphicsEliment.replace("*collor*", F("Lime"));
    if (GraphicsEliments[i][5] == 11 ) GraphicsEliment.replace("*collor*", F("Aqua"));
    if (GraphicsEliments[i][5] == 12 ) GraphicsEliment.replace("*collor*", F("Red"));
    if (GraphicsEliments[i][5] == 13 ) GraphicsEliment.replace("*collor*", F("Fuchsia"));
    if (GraphicsEliments[i][5] == 14 ) GraphicsEliment.replace("*collor*", F("Yellow"));
    if (GraphicsEliments[i][5] == 15 ) GraphicsEliment.replace("*collor*", F("White"));
    BasicGraphicsOut += GraphicsEliment;
  }
  BasicGraphicsOut += "</svg>";
  return BasicGraphicsOut;
}








byte CheckFOrWebGOTO()
{
  String bla;
  byte x = 0;

  for (int i = 0; i <= TotalNumberOfLines - 1; i++)
  {
    int str_len = String(i).length() + 1 + 4;
    char ArgumentToTest[str_len];
    String(String("goto" + String(i))).toCharArray(ArgumentToTest, str_len);
    delay(0);
    bla = server.arg(ArgumentToTest);
    if (bla.length() > 0)
    {
      x = i;
    }
  }

  //Serial.println(x);
  if (x != 0)
  {
    for (int i = 0; i <= TotalNumberOfLines - 1; i++) {
      delay(0);

      String gotoTest = BasicProgram(i);
      gotoTest.trim();

      if (gotoTest == ButtonsInUse[x] | String(gotoTest + ":") == ButtonsInUse[x])
      {
        //Serial.println("This is the line I am going to");
        //Serial.println(BasicProgram(i));
        RunningProgramCurrentLine = i - 1;
        return 1;
      }
    }

  }

  WaitForTheInterpertersResponse = 1;
}


void CheckFOrWebVarInput()
{
  String bla;


  for (int i = 0; i < 50; i++)
  {
    int str_len = String(i).length() + 1;
    char ArgumentToTest[str_len];
    String(i).toCharArray(ArgumentToTest, str_len);
    delay(0);
    bla = server.arg(ArgumentToTest);
    if (bla.length() > 0)
    {
      AllMyVaribles[i][1] = GetRidOfurlCharacters(bla);
    }
  }
  return;
}




String GenerateIDtag(String TempateString)
{
  LastElimentIdTag = String(millis());
  TempateString.replace(F("myid"),LastElimentIdTag );
  return TempateString;
}

