//ESP8266 Basic Interperter
//HTTP://ESP8266BASIC.COM
//
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



//Onewire tempture sensoor code conntributed by Rotohammer.

#include "spiffs/spiffs.h"
#include <FS.h>
#include <ESP8266mDNS.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
//#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <ESP8266httpUpdate.h>
#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>

String BasicVersion = "ESP Basic 1.28";


OneWire oneWire(5);
DallasTemperature sensors(&oneWire);





ESP8266WebServer server(80);

//Web Server Variables
String HTMLout;
const String InputFormText = R"=====( <input type="text" name="input"><input type="submit" value="Submit" name="inputButton"><hr>)=====";
const String TextBox = R"=====( <input type="text" name="variablenumber" value="variablevalue">)=====";
const String Slider = R"=====( <input type="range" name="variablenumber" min="minval" max="maxval" value=variablevalue>)=====";
const String GOTObutton =  R"=====(<input type="submit" value="gotonotext" name="gotonobranch">)=====";
const String GOTOimagebutton =  R"=====(<input type="image" src="/file?file=gotonotext" value="gotonotext" name="gotonobranch">)=====";
const String normalImage =  R"=====(<img src="/file?file=name")=====";
const String javascript =  R"=====(<script src="/file?file=name"></script>)=====";
const String DropDownList =  R"=====(<select name="variablenumber" size="theSize">options</select>
<script>document.getElementsByName("variablenumber")[0].value = "VARS|variablenumber";</script>)=====";
const String DropDownListOpptions =  R"=====(<option>item</option>)=====";



byte WaitForTheInterpertersResponse = 1;

const String AdminBarHTML = R"=====(
<a href="./vars">[ VARS ]</a> 
<a href="./edit">[ EDIT ]</a>
<a href="./run">[ RUN ]</a>
<a href="./settings">[ SETTINGS ]</a>
<a href="./filemng">[ FILE MANAGER ]</a>
<hr>)=====";



const String UploadPage = R"=====(
<form method='POST' action='/filemng' enctype='multipart/form-data'>
<input type='file' name='Upload'>
<input type='submit' value='Upload'>
</form>
<form id="filelist">
<input type="submit" value="Delete" name="Delete">
<input type="submit" value="View" name="View">
</form>

<select name="fileName" size="25" form="filelist">*table*</select>
)=====";



//<a href="http://www.esp8266basic.com/help"  target="_blank">[ HELP ]</a>

const String EditorPageHTML =  R"=====(
<script src="editor.js"></script>
<form action='edit' id="usrform">
<input type="text" name="name" value="*program name*">
<input type="submit" value="Open" name="open">
</form><button onclick="SaveTheCode()">Save</button>
<br>
<textarea rows="30" style="width:100%" name="code" id="code">*program txt*</textarea><br>
<input type="text" id="Status" value="">
)=====";


const String editCodeJavaScript =  R"=====(
function SaveTheCode() {
  var textArea = document.getElementById("code");
  var arrayOfLines = textArea.value.split("\n");
  httpGet("/codein?SaveTheCode=yes");
for (i = 0; i <= arrayOfLines.length - 1; i++) 
{ 
  var x = i + 1;
  if (arrayOfLines[i] != "undefined")
  {
    arrayOfLines[i] = arrayOfLines[i].replace("+", "%2B");
    arrayOfLines[i] = arrayOfLines[i].replace("&", "%26");
    var WhatToSend = "/codein?line=" + x + "&code=" + encodeURI(arrayOfLines[i]);
    httpGet(WhatToSend);
    document.getElementById("Status").value = i.toString();
  }
}

document.getElementById("Status").value = "Saved";
alert("Saved");
}
function httpGet(theUrl)
{
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open( "GET", theUrl, false ); // false for synchronous request
    xmlHttp.send( null );
    return xmlHttp.responseText;
}
)=====";


