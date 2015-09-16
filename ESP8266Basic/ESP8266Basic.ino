//ESP8266 Basic Interperter
//The MIT License (MIT)
//
//Copyright (c) 2015 Michael Molinari
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.


#include "spiffs/spiffs.h"
#include <FS.h>
#include <ESP8266mDNS.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <Wire.h>




ESP8266WebServer server(80);

//Web Server Variables
//String HTMLout = "<input type='text' name='firstname'>";
String HTMLout;
const String InputFormText = R"=====( <input type="text" name="input"><input type="submit" value="Submit" name="inputButton"><hr>)=====";

const String TextBox = R"=====( <input type="text" name="variablenumber" value="variablevalue"><hr>)=====";
const String GOTObutton =  R"=====(<input type="submit" value="gotonotext" name="gotonobranch">)=====";
byte WaitForTheInterpertersResponse = 1;

const String AdminBarHTML = R"=====(
<a href="./vars">[ VARS ]</a> 
<a href="./edit">[ EDIT ]</a>
<a href="./run">[ RUN ]</a>
<a href="./settings">[ SETTINGS ]</a>
<hr>)=====";

const String EditorPageHTML =  R"=====(
<form action='edit' id="usrform">
<input type="text" name="name" value="*program name*">
<input type="submit" value="Open" name="open">
<input type="submit" value="Save" name="save">
</form>
<br>
<textarea rows="25" cols="75" name="code" form="usrform">
*program txt*
</textarea>
)=====";

const String SettingsPageHTML =  R"=====(
<form action='settings' id="usrform"><table>
<tr><th>
Station Mode (Connect to your router):</th></tr>
<tr><th><p align="right">Name:</p></th><th><input type="text" name="staName" value="*sta name*"></th></tr>
<tr><th><p align="right">Pass:</p></th><th><input type="text" name="staPass" value="*sta pass*"></th></tr>
<tr><th>
<br><br>Ap mode (ESP brocast out its own ap):</th></tr>
<tr><th><p align="right">Name:</p></th><th><input type="text" name="apName" value="*ap name*"></th></tr>
<tr><th><p align="right">Pass:</p></th><th><input type="text" name="apPass" value="*ap pass*"></th></tr>
<tr><th>
<input type="submit" value="Save" name="save"></th><th>
<input type="submit" value="Format" name="format"></th>
</tr>
</table></form>
<br>
)=====";




//Graphics HTML CODE

const String GraphicsStartCode =  R"=====(
<svg width="*wid*" height="*hei*">
)=====";


const String GraphicsLineCode =  R"=====(
<line x1="*x1*" y1="*y1*" x2="*x2*" y2="*y2*" stroke="*collor*"/>
)=====";

const String GraphicsCircleCode =  R"=====(
<circle cx="*x1*" cy="*y1*" r="*x2*" fill="*collor*"/>
)=====";


const String GraphicsEllipseCode =  R"=====(
<ellipse cx="*x1*" cy="*y1*" rx="*x2*" ry="*y2*" fill="*collor*"/>
)=====";

const String GraphicsRectangleCode =  R"=====(
<rect x="*x1*" y="*y1*" width="*x2*" height="*y2*" style="fill:*collor*"/>
)=====";
  


String WebArgumentsReceived;
String WebArgumentsReceivedInput;
byte numberButtonInUse = 0;
String ButtonsInUse[11];


// Buffer to store incoming commands from serial port
String inData;

int TotalNumberOfLines = 255;
String BasicProgram[255];                                //Array of strings to hold basic program


String AllMyVaribles[50][2];
byte LastVarNumberLookedUp;                                 //Array to hold all of the basic variables



<<<<<<< HEAD
bool RunningProgram = 1;                                //Will be set to 1 if the program is currently running
=======
byte RunningProgram = 0;                                //Will be set to 1 if the program is currently running
>>>>>>> parent of 95617bf... Added auto run feature
byte RunningProgramCurrentLine = 0;                     //Keeps track of the currently running line of code
byte NumberOfReturns;
bool BasicDebuggingOn;
byte ReturnLocations[254];

int GraphicsEliments[100][7];




bool inputPromptActive = 0;