const String SettingsPageHTML =  R"=====(
<form action='settings' id="usrform"><table>
*BasicVersion*
<tr><th>
Station Mode (Connect to your router):</th></tr>
<tr><th><p align="right">Name:</p></th><th><input type="text" name="staName" value="*sta name*"></th></tr>
<tr><th><p align="right">Pass:</p></th><th><input type="text" name="staPass" value="*sta pass*"></th></tr>
<tr><th>
<br><br>Ap mode (ESP brocast out its own ap):</th></tr>
<tr><th><p align="right">Name:</p></th><th><input type="text" name="apName" value="*ap name*"></th></tr>
<tr><th><p align="right">Pass:<br>Must be at least 8 characters</p></th><th><input type="text" name="apPass" value="*ap pass*"></th></tr>
<tr><th>
<br><br>Log In Key (For Security):</th></tr>
<tr><th><p align="right">Log In Key:</p></th><th><input type="text" name="LoginKey" value="*LoginKey*"></th></tr>
<tr><th><p align="right">Display menu bar on index page:</p></th><th><input type="checkbox" name="showMenueBar" value="off" **checked**> Disable<br></th></tr>
<tr><th>
<input type="submit" value="Save" name="save"></th>
<th><input type="submit" value="Format" name="format"></th>
<th><input type="submit" value="Update" name="update"></th>
</tr>
</table></form>
<br>
)=====";







const String LogInPage =  R"=====(
<form action='settings' id="usrform">
Log In Key
<input type="text" name="key" value="">
<input type="submit" value="login" name="login">
</form>
)=====";


//Graphics HTML CODE

const String GraphicsStartCode =  R"=====(<svg width="*wid*" height="*hei*">)=====";

const String GraphicsLineCode =  R"=====(<line x1="*x1*" y1="*y1*" x2="*x2*" y2="*y2*" stroke="*collor*"/>)=====";

const String GraphicsCircleCode =  R"=====(<circle cx="*x1*" cy="*y1*" r="*x2*" fill="*collor*"/>)=====";

const String GraphicsEllipseCode =  R"=====(<ellipse cx="*x1*" cy="*y1*" rx="*x2*" ry="*y2*" fill="*collor*"/>)=====";

const String GraphicsRectangleCode =  R"=====(<rect x="*x1*" y="*y1*" width="*x2*" height="*y2*" style="fill:*collor*"/>)=====";



String WebArgumentsReceived;
String WebArgumentsReceivedInput;
byte numberButtonInUse = 0;
String ButtonsInUse[11];


String   msgbranch;
String   MsgBranchRetrnData;


// Buffer to store incoming commands from serial port
String inData;

int TotalNumberOfLines = 255;
//String BasicProgram[255];                                //Array of strings to hold basic program


String AllMyVaribles[50][2];
int LastVarNumberLookedUp;                                 //Array to hold all of the basic variables



bool RunningProgram = 1;                                //Will be set to 1 if the program is currently running
byte RunningProgramCurrentLine = 0;                     //Keeps track of the currently running line of code
byte NumberOfReturns;
bool BasicDebuggingOn;
byte ReturnLocations[254];

int TimerWaitTime;
int timerLastActiveTime;
String TimerBranch;

String refreshBranch;

int GraphicsEliments[100][7];

File fsUploadFile;

int noOfLinesForEdit;
String ProgramName;

//int IOstatus[20][2];

bool inputPromptActive = 0;

int LoggedIn = 0;


Servo Servo0;
Servo Servo1;
Servo Servo2;
Servo Servo3;
Servo Servo4;
Servo Servo5;
Servo Servo6;
Servo Servo7;
Servo Servo8;
Servo Servo9;
Servo Servo10;
Servo Servo11;
Servo Servo12;
Servo Servo13;
Servo Servo14;
Servo Servo15;
Servo Servo16;

String  PinListOfStatus[16];
int  PinListOfStatusValues[16];




void setup() {
  SPIFFS.begin();
  Serial.begin(9600);
  //Serial.setDebugOutput(true);
  WiFi.mode(WIFI_AP_STA);
  PrintAndWebOut(BasicVersion);

  //CheckWaitForRunningCode();

  server.on("/", []()
  {
    String WebOut;
    if (LoadDataFromFile("ShowMenueBar") != "off") WebOut =    AdminBarHTML;
    WebOut += RunningProgramGui();
    server.send(200, "text/html", WebOut);
  });


  server.on("/settings", []()
  {

    if ( server.arg("key") == LoadDataFromFile("LoginKey"))
    {
      LoggedIn = millis();
    }


    WaitForTheInterpertersResponse = 1;
    String WebOut = AdminBarHTML;
    WebOut += SettingsPageHTML;
    String staName;
    String staPass;
    String apName;
    String apPass;
    String LoginKey;
    String ShowMenueBar;
    //Serial.print("Loading Settings Files");

    staName      = LoadDataFromFile("WIFIname");
    staPass      = LoadDataFromFile("WIFIpass");
    apName       = LoadDataFromFile("APname");
    apPass       = LoadDataFromFile("APpass");
    LoginKey     = LoadDataFromFile("LoginKey");
    ShowMenueBar = LoadDataFromFile("ShowMenueBar");

    if (millis() > LoggedIn + 600000 || LoggedIn == 0 )
    {
      WebOut = LogInPage;
    }
    else
    {

      if ( server.arg("update") == "Update" )
      {
        t_httpUpdate_return  ret = ESPhttpUpdate.update("172.16.0.5", 80, "/test.bin");
        switch (ret) {
          case HTTP_UPDATE_FAILD:
            Serial.println("HTTP_UPDATE_FAILD");
            break;

          case HTTP_UPDATE_NO_UPDATES:
            Serial.println("HTTP_UPDATE_NO_UPDATES");
            break;

          case HTTP_UPDATE_OK:
            Serial.println("HTTP_UPDATE_OK");
            break;
        }
      }


      if ( server.arg("save") == "Save" )
      {
        staName = server.arg("staName");
        staPass = server.arg("staPass");
        apName  = server.arg("apName");
        apPass  = server.arg("apPass");
        LoginKey = server.arg("LoginKey");
        ShowMenueBar = server.arg("showMenueBar");

        SaveDataToFile("WIFIname" , staName);
        SaveDataToFile("WIFIpass" , staPass);
        SaveDataToFile("APname" , apName);
        SaveDataToFile("APpass" , apPass);
        SaveDataToFile("LoginKey" , LoginKey);
        SaveDataToFile("ShowMenueBar" , ShowMenueBar);
      }

      if ( server.arg("format") == "Format" )
      {
        Serial.println("Formating ");
        Serial.print(SPIFFS.format());
      }

      WebOut.replace("*sta name*", staName);
      WebOut.replace("*sta pass*", staPass);
      WebOut.replace("*ap name*",  apName);
      WebOut.replace("*ap pass*",  apPass);
      WebOut.replace("*LoginKey*", LoginKey);
      WebOut.replace("*BasicVersion*", BasicVersion);

      if ( ShowMenueBar == "off")
      {
        WebOut.replace("**checked**", "checked");
      }
      else
      {
        WebOut.replace("**checked**", "");
      }



    }



    server.send(200, "text/html", WebOut);
  });



  server.on("/vars", []()
  {
    String WebOut = AdminBarHTML;
    if (millis() > LoggedIn + 600000 || LoggedIn == 0 )
    {
      WebOut = LogInPage;
    }
    else
    {
      WebOut += "<div>Variable Dump:";
      for (byte i = 0; i <= 50; i++)
      {
        if (AllMyVaribles[i][1] != "" ) WebOut += String("<hr>" + AllMyVaribles[i][1] + " = " + AllMyVaribles[i][2]);
      }


      WebOut += "<hr></div><div>Pin Stats";
      for (byte i = 0; i <= 16; i++)
      {
        WebOut += String("<hr>" + String(i) + " = " + PinListOfStatus[i] + "  , " + String(PinListOfStatusValues[i]));
      }
      WebOut += "</div>";
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
    TimerWaitTime = 0;
    GraphicsEliments[0][0] = 0;
    WebOut = R"=====(  <meta http-equiv="refresh" content="0; url=./input?" />)=====";

    server.send(200, "text/html", WebOut);
  });



  server.onFileUpload(handleFileUpdate);

  server.on("/filemng", []()
  {
    DoSomeFileManagerCode();
  });


  server.on("/edit", []()
  {
    String WebOut = AdminBarHTML;
    if (millis() > LoggedIn + 600000 || LoggedIn == 0 )
    {
      WebOut = LogInPage;
    }
    else
    {

      WaitForTheInterpertersResponse = 1;

      String TextboxProgramBeingEdited;
      //String ProgramName;
      //WebOut = String("<form action='input'>" + HTMLout + "</form>");
      ProgramName = server.arg("name");


      if ( server.arg("open") == "Open" )
      {

        //LoadBasicProgramFromFlash(ProgramName);
        TextboxProgramBeingEdited = "";
        for (int i = TotalNumberOfLines - 1; i >= 0; i--)
        {
          delay(0);
          String yada;
          yada = BasicProgram(i);
          yada.trim();
          if (yada != "")  TextboxProgramBeingEdited = String( BasicProgram(i) + String('\n') + TextboxProgramBeingEdited);
        }
      }


      WebOut += EditorPageHTML;

      WebOut.replace("*program txt*", TextboxProgramBeingEdited);
      WebOut.replace("*program name*", ProgramName);

      //TextboxProgramBeingEdited
    }
    server.send(200, "text/html", WebOut);
  });



  server.on("/editor.js", []() {
    server.send(200, "text/html", editCodeJavaScript);
  });


  server.on("/codein", []() {

    if (ProgramName == "")
    {
      ProgramName = "default";
    }


    if (server.arg("SaveTheCode") != "yes")
    {
      String LineNoForWebEditorIn;
      LineNoForWebEditorIn = server.arg("line");
      int y = LineNoForWebEditorIn.toInt();
      delay(0);
      BasicProgramWriteLine(y, GetRidOfurlCharacters(server.arg("code")));
      delay(0);
      noOfLinesForEdit = y;

    }
    else
    {

      String directoryToDeleteFilesFrom;
      directoryToDeleteFilesFrom = String(" /data/" + ProgramName );
      Dir dir1 = SPIFFS.openDir(directoryToDeleteFilesFrom);

      while (dir1.next())
      {
        delay(0);
        File f = dir1.openFile("r");
        if (dir1.fileName().substring(0, directoryToDeleteFilesFrom.length()) == directoryToDeleteFilesFrom) SPIFFS.remove(dir1.fileName());
      }
    }
    server.send(200, "text/html", "good");
  });





  server.on("/msg", []() {

    MsgBranchRetrnData = "No MSG Branch Defined";

    if (msgbranch != "")
    {
      inData = String(" goto " + msgbranch + " ");
      WaitForTheInterpertersResponse = 0;
      ExicuteTheCurrentLine();
      runTillWaitPart2();
    }


    server.send(200, "text/html", MsgBranchRetrnData);
  });





  server.on("/input", []() {
    server.send(200, "text/html", RunningProgramGui());
  });

  server.onNotFound ( []() {
    String fileNameToServeUp;
    fileNameToServeUp = GetRidOfurlCharacters(server.arg("file"));
    File mySuperFile = SPIFFS.open(String("uploads/" + fileNameToServeUp), "r");
    if (mySuperFile)
    {
      server.streamFile(mySuperFile, getContentType(fileNameToServeUp));
      //server.send(200, getContentType(fileNameToServeUp), mySuperFile.readString());

    }
    else
    {
      server.send(200, "text/html", RunningProgramGui());
    }
    mySuperFile.close();
  });

  //LoadBasicProgramFromFlash("");


  if (  ConnectToTheWIFI(LoadDataFromFile("WIFIname"), LoadDataFromFile("WIFIpass"), "", "", "") == 0)
  {
    if (LoadDataFromFile("APname") == "")
    {
      CreateAP("", "");
    }
    else
    {
      CreateAP(LoadDataFromFile("APname"), LoadDataFromFile("APpass"));
    }
  }


  Wire.begin(0, 2);
  sensors.begin();

  server.begin();
  RunningProgram = 0;
  WaitForTheInterpertersResponse = 1;
  StartUpProgramTimer();
}


String getContentType(String filename) {
  if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

void StartUpProgramTimer()
{
  while  (millis() < 30000)
  {
    //delay(1000);
    //Serial.println(millis());
    server.handleClient();
    if (WaitForTheInterpertersResponse == 0) return;
  }
  Serial.println("Starting Default Program");
  RunningProgram = 1;
  RunningProgramCurrentLine = 0;
  WaitForTheInterpertersResponse = 0 ;
  numberButtonInUse = 0;
  HTMLout = "";
  return;
}



void DoSomeFileManagerCode()
{
  String WholeUploadPage = UploadPage;
  String FileListForPage ;

  if (millis() > LoggedIn + 600000 || LoggedIn == 0 )
  {
    WholeUploadPage = LogInPage;
  }
  else
  {
    if (server.arg("Delete") != "")
    {
      String FIleNameForDelete = server.arg("fileName");
      FIleNameForDelete = GetRidOfurlCharacters(FIleNameForDelete);
      Serial.println(FIleNameForDelete);
      SPIFFS.remove(FIleNameForDelete);
      //Serial.println(SPIFFS.remove("uploads/settings.png"));
    }

    Dir dir = SPIFFS.openDir(String("uploads" ));
    while (dir.next()) {
      FileListForPage += String("<option>" + dir.fileName() + "</option>");
      delay(0);
    }

    WholeUploadPage.replace("*table*", FileListForPage);

    if (server.arg("View") != "")
    {
      String FileNameToView = server.arg("fileName");
      FileNameToView = GetRidOfurlCharacters(FileNameToView);
      FileNameToView.replace("uploads/", "");
      WholeUploadPage = R"=====(  <meta http-equiv="refresh" content="0; url=./file?file=item" />)=====";
      WholeUploadPage.replace("item", FileNameToView);
    }

  }
  server.send(200, "text/html",  String( AdminBarHTML + WholeUploadPage ));
}



void handleFileUpdate()
{
  //if (server.uri() != "/edit") return;
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    //DBG_OUTPUT_PORT.print("Upload Name: "); DBG_OUTPUT_PORT.println(filename);
    fsUploadFile = SPIFFS.open(String("uploads/" + filename), "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    //DBG_OUTPUT_PORT.print("Upload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile)
      fsUploadFile.close();
    //DBG_OUTPUT_PORT.print("Upload Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
  }
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

  String WebOut = String("<form action='input'>" + HTMLout + "</form>");


  if (BasicDebuggingOn == 1)
  {
    Serial.println("Web out first");
    Serial.println( WebOut);
    Serial.println("HTML out");
    Serial.println( HTMLout);
  }

  for (int i = 50; i >= 0; i--)
  {
    delay(0);
    WebOut.replace(String("VARS|" + String(i)), AllMyVaribles[i][2]);
  }


  WebOut.replace("**graphics**", BasicGraphics());

  if (BasicDebuggingOn == 1)
  {
    Serial.println( WebOut);
  }
  return WebOut;
}



String  getSerialInput()
{
  byte donereceivinginfo = 0;
  Serial.println(">");

  String someInput;
  while (donereceivinginfo == 0)
  {
    delay(0);
    while (Serial.available() > 0)
    {
      char recieved = Serial.read();
      // Process message when new line character is recieved
      if (recieved == '\n')
      {
        Serial.println(someInput);
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



void RunBasicTillWait()
{
  runTillWaitPart2();
  if (RunningProgramCurrentLine > TotalNumberOfLines)
  {
    RunningProgram = 0 ;
    TimerWaitTime = 0;
    return;
  }

  if (TimerWaitTime + timerLastActiveTime <= millis() &  TimerWaitTime != 0)
  {
    inData = String(" goto " + TimerBranch + " ");
    WaitForTheInterpertersResponse = 0;
    timerLastActiveTime = millis() ;
    ExicuteTheCurrentLine();
    runTillWaitPart2();
  }
}


void runTillWaitPart2()
{
  while (RunningProgram == 1 && RunningProgramCurrentLine < TotalNumberOfLines && WaitForTheInterpertersResponse == 0 )
  {
    delay(0);
    RunningProgramCurrentLine++;
    inData = BasicProgram(RunningProgramCurrentLine);
    ExicuteTheCurrentLine();
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
  data = String(data + "           ");
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
        delay(0);
      }
    }
    else if (data[i] == '|' )
    {
      i++;
      while (i <= maxIndex && data[i] != '|' ) {
        chunkVal.concat(data[i]);
        i++;
        delay(0);
      }
    }
    else
    {
      if (data[i] != separator) chunkVal.concat(data[i]);
    }

    if (data[i] == separator & data[i - 1] != separator)
    {
      j++;
      if (j > index)
      {
        //chunkVal.trim();
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
    Serial.println("Exicuting line Debug Statement");
    Serial.println(String(String(RunningProgramCurrentLine) + " " + inData));
    Serial.println(Param0);
    Serial.println(Param1);
    Serial.println(Param2);
    Serial.println(Param3);
    Serial.println(Param4);
    Serial.println(Param5);
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
    Param0 = "let";
  }



  if (Param0 == "next")
  {
    //for x = 1 to 10

    for (int i = RunningProgramCurrentLine; i >= 1; i--)
    {
      delay(0);
      String gotoTestFor = BasicProgram(i);
      gotoTestFor.trim();

      String ForTest = getValue(gotoTestFor, ' ', 0);
      ForTest.toLowerCase();
      String VarTest = getValue(gotoTestFor, ' ', 1);

      String VarTestIfDone = getValue(gotoTestFor, ' ', 5);
      if (ForTest == "for" &  VarTest == Param1)
      {
        float test1 = GetMeThatVar(VarTest).toFloat();
        float test2 = GetMeThatVar(VarTestIfDone).toFloat();

        //Serial.println(test1);
        //Serial.println(test2);

        if ( test1  !=  test2 )
        {
          RunningProgramCurrentLine = i;
          Param0 = "let";
          Param3 = Param1;
          Param4 = "+";
          Param5 = "1";
          break;
        }
        else
        {
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




  //Loading and saving basic program commands
  if (Param0 == "load")
  {
    GraphicsEliments[0][0] = 0;
    PrintAndWebOut(String("Loading . . . . " + Param1));
    ProgramName = Param1;
    numberButtonInUse = 0;
    RunningProgramCurrentLine = 0;
    HTMLout = "";
    TimerWaitTime = 0;
    return;
  }





  //Commnads to controll pins

  if ( Param0 == "pi")
  {
    valParam1 = GetMeThatVar(Param1).toInt();

    SetMeThatVar(Param2, String(UniversalPinIO("pi", valParam1, 0)));
    return;
  }

  if ( Param0 == "po")
  {
    valParam1 = GetMeThatVar(Param1).toInt();
    valParam2 = GetMeThatVar(Param2).toInt();

    UniversalPinIO("po", valParam1, valParam2);
    return;
  }


  if ( Param0 == "pwi")
  {
    valParam1 = GetMeThatVar(Param1).toInt();

    SetMeThatVar(Param2, String(UniversalPinIO("pwi", valParam1, 0)));
    return;
  }

  if ( Param0 == "pwo")
  {
    valParam1 = GetMeThatVar(Param1).toInt();
    valParam2 = GetMeThatVar(Param2).toInt();

    UniversalPinIO("pwo", valParam1, valParam2);
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
    valParam1 = GetMeThatVar(Param1).toInt();
    valParam2 = GetMeThatVar(Param2).toInt();

    UniversalPinIO("servo", valParam1, valParam2);
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


  //Web Browser output commands

  if (Param0 == "wprint")
  {
    HTMLout += GetMeThatVar(Param1);
    //Serial.print(HTMLout);
    return;
  }



  if (Param0 == "image")
  {
    String tempInfo = normalImage;
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



  if (Param0 == "imagebutton")
  {
    numberButtonInUse++;
    String tempButton = GOTOimagebutton;
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


  if (Param0 == "slider")
  {
    String tempSlider = Slider;
    GetMeThatVar(Param1);
    tempSlider.replace("variablevalue",  String("VARS|" + String(LastVarNumberLookedUp)));
    tempSlider.replace("variablenumber",  String(LastVarNumberLookedUp));

    tempSlider.replace("minval",  GetMeThatVar(Param2));
    tempSlider.replace("maxval",  GetMeThatVar(Param3));

    HTMLout = String(HTMLout + tempSlider);
    return;
  }




  if (Param0 == "dropdown" | Param0 == "listbox")
  {
    String tempDropDownList = DropDownList;
    String tempDropDownListOpptions  = DropDownListOpptions;
    String TempItems;
    String TempBla;

    Param1 = GetMeThatVar(Param1);

    for (int i = 0; i <= 20; i++)
    {
      tempDropDownListOpptions  = DropDownListOpptions;
      TempBla = getValue(Param1, ',', i);
      TempBla.replace(",", "");
      if (TempBla != "") {

        tempDropDownListOpptions.replace("item",  TempBla);
        TempItems = String( TempItems + tempDropDownListOpptions);
      }
      delay(0);
    }

    Param2 = GetMeThatVar(Param2);

    tempDropDownList.replace("variablenumber",  String(LastVarNumberLookedUp));
    tempDropDownList.replace("options",  TempItems);
    if (Param3.toInt() > 1 | Param0 == "dropdown") Param3 = "1";
    tempDropDownList.replace("theSize",  Param3);

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
    RunningProgram = 0;
    WaitForTheInterpertersResponse = 1;
    PrintAndWebOut("Done...");
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


String DoMathForMe(String cc, String f, String dd )
{
  float e;
  String ee = cc;

  float c = cc.toFloat();
  float d = dd.toFloat();

  f.trim();

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


  if (f == "==")
  {
    ee = String((cc == dd));
  }

  if (f == "=")
  {
    ee = String((c == d));
  }
  return ee;
}



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
    //PrintAndWebOut("file open failed");
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

bool ConnectToTheWIFI(String NetworkName, String NetworkPassword, String NetworkStaticIP, String NetworkGateway, String NetworkSubnet)
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
    Serial.println(numberOfAtempts);
    if (numberOfAtempts >= 12)
    {
      Serial.println("");
      Serial.println("Failed Wifi Connect ");
      return 0;
    }
  }


  if (NetworkStaticIP != "" & NetworkGateway != "" & NetworkSubnet != "" )
  {

    NetworkStaticIP += ".";
    NetworkGateway += ".";
    NetworkSubnet += ".";
    IPAddress ip(     getValue(NetworkStaticIP, '.', 0).toInt(), getValue(NetworkStaticIP, '.', 1).toInt(), getValue(NetworkStaticIP, '.', 2).toInt(), getValue(NetworkStaticIP, '.', 3).toInt());
    IPAddress gateway(getValue(NetworkGateway,  '.', 0).toInt(), getValue(NetworkGateway, '.', 1).toInt(), getValue(NetworkGateway, '.', 2).toInt(), getValue(NetworkGateway, '.', 3).toInt());
    IPAddress subnet( getValue(NetworkSubnet,   '.', 0).toInt(), getValue(NetworkSubnet, '.', 1).toInt(), getValue(NetworkSubnet, '.', 2).toInt(), getValue(NetworkSubnet, '.', 3).toInt());
    WiFi.config(ip, gateway, subnet);
  }

  if (WiFi.localIP().toString().endsWith(".0"))
  {
    CreateAP("", "");
  }
  else
  {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address : ");
    Serial.println(WiFi.localIP());
    SaveDataToFile("WIFIname" ,  NetworkName);
    SaveDataToFile("WIFIpass", NetworkPassword);
  }

  return 1;
}




void CreateAP(String NetworkName, String NetworkPassword)
{
  WiFi.mode(WIFI_AP_STA);
  Serial.println("Creating WIFI access point");


  if (NetworkName == "")
  {
    NetworkName = LoadDataFromFile("APname");
    NetworkPassword = LoadDataFromFile("APpass");

    if (NetworkName == "")
    {
      NetworkName = "ESP";
      NetworkPassword = "";
    }
  }

  Serial.println(NetworkName);


  int str_len = NetworkName.length() + 1;
  char ssid[str_len];
  NetworkName.toCharArray(ssid, str_len);

  str_len = NetworkPassword.length() + 1;
  char password[str_len];
  NetworkPassword.toCharArray(password, str_len);

  WiFi.disconnect();
  delay(2000);
  if (NetworkPassword.length() < 8)
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
  urlChars.replace("+",   " ");
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
  urlChars.replace("%7B", String(char(123)));
  urlChars.replace("%7C", "|");
  urlChars.replace("%7D", String(char(125)));
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

  for (int i = 0; i <= TotalNumberOfLines - 1; i++)
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

  //Serial.println(x);
  if (x != 0)
  {
    for (int i = 0; i <= TotalNumberOfLines - 1; i++) {
      delay(1);

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




//begin all of the i/o code
float UniversalPinIO(String PinCommand, byte pin, float PinValue)
{
  PinListOfStatus[pin] = PinCommand;
  PinListOfStatusValues[pin] = PinValue;
  SetThePinMode(PinCommand, pin);
  if (PinCommand == "po") digitalWrite(pin, PinValue);
  else if (PinCommand == "pi") return digitalRead(pin);
  else if (PinCommand == "pwi") return analogRead(pin);
  else if (PinCommand == "pwo") analogWrite(pin, PinValue);
  else if (PinCommand == "servo") servoWrite(pin, PinValue);
  return 0;
}

void SetThePinMode(String PinCommand, byte pin)
{
  delay(0);

  pinMode(pin, OUTPUT);
  analogWrite(pin, 0);


  if (PinCommand != "servo")
  {
    if (pin == 0)   Servo0.detach();
    if (pin == 1)   Servo1.detach();
    if (pin == 2)   Servo2.detach();
    if (pin == 3)   Servo3.detach();
    if (pin == 4)   Servo4.detach();
    if (pin == 5)   Servo5.detach();
    if (pin == 6)   Servo6.detach();
    if (pin == 7)   Servo7.detach();
    if (pin == 8)   Servo8.detach();
    if (pin == 9)   Servo9.detach();
    if (pin == 10)   Servo10.detach();
    if (pin == 11)   Servo11.detach();
    if (pin == 12)   Servo12.detach();
    if (pin == 13)   Servo13.detach();
    if (pin == 14)   Servo14.detach();
    if (pin == 15)   Servo15.detach();
    if (pin == 16)   Servo16.detach();
  }

  if (PinCommand == "po" | PinCommand == "pwo" ) pinMode(pin, OUTPUT);
  if (PinCommand == "pi" | PinCommand == "pwi" ) pinMode(pin, INPUT);
}

void servoWrite(byte pin, int ValueForIO)
{
  if (pin == 0)
  {
    Servo0.attach(0);
    Servo0.write(ValueForIO);
  }
  if (pin == 1)
  {
    Servo1.attach(1);
    Servo1.write(ValueForIO);
  }
  if (pin == 2)
  {
    Servo2.attach(2);
    Servo2.write(ValueForIO);
  }
  if (pin == 3)
  {
    Servo3.attach(3);
    Servo3.write(ValueForIO);
  }
  if (pin == 4)
  {
    Servo4.attach(4);
    Servo4.write(ValueForIO);
  }
  if (pin == 5)
  {
    Servo5.attach(5);
    Servo5.write(ValueForIO);
  }
  if (pin == 6)
  {
    Servo6.attach(6);
    Servo6.write(ValueForIO);
  }
  if (pin == 7)
  {
    Servo7.attach(7);
    Servo7.write(ValueForIO);
  }
  if (pin == 8)
  {
    Servo8.attach(8);
    Servo8.write(ValueForIO);
  }
  if (pin == 9)
  {
    Servo9.attach(9);
    Servo9.write(ValueForIO);
  }
  if (pin == 10)
  {
    Servo10.attach(10);
    Servo10.write(ValueForIO);
  }
  if (pin == 11)
  {
    Servo11.attach(11);
    Servo11.write(ValueForIO);
  }
  if (pin == 12)
  {
    Servo12.attach(12);
    Servo12.write(ValueForIO);
  }
  if (pin == 13)
  {
    Servo13.attach(13);
    Servo13.write(ValueForIO);
  }
  if (pin == 14)
  {
    Servo14.attach(14);
    Servo14.write(ValueForIO);
  }
  if (pin == 15)
  {
    Servo15.attach(15);
    Servo15.write(ValueForIO);
  }
  if (pin == 16)
  {
    Servo16.attach(16);
    Servo16.write(ValueForIO);
  }
}


//  End all of the I/O code

















String BasicProgram(int LineNumberToLookUp)
{
  if (ProgramName == "")
  {
    ProgramName = "default";
  }
  delay(0);
  return LoadDataFromFile(String(ProgramName  + "/" + String(LineNumberToLookUp)));
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