void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_AP_STA);
  PrintAndWebOut("Simple Basic Interperter For ESP8266...");

  //CheckWaitForRunningCode();

  server.on("/", []()
  {
    String WebOut = AdminBarHTML;
    WebOut += RunningProgramGui();
    server.send(200, "text/html", WebOut);
  });






  server.on("/settings", []()
  {
<<<<<<< HEAD
    WaitForTheInterpertersResponse = 1;
=======
>>>>>>> parent of 95617bf... Added auto run feature
    String WebOut = AdminBarHTML;
    WebOut += SettingsPageHTML;
    String staName;
    String staPass;
    String apName;
    String apPass;
    Serial.print("Loading Settings Files");
  
    staName = LoadDataFromFile("WIFIname");
    staPass = LoadDataFromFile("WIFIpass");
    apName  = LoadDataFromFile("APname");
    apPass  = LoadDataFromFile("APpass");

    if ( server.arg("save") == "Save" )
    {
      staName = server.arg("staName");
      staPass = server.arg("staPass");
      apName  = server.arg("apName");
      apPass  = server.arg("apPass");

      SaveDataToFile("WIFIname" , staName);
      SaveDataToFile("WIFIpass" , staPass);
      SaveDataToFile("APname" , apName);
      SaveDataToFile("APpass" , apPass);
    }

    if ( server.arg("format") == "Format" )
    {
      Serial.println("Formating ");
      SPIFFS.begin();
      //Serial.print(SPIFFS.format());
    }
    
    WebOut.replace("*sta name*", staName);
    WebOut.replace("*sta pass*", staPass);
    WebOut.replace("*ap name*",  apName);
    WebOut.replace("*ap pass*",  apPass);
    
    server.send(200, "text/html", WebOut);
  });



  server.on("/vars", []()
  {
    String WebOut = AdminBarHTML;
    //WebOut = String("<form action='input'>" + HTMLout + "</form>");

    WebOut += "Variable Dump";
    for (byte i = 0; i <= 50; i++)
    {
      WebOut = String(WebOut + "<hr>" + AllMyVaribles[i][1] + " = " + AllMyVaribles[i][2]);
    }

    server.send(200, "text/html", WebOut);
  });


  server.on("/run", []()
  {
    String WebOut;
    RunningProgram = 1;
    RunningProgramCurrentLine = 0;
    WaitForTheInterpertersResponse = 0 ;
    numberButtonInUse = 0;
    HTMLout = "";
    GraphicsEliments[0][0] = 0;
    WebOut = R"=====(  <meta http-equiv="refresh" content="0; url=./input?" />)=====";

server.send(200, "text/html", WebOut);
});


server.on("/edit", []()
{
<<<<<<< HEAD
  WaitForTheInterpertersResponse = 1;
=======
>>>>>>> parent of 95617bf... Added auto run feature
  String WebOut = AdminBarHTML;
  String TextboxProgramBeingEdited;
  String ProgramName;
  //WebOut = String("<form action='input'>" + HTMLout + "</form>");
  ProgramName = server.arg("name");


  if ( server.arg("open") == "Open" )
  {

    LoadBasicProgramFromFlash(ProgramName);
    TextboxProgramBeingEdited = "";
    for (int i = TotalNumberOfLines - 1; i >= 0; i--)
    {
      delay(0);
      String yada;
      yada = BasicProgram[i];
      yada.trim();
      if (yada != "")  TextboxProgramBeingEdited = String( BasicProgram[i] + String('\n') + TextboxProgramBeingEdited);
    }
  }

  if ( server.arg("save") == "Save" )
  {

    TextboxProgramBeingEdited = GetRidOfurlCharacters(server.arg("code"));

    TextboxProgramBeingEdited.replace("%0D%0A", String(" " + String('\n')));
    for (int i = 1; i <= TotalNumberOfLines - 1; i++)
    {
      BasicProgram[i] = getValueforPrograming(TextboxProgramBeingEdited, '\n', i - 1);
    }
    SaveBasicProgramToFlash(ProgramName);
  }


  WebOut += EditorPageHTML;

  WebOut.replace("*program txt*", TextboxProgramBeingEdited);
  WebOut.replace("*program name*", ProgramName);

  //TextboxProgramBeingEdited

  server.send(200, "text/html", WebOut);
});




server.on("/input", []() {
  server.send(200, "text/html", RunningProgramGui());
});

server.onNotFound ( []() {
  server.send(200, "text/html", RunningProgramGui());
});

LoadBasicProgramFromFlash("");


if (  ConnectToTheWIFI(LoadDataFromFile("WIFIname"), LoadDataFromFile("WIFIpass")) == 0)
{
  if (LoadDataFromFile("APname") == "")
  {
    CreateAP("ESP", "");
  }
  else
  {
    CreateAP(LoadDataFromFile("APname"), LoadDataFromFile("APpass"));
  }
}

Wire.begin(0, 2);

server.begin();
<<<<<<< HEAD
RunningProgram = 0;
WaitForTheInterpertersResponse = 1;
StartUpProgramTimer();
}


void StartUpProgramTimer()
{
  while  (millis() < 30000)
  {
    delay(1000);
    Serial.println(millis());
    server.handleClient();
    if (WaitForTheInterpertersResponse == 0) return;
  }
    RunningProgram = 1;
    RunningProgramCurrentLine = 0;
    WaitForTheInterpertersResponse = 0 ;
    numberButtonInUse = 0;
  return;
=======
RunningProgram = 1;
>>>>>>> parent of 95617bf... Added auto run feature
}


String BasicGraphics()
{
  String BasicGraphicsOut;
  BasicGraphicsOut = GraphicsStartCode;
  BasicGraphicsOut.replace("*wid*",  String(GraphicsEliments[0][1]));
  BasicGraphicsOut.replace("*hei*",  String(GraphicsEliments[0][2]));

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


    GraphicsEliment.replace("*x1*",  String(GraphicsEliments[i][1]));
    GraphicsEliment.replace("*y1*",  String(GraphicsEliments[i][2]));
    GraphicsEliment.replace("*x2*",  String(GraphicsEliments[i][3]));
    GraphicsEliment.replace("*y2*",  String(GraphicsEliments[i][4]));

    if (GraphicsEliments[i][5] == 0 ) GraphicsEliment.replace("*collor*", "black");
    if (GraphicsEliments[i][5] == 1 ) GraphicsEliment.replace("*collor*", "Navy");
    if (GraphicsEliments[i][5] == 2 ) GraphicsEliment.replace("*collor*", "Green");
    if (GraphicsEliments[i][5] == 3 ) GraphicsEliment.replace("*collor*", "Teal");
    if (GraphicsEliments[i][5] == 4 ) GraphicsEliment.replace("*collor*", "Maroon");
    if (GraphicsEliments[i][5] == 5 ) GraphicsEliment.replace("*collor*", "Purple");
    if (GraphicsEliments[i][5] == 6 ) GraphicsEliment.replace("*collor*", "Olive");
    if (GraphicsEliments[i][5] == 7 ) GraphicsEliment.replace("*collor*", "Silver");
    if (GraphicsEliments[i][5] == 8 ) GraphicsEliment.replace("*collor*", "Gray");
    if (GraphicsEliments[i][5] == 9 ) GraphicsEliment.replace("*collor*", "Blue");
    if (GraphicsEliments[i][5] == 10 ) GraphicsEliment.replace("*collor*", "Lime");
    if (GraphicsEliments[i][5] == 11 ) GraphicsEliment.replace("*collor*", "Aqua");
    if (GraphicsEliments[i][5] == 12 ) GraphicsEliment.replace("*collor*", "Red");
    if (GraphicsEliments[i][5] == 13 ) GraphicsEliment.replace("*collor*", "Fuchsia");
    if (GraphicsEliments[i][5] == 14 ) GraphicsEliment.replace("*collor*", "Yellow");
    if (GraphicsEliments[i][5] == 15 ) GraphicsEliment.replace("*collor*", "White");
    BasicGraphicsOut += GraphicsEliment;
  }
  BasicGraphicsOut += "</svg>";
  return BasicGraphicsOut;
}


String RunningProgramGui()
{
  WebArgumentsReceived = server.arg("send");
  WebArgumentsReceivedInput = server.arg("input");
  //PrintAndWebOut(WebArgumentsReceivedInput );

  //Serial.println("Program is running. CHecking for goto statemets");
  CheckFOrWebVarInput();
  if (CheckFOrWebGOTO() == 1)
  {
    WaitForTheInterpertersResponse = 0;
    RunningProgram == 1;
    Serial.println("Running some code befor returning the web page");
    RunBasicTillWait();
    WaitForTheInterpertersResponse = 1;
    //Serial.println("Got to the point Should be returning a web page");
  }

  String WebOut;
  WebOut = String("<form action='input'>" + HTMLout + "</form>");

  String WebOutFinal;
  WebOutFinal = WebOut;

  for (int i = 0; i <= 50; i++)
  {
    WebOutFinal.replace(String("VARS|" + String(i)), AllMyVaribles[i][2]);
  }


  WebOutFinal.replace("**graphics**", BasicGraphics());


  return WebOutFinal;
}



String  getSerialInput()
{
  byte donereceivinginfo = 0;
  Serial.println(">");

  String someInput;
  while (donereceivinginfo == 0)
  {
    delay(10);
    while (Serial.available() > 0)
    {
      char recieved = Serial.read();
      // Process message when new line character is recieved
      if (recieved == '\n')
      {
        Serial.print(someInput);
        donereceivinginfo = 1;
        return someInput;
      }
      someInput += recieved;
    }
  }
}





void loop()
{
  RunBasicTillWait();
  server.handleClient();
}


int RunBasicTillWait()
{
  while (RunningProgram == 1 && RunningProgramCurrentLine < TotalNumberOfLines && WaitForTheInterpertersResponse == 0 )
  {

    delay(1);
    RunningProgramCurrentLine++;
    inData = BasicProgram[RunningProgramCurrentLine];

    ExicuteTheCurrentLine();
  }
  if (RunningProgramCurrentLine > TotalNumberOfLines)
  {
    RunningProgram = 0 ;
  }

}



String getValueforPrograming(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {
    0, -1
  };
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}




String getValue(String data, char separator, int index)
{
  data = String(data + " ");
  int maxIndex = data.length() - 1;
  int j = 0;
  byte WaitingForQuote;
  String chunkVal = "";
  String ChunkReturnVal;
  for (int i = 0; i <= maxIndex && j <= index; i++)
  {
    if (data[i] == '\"' )
    {
      i++;
      while (i <= maxIndex && data[i] != '\"' ) {
        chunkVal.concat(data[i]);
        i++;
      }
    }
    else
    {
      chunkVal.concat(data[i]);
    }

    if (data[i] == separator)
    {
      j++;
      if (j > index)
      {
        chunkVal.trim();
        if (chunkVal != String(separator))
        {
          ChunkReturnVal = chunkVal;
          break;
        }
      }
      chunkVal = "";
    }
  }
  //    Serial.println("index");
  //    Serial.println(index);
  //    Serial.println(j);

  if (j == index + 1)
  {

    return ChunkReturnVal;
  }
}









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
    PrintAndWebOut("Exicuting line Debug Statement");
    PrintAndWebOut(String(String(RunningProgramCurrentLine) + " " + inData));
    PrintAndWebOut(Param0);
    PrintAndWebOut(Param1);
    PrintAndWebOut(Param2);
    PrintAndWebOut(Param3);
    PrintAndWebOut(Param4);
    PrintAndWebOut(Param5);
  }


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
      Param0.toLowerCase();
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
    //RunBasicProgram();
    return;
  }

  if (Param0 == "vars")
  {
    //for debugging purpose to print out all of the basic variables
    PrintAllMyVars();
    return;
  }

  if (Param0 == "list")
  {
    for (int i = 0; i <= TotalNumberOfLines; i++)
    {
      if (BasicProgram[i].length() > 0)
      {
        //PrintAndWebOut(BasicProgram[i].length());
        PrintAndWebOut(String(String(i) + " " + BasicProgram[i]));
      }
    }
    return;
  }


  if (Param0 == "dir")
  {
    SPIFFS.begin();
    Dir dir = SPIFFS.openDir(String("/" ));
    while (dir.next()) {
      File f = dir.openFile("r");
      PrintAndWebOut(String(dir.fileName() + "       " + f.size()));
    }
    return;
  }

  if (Param0 == "del")
  {
    SPIFFS.begin();

    PrintAndWebOut(String("/" + Param1));
    PrintAndWebOut(String(SPIFFS.remove(String("/" + Param1))));
    return;
  }




  //Loading and saving basic program commands
  if (Param0 == "load")
  {
    GraphicsEliments[0][0] = 0;
    PrintAndWebOut(String("Loading . . . . " + Param1));
    LoadBasicProgramFromFlash(Param1);
    numberButtonInUse = 0;
    RunningProgramCurrentLine = 0;
    HTMLout = "";
    return;
  }

  if (Param0 == "save")
  {
    PrintAndWebOut("Saving . . . . ");
    SaveBasicProgramToFlash(Param1);
    return;
  }



  //Commnads to controll pins

  if ( Param0 == "pi")
  {
    valParam1 = GetMeThatVar(Param1).toInt();
    pinMode(valParam1, INPUT);
    SetMeThatVar(Param2, String(digitalRead(valParam1)));
    return;
  }

  if ( Param0 == "po")
  {
    valParam1 = GetMeThatVar(Param1).toInt();
    valParam2 = GetMeThatVar(Param2).toInt();
    pinMode(valParam1, OUTPUT);
    analogWrite(valParam1, 0);
    pinMode(valParam1, OUTPUT);
    digitalWrite(valParam1, valParam2);
    return;
  }


  if ( Param0 == "pwi")
  {
    valParam1 = GetMeThatVar(Param1).toInt();
    pinMode(valParam1, INPUT);
    SetMeThatVar(Param2, String(analogRead(valParam1)));
    return;
  }

  if ( Param0 == "pwo")
  {
    valParam1 = GetMeThatVar(Param1).toInt();
    valParam2 = GetMeThatVar(Param2).toInt();
    pinMode(valParam1, OUTPUT);
    analogWrite(valParam1, valParam2);
    return;
  }


  if ( Param0 == "ai")
  {
    SetMeThatVar(Param1, String(analogRead(A0)));
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






  if (Param0 == "print")
  {
    PrintAndWebOut(GetMeThatVar(Param1));
    return;
  }





  //Web Browser output commands

  if (Param0 == "wprint")
  {
    HTMLout += GetMeThatVar(Param1);
    //Serial.print(HTMLout);
    return;
  }



  if (Param0 == "button")
  {
    numberButtonInUse++;
    String tempButton = GOTObutton;
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
    String tempTextBox = TextBox;
    GetMeThatVar(Param1);
    tempTextBox.replace("variablevalue",  String("VARS|" + String(LastVarNumberLookedUp)));
    tempTextBox.replace("variablenumber",  String(LastVarNumberLookedUp));

    HTMLout = String(HTMLout + tempTextBox);
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
      PrintAndWebOut(GetMeThatVar(Param1));
      SetMeThatVar(Param2, getSerialInput());
    }
    PrintAndWebOut("");
    return;
  }




  //branching commands

  if (Param0 == "goto")
  {
    for (int i = 0; i <= 50; i++) {
      String gotoTest = BasicProgram[i];
      gotoTest.trim();

      if (gotoTest == Param1 | String(gotoTest + ":") == Param1)
      {
        RunningProgramCurrentLine = i - 1;
      }
    }
    return;
  }


  if (Param0 == "gosub")
  {
    for (int i = 0; i <= TotalNumberOfLines; i++) {
      if (BasicProgram[i] == Param1)
      {
        RunningProgramCurrentLine = i - 1;


        NumberOfReturns = NumberOfReturns + 1;
        ReturnLocations[NumberOfReturns] = RunningProgramCurrentLine;
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
    RunningProgram = 0;
    WaitForTheInterpertersResponse = 1;
    PrintAndWebOut("Done...");
    return;
  }




  //Wifi Commands

  if (Param0 == "connect")
  {
    ConnectToTheWIFI(GetMeThatVar(Param1), GetMeThatVar(Param2));
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





  // let command down here for a reason


  if ( Param1 == "=")
  {
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
  return;
}


String DoMathForMe(String cc, String f, String dd )
{
  float e;
  String ee = cc;

  float c = cc.toInt();
  float d = dd.toInt();

  f.trim();
  //  Serial.println(c);
  //  Serial.println(f);
  //  Serial.println(d);

  //sscanf(s, " % d % c % d", c, f, d);
  if (f == "-") {
    e = c - d;
    ee = String(e);
  }
  if (f == "+") {
    e = c + d;
    ee = String(e);
  }
  if (f == "*") {
    e = c * d;
    ee = String(e);
  }
  if (f == "/") {
    e = c / d;
    ee = String(e);
  }
  if (f == "^") {
    e = pow(c , d);
    ee = String(e);
  }



  if (f == "&") {
    ee = String(cc + dd);
  }


  if (f ==  ">") {
    ee = String((c > d));
  }
  if (f ==  "<") {
    ee = String((c < d));
  }


  if (f ==  ">=") {
    ee = String((c >= d));
  }
  if (f ==  "<=") {
    ee = String((c <= d));
  }

  if (f == "<>" || f == " != ")
  {
    if (cc != dd)
    {
      ee = "1";
    }
    else
    {
      if (c != d)
      {
        ee = "1";
      }
    }
  }


  if (f == "==" || f == "=")
  {
    if (cc == dd) {
      ee = "1";
    }
    else
    {
      if (c == d) {
        ee = "1";
      }
    }
  }

  //printf(" % s % d\n", s, e);

  return ee;
}



String GetMeThatVar(String VariableNameToFind)
{
  //PrintAndWebOut(String("Looking for variable " + VariableNameToFind));

  String FunctionName;

  String Param0;
  String Param1;
  String Param2;
  String Param3;
  String Param4;
  String Param5;

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

    Param0.replace(",", "");
    Param1.replace(",", "");
    Param2.replace(",", "");
    Param3.replace(",", "");

    Param0 = VarialbeLookup(Param0);
    Param1 = VarialbeLookup(Param1);
    Param2 = VarialbeLookup(Param2);
    Param3 = VarialbeLookup(Param3);

    FunctionName.toLowerCase();
  }
  delay(0);

  String MyOut;
  MyOut = VarialbeLookup(VariableNameToFind);

  if (VariableNameToFind == "millis") MyOut = String(millis());

  if (FunctionName == "len")   MyOut = String(MyOut.length());
  if (FunctionName == "mid")   MyOut = Mid(Param0, Param1.toFloat(), Param2.toFloat());

  if (FunctionName == "i2cread")    MyOut =  i2cRead(Param0.toFloat(), Param1.toFloat());
  if (FunctionName == "i2cwrite")   MyOut = i2cWrite(Param0.toFloat(), Param1);

  if (FunctionName == "sqr")   MyOut = String(sqrt(MyOut.toFloat()));
  if (FunctionName == "sin")   MyOut = String(sin(MyOut.toFloat()));
  if (FunctionName == "cos")   MyOut = String(cos(MyOut.toFloat()));
  if (FunctionName == "tan")   MyOut = String(tan(MyOut.toFloat()));

  if (FunctionName == "wget")   MyOut = String(FetchWebUrl(MyOut));

  if (FunctionName == "rnd")
  {
    randomSeed(millis());
    MyOut = String(random(MyOut.toFloat()));
  }


  delay(1);

  //  Serial.println("VariableNameToFind=");
  //  Serial.println(VariableNameToFind);


  return MyOut;
}

String VarialbeLookup(String VariableNameToFind)
{
  String MyOut = VariableNameToFind;
  for (byte i = 0; i <= 50; i++)
  {
    if (AllMyVaribles[i][1] == VariableNameToFind)
    {
      delay(0);
      MyOut =  AllMyVaribles[i][2];
      LastVarNumberLookedUp = i;

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
  for (i = pos1; i < pos2; i++)
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


//File System Stuff



void SaveBasicProgramToFlash(String fileNameForSave)
{
  String bla;
  Serial.println("Saving that file");
  SPIFFS.begin();
  File f = SPIFFS.open(String("/" + fileNameForSave + ".bas"), "w");
  if (!f)
  {
    PrintAndWebOut("file open failed");
  }
  else
  {
    for (int i = 0; i <= TotalNumberOfLines; i++)
    {
      //Serial.println(String(BasicProgram[i]  + "---"));
      //f.println(BasicProgram[i]);
      bla = BasicProgram[i];
      bla.trim();
      bla = String(bla + " ");
      f.println(bla);
    }
    f.close();
  }

  Serial.println("done Saving File");
  return;
}



void LoadBasicProgramFromFlash(String fileNameForSave)
{
  SPIFFS.begin();
  File f = SPIFFS.open(String("/" + fileNameForSave + ".bas"), "r");
  if (!f)
  {
    PrintAndWebOut("file open failed");
  }
  else
  {
    for (int i = 0; i <= TotalNumberOfLines; i++)
    {
      BasicProgram[i] = f.readStringUntil('\r');
      BasicProgram[i].replace("\n", "");
    }
    f.close();
  }
  return;
}


void SaveDataToFile(String fileNameForSave, String DataToSave)
{
  SPIFFS.begin();
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
  SPIFFS.begin();
  File f = SPIFFS.open(String(" /data/" + fileNameForSave + ".dat"), "r");
  if (!f)
  {
    PrintAndWebOut("file open failed");
  }
  else
  {
    WhatIwillReturn =  f.readStringUntil('\r');
    WhatIwillReturn.replace("\n", "");
    f.close();
    return WhatIwillReturn;
  }
}



//wifi code here

byte ConnectToTheWIFI(String NetworkName, String NetworkPassword)
{
  WiFi.mode(WIFI_AP_STA);
  byte numberOfAtempts = 0;
  int str_len = NetworkName.length() + 1;
  char ssid[str_len];
  NetworkName.toCharArray(ssid, str_len);

  str_len = NetworkPassword.length() + 1;
  char password[str_len];
  NetworkPassword.toCharArray(password, str_len);


  // Connect to WiFi network
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    numberOfAtempts = numberOfAtempts  + 1;
    delay(1000);
    Serial.print(numberOfAtempts);
    if (numberOfAtempts >= 10)
    {
      Serial.println("");
      Serial.println("Failed Wifi Connect ");
      return 0;
    }
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());


  SaveDataToFile("WIFIname" ,  NetworkName);
  SaveDataToFile("WIFIpass", NetworkPassword);
  return 1;
}




void CreateAP(String NetworkName, String NetworkPassword)
{
  WiFi.mode(WIFI_AP_STA);
  Serial.println("Creating WIFI access point");
  byte numberOfAtempts;
  int str_len = NetworkName.length() + 1;
  char ssid[str_len];
  NetworkName.toCharArray(ssid, str_len);

  str_len = NetworkPassword.length() + 1;
  char password[str_len];
  NetworkPassword.toCharArray(password, str_len);

  WiFi.disconnect();
  delay(2000);
  if (password == "")
  {
    WiFi.softAP(ssid);
  }
  else
  {
    WiFi.softAP(ssid, password);
  }
  delay(2000);

  SaveDataToFile("APname" ,  NetworkName);
  SaveDataToFile("APpass", NetworkPassword);



}



void PrintAndWebOut(String itemToBePrinted)
{
  Serial.println(itemToBePrinted);
  HTMLout = String(HTMLout + "<hr>" + itemToBePrinted);
  return;
}


String GetRidOfurlCharacters(String urlChars)
{
  urlChars.replace("+++",   "***fuck***");
  urlChars.replace("+",   " ");
  urlChars.replace("***fuck***", "+");
  urlChars.replace("%2B", "+");
  urlChars.replace("%2F", "/");
  urlChars.replace("%21", "!");

  urlChars.replace("%0D%0A", String('\n'));

  urlChars.replace("%20", " ");
  urlChars.replace("%21", "!");
  urlChars.replace("%22", String(char('\"')));
  urlChars.replace("%23", "#");
  urlChars.replace("%24", "$");
  urlChars.replace("%25", "%");
  urlChars.replace("%26", "&");
  urlChars.replace("%27", String(char(39)));
  urlChars.replace("%28", "(");
  urlChars.replace("%29", ")");
  urlChars.replace("%2A", "*");
  urlChars.replace("%2B", "+");
  urlChars.replace("%2C", ",");
  urlChars.replace("%2D", "-");
  urlChars.replace("%2E", ".");
  urlChars.replace("%2F", "/");
  urlChars.replace("%30", "0");
  urlChars.replace("%31", "1");
  urlChars.replace("%32", "2");
  urlChars.replace("%33", "3");
  urlChars.replace("%34", "4");
  urlChars.replace("%35", "5");
  urlChars.replace("%36", "6");
  urlChars.replace("%37", "7");
  urlChars.replace("%38", "8");
  urlChars.replace("%39", "9");
  urlChars.replace("%3A", ":");
  urlChars.replace("%3B", ";");
  urlChars.replace("%3C", "<");
  urlChars.replace("%3D", "=");
  urlChars.replace("%3E", ">");
  urlChars.replace("%3F", "?");
  urlChars.replace("%40", "@");
  urlChars.replace("%41", "A");
  urlChars.replace("%42", "B");
  urlChars.replace("%43", "C");
  urlChars.replace("%44", "D");
  urlChars.replace("%45", "E");
  urlChars.replace("%46", "F");
  urlChars.replace("%47", "G");
  urlChars.replace("%48", "H");
  urlChars.replace("%49", "I");
  urlChars.replace("%4A", "J");
  urlChars.replace("%4B", "K");
  urlChars.replace("%4C", "L");
  urlChars.replace("%4D", "M");
  urlChars.replace("%4E", "N");
  urlChars.replace("%4F", "O");
  urlChars.replace("%50", "P");
  urlChars.replace("%51", "Q");
  urlChars.replace("%52", "R");
  urlChars.replace("%53", "S");
  urlChars.replace("%54", "T");
  urlChars.replace("%55", "U");
  urlChars.replace("%56", "V");
  urlChars.replace("%57", "W");
  urlChars.replace("%58", "X");
  urlChars.replace("%59", "Y");
  urlChars.replace("%5A", "Z");
  urlChars.replace("%5B", "[");
  urlChars.replace("%5C", String(char(65)));
  urlChars.replace("%5D", "]");
  urlChars.replace("%5E", "^");
  urlChars.replace("%5F", "_");
  urlChars.replace("%60", "`");
  urlChars.replace("%61", "a");
  urlChars.replace("%62", "b");
  urlChars.replace("%63", "c");
  urlChars.replace("%64", "d");
  urlChars.replace("%65", "e");
  urlChars.replace("%66", "f");
  urlChars.replace("%67", "g");
  urlChars.replace("%68", "h");
  urlChars.replace("%69", "i");
  urlChars.replace("%6A", "j");
  urlChars.replace("%6B", "k");
  urlChars.replace("%6C", "l");
  urlChars.replace("%6D", "m");
  urlChars.replace("%6E", "n");
  urlChars.replace("%6F", "o");
  urlChars.replace("%70", "p");
  urlChars.replace("%71", "q");
  urlChars.replace("%72", "r");
  urlChars.replace("%73", "s");
  urlChars.replace("%74", "t");
  urlChars.replace("%75", "u");
  urlChars.replace("%76", "v");
  urlChars.replace("%77", "w");
  urlChars.replace("%78", "x");
  urlChars.replace("%79", "y");
  urlChars.replace("%7A", "z");
  //urlChars.replace("%7B","{");
  urlChars.replace("%7C", "|");
  //urlChars.replace("%7D","}");
  urlChars.replace("%7E", "~");
  urlChars.replace("%7F", "Â");
  urlChars.replace("%80", "`");
  urlChars.replace("%81", "Â");
  urlChars.replace("%82", "â");
  urlChars.replace("%83", "Æ");
  urlChars.replace("%84", "â");
  urlChars.replace("%85", "â¦");
  urlChars.replace("%86", "â");
  urlChars.replace("%87", "â¡");
  urlChars.replace("%88", "Ë");
  urlChars.replace("%89", "â°");
  urlChars.replace("%8A", "Å");
  urlChars.replace("%8B", "â¹");
  urlChars.replace("%8C", "Å");
  urlChars.replace("%8D", "Â");
  urlChars.replace("%8E", "Å½");
  urlChars.replace("%8F", "Â");
  urlChars.replace("%90", "Â");
  urlChars.replace("%91", "â");
  urlChars.replace("%92", "â");
  urlChars.replace("%93", "â");
  urlChars.replace("%94", "â");
  urlChars.replace("%95", "â¢");
  urlChars.replace("%96", "â");
  urlChars.replace("%97", "â");
  urlChars.replace("%98", "Ë");
  urlChars.replace("%99", "â¢");
  urlChars.replace("%9A", "Å¡");
  urlChars.replace("%9B", "âº");
  urlChars.replace("%9C", "Å");
  urlChars.replace("%9D", "Â");
  urlChars.replace("%9E", "Å¾");
  urlChars.replace("%9F", "Å¸");
  urlChars.replace("%A0", "Â");
  urlChars.replace("%A1", "Â¡");
  urlChars.replace("%A2", "Â¢");
  urlChars.replace("%A3", "Â£");
  urlChars.replace("%A4", "Â¤");
  urlChars.replace("%A5", "Â¥");
  urlChars.replace("%A6", "Â¦");
  urlChars.replace("%A7", "Â§");
  urlChars.replace("%A8", "Â¨");
  urlChars.replace("%A9", "Â©");
  urlChars.replace("%AA", "Âª");
  urlChars.replace("%AB", "Â«");
  urlChars.replace("%AC", "Â¬");
  urlChars.replace("%AE", "Â®");
  urlChars.replace("%AF", "Â¯");
  urlChars.replace("%B0", "Â°");
  urlChars.replace("%B1", "Â±");
  urlChars.replace("%B2", "Â²");
  urlChars.replace("%B3", "Â³");
  urlChars.replace("%B4", "Â´");
  urlChars.replace("%B5", "Âµ");
  urlChars.replace("%B6", "Â¶");
  urlChars.replace("%B7", "Â·");
  urlChars.replace("%B8", "Â¸");
  urlChars.replace("%B9", "Â¹");
  urlChars.replace("%BA", "Âº");
  urlChars.replace("%BB", "Â»");
  urlChars.replace("%BC", "Â¼");
  urlChars.replace("%BD", "Â½");
  urlChars.replace("%BE", "Â¾");
  urlChars.replace("%BF", "Â¿");
  urlChars.replace("%C0", "Ã");
  urlChars.replace("%C1", "Ã");
  urlChars.replace("%C2", "Ã");
  urlChars.replace("%C3", "Ã");
  urlChars.replace("%C4", "Ã");
  urlChars.replace("%C5", "Ã");
  urlChars.replace("%C6", "Ã");
  urlChars.replace("%C7", "Ã");
  urlChars.replace("%C8", "Ã");
  urlChars.replace("%C9", "Ã");
  urlChars.replace("%CA", "Ã");
  urlChars.replace("%CB", "Ã");
  urlChars.replace("%CC", "Ã");
  urlChars.replace("%CD", "Ã");
  urlChars.replace("%CE", "Ã");
  urlChars.replace("%CF", "Ã");
  urlChars.replace("%D0", "Ã");
  urlChars.replace("%D1", "Ã");
  urlChars.replace("%D2", "Ã");
  urlChars.replace("%D3", "Ã");
  urlChars.replace("%D4", "Ã");
  urlChars.replace("%D5", "Ã");
  urlChars.replace("%D6", "Ã");
  urlChars.replace("%D7", "Ã");
  urlChars.replace("%D8", "Ã");
  urlChars.replace("%D9", "Ã");
  urlChars.replace("%DA", "Ã");
  urlChars.replace("%DB", "Ã");
  urlChars.replace("%DC", "Ã");
  urlChars.replace("%DD", "Ã");
  urlChars.replace("%DE", "Ã");
  urlChars.replace("%DF", "Ã");
  urlChars.replace("%E0", "Ã");
  urlChars.replace("%E1", "Ã¡");
  urlChars.replace("%E2", "Ã¢");
  urlChars.replace("%E3", "Ã£");
  urlChars.replace("%E4", "Ã¤");
  urlChars.replace("%E5", "Ã¥");
  urlChars.replace("%E6", "Ã¦");
  urlChars.replace("%E7", "Ã§");
  urlChars.replace("%E8", "Ã¨");
  urlChars.replace("%E9", "Ã©");
  urlChars.replace("%EA", "Ãª");
  urlChars.replace("%EB", "Ã«");
  urlChars.replace("%EC", "Ã¬");
  urlChars.replace("%ED", "Ã­");
  urlChars.replace("%EE", "Ã®");
  urlChars.replace("%EF", "Ã¯");
  urlChars.replace("%F0", "Ã°");
  urlChars.replace("%F1", "Ã±");
  urlChars.replace("%F2", "Ã²");
  urlChars.replace("%F3", "Ã³");
  urlChars.replace("%F4", "Ã´");
  urlChars.replace("%F5", "Ãµ");
  urlChars.replace("%F6", "Ã¶");
  urlChars.replace("%F7", "Ã·");
  urlChars.replace("%F8", "Ã¸");
  urlChars.replace("%F9", "Ã¹");
  urlChars.replace("%FA", "Ãº");
  urlChars.replace("%FB", "Ã»");
  urlChars.replace("%FC", "Ã¼");
  urlChars.replace("%FD", "Ã½");
  urlChars.replace("%FE", "Ã¾");
  urlChars.replace("%FF", "Ã¿");

  return urlChars;
}


byte CheckFOrWebGOTO()
{
  String bla;
  byte x = 0;



  for (int i = 0; i <= 50; i++)
  {
    int str_len = String(i).length() + 1 + 4;
    char ArgumentToTest[str_len];
    String(String("goto" + String(i))).toCharArray(ArgumentToTest, str_len);

    bla = server.arg(ArgumentToTest);
    if (bla.length() > 0)
    {
      x = i;
    }
  }

  Serial.println(x);
  if (x != 0)
  {
    for (int i = 0; i <= 254; i++) {
      delay(1);
      if (getValue(BasicProgram[i], ' ', 0) == ButtonsInUse[x])
      {
        //Serial.println("This is the line I am going to");
        Serial.println(BasicProgram[i]);
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


  for (int i = 0; i <= 50; i++)
  {
    int str_len = String(i).length() + 1;
    char ArgumentToTest[str_len];
    String(i).toCharArray(ArgumentToTest, str_len);

    bla = server.arg(ArgumentToTest);
    if (bla.length() > 0)
    {
      AllMyVaribles[i][2] = GetRidOfurlCharacters(bla);
    }
  }
  return;
}

String FetchWebUrl(String URLtoGet)
{
  String str;
  String ServerToConnectTo;
  String PageToGet;
  ServerToConnectTo = URLtoGet.substring(0, URLtoGet.indexOf("/"));
  PageToGet = URLtoGet.substring(URLtoGet.indexOf("/"));

  Serial.println(ServerToConnectTo);
  Serial.println(PageToGet);

  WiFiClient client;
  if (client.connect(ServerToConnectTo.c_str() , 80))
  {
    client.print(String("GET " + PageToGet + " HTTP/1.1\r\nHost: " + ServerToConnectTo + "\r\n\r\n"));
    delay(100);
    str = "";
    while (client.available())
    {
      str += String((const char)client.read());
      delay(0);
    }
    return str;
  }
  return "";
}



//i2c stuff

String i2cRead(byte DeviceNo, byte NoOfByteToRequest)
{
  String i2cReturn;

  Wire.requestFrom(DeviceNo, NoOfByteToRequest);

  while (Wire.available())
  {
    delay(0);
    i2cReturn += Wire.read();
  }
  return i2cReturn;
}


String i2cWrite(byte DeviceNo, String DataToSend)
{
  Wire.beginTransmission(DeviceNo);
  Wire.write(DataToSend.c_str());
  return String(Wire.endTransmission());
}
