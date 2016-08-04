String RunningProgramGui()
{
	if (WebGuiOff == 2)
	{
		HTMLout = "";
		return "GUI OFF";
	}
	if (WebGuiOff == 1)
	{
		HTMLout = "";
		return "GUI OFF";
	}

  if (refreshBranch != "")
  {

    inData = String(" goto " + refreshBranch + " ");
    WaitForTheInterpertersResponse = 0;
    ExicuteTheCurrentLine();
    runTillWaitPart2();
    RunBasicTillWait();
  }
 String WebOut;
 if (BasicDebuggingOn == 1) WebOut = String(MobileFreindlyWidth) + String(DebugPage)  + HTMLout + String(F("</div>"));
 if (BasicDebuggingOn == 0) WebOut = String(MobileFreindlyWidth) + String(F("<script src='WebSockets.js'></script><script src='/file?file=widgets.js.gz'></script>"))  + HTMLout;

  for (int i = TotalNumberOfVariables - 1; i >= 0; i--)
  {
    delay(0);
    WebOut.replace(String(F("VARS|")) + String(i), AllMyVariables[i].getVar());
  }
  
  
  WebOut.replace(F("**graphics**"), F("<iframe src='graphics.htm' style='border:none' name='gra' width='*gwid*' height='*ghei*' scrolling='no'></iframe>"));
  WebOut.replace(F("*gwid*"),  String(GraphicsEliments[0][1]));
  WebOut.replace(F("*ghei*"),  String(GraphicsEliments[0][2]));
  
  return WebOut;
}


void SendErrorMsg(String BasicErrorMsg)
{
PrintAndWebOut(	String(F("Error at line ")) + String(RunningProgramCurrentLine) + String(F(": ")) + BasicErrorMsg);
}


void PrintAndWebOut(String itemToBePrinted)
{
  delay(0);
  Serial.println(itemToBePrinted);
  WebSocketSend( "print~^`" + itemToBePrinted);
  itemToBePrinted.replace(' ' , char(160));
  if ( WebGuiOff == 1 ){return;}
  if ( WebGuiOff == 2 ){HTMLout = "";return;}
  if (HTMLout.length() < 4000)
    HTMLout = String(HTMLout + "<hr>" + itemToBePrinted);
  else
  {
    HTMLout = String(HTMLout + String(F("<hr> BUFFER TOO BIG! PROGRAM STOPPED")));
    Serial.println(F("BUFFER TOO BIG! PROGRAM STOPPED"));
    RunningProgram = 0;
  }
  return;
}


void AddToWebOut(String itemToBePrinted)
{
  delay(0);
  if ( WebGuiOff == 1 ){return;}
  if ( WebGuiOff == 2 ){HTMLout = "";return;}
  WebSocketSend( "wprint~^`" + itemToBePrinted);
  //itemToBePrinted.replace(' ' , char(160));
  if (HTMLout.length() < 4000)
    HTMLout = String(HTMLout + itemToBePrinted);
  else
  {
    HTMLout = String(HTMLout + String(F("<hr> BUFFER TOO BIG! PROGRAM STOPPED")));
    RunningProgram = 0;
  }
  return;
}


void SendAllTheVars()
{
  for (int i = 0; i < TotalNumberOfVariables; i++)
  {
    if (AllMyVariables[i].getName() == "") break;
	if (BasicDebuggingOn == 1)  WebSocketSend( "varname~^`" + String(i) + "~^`" + String(AllMyVariables[i].getName()));
    WebSocketSend( "var~^`" + String(i) + "~^`" + String(AllMyVariables[i].getVar()));
    delay(0);
    //Serial.println(i);
  }
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

    if (GraphicsEliments[i][0] == 5) //For rectangle
    {
      GraphicsEliment = GraphicsTextCode;
    }	
	
	


    GraphicsEliment.replace(F("*x1*"),  String(GraphicsEliments[i][1]));
    GraphicsEliment.replace(F("*y1*"),  String(GraphicsEliments[i][2]));
    GraphicsEliment.replace(F("*x2*"),  String(GraphicsEliments[i][3]));
    GraphicsEliment.replace(F("*y2*"),  String(GraphicsEliments[i][4]));
	GraphicsEliment.replace(F("*text*"),  GraphicsText[i]);

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





String GenerateIDtag(String TempateString)
{
  LastElimentIdTag = String("I" + String(millis()));
  TempateString.replace(F("myid"), LastElimentIdTag );
  return TempateString;
}




String RequestWebSocket(String Request)
{
  WebSocketSend(  Request);
  WebSockMessage = "";
  for (int i = 0; ((i < 5) && (WebSockMessage == "")); i++) // wait for the answer
  {
    webSocket.loop();
    delay(100);
  }
  return WebSockMessage;
}


void WebSocketSend(String MessageToSend)
{
	if ( WebGuiOff == 1 )return;
	for (byte i = 0; i <= 5; i++)
	{
		if (WebSocketTimeOut[i] + 20000 >=  millis())
		{
		 webSocket.sendTXT(i, MessageToSend);
		 delaytime = 35 + millis();
		  webSocket.loop();
		}
	}
}






void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght)
{
  switch (type)
  {
    case WStype_DISCONNECTED:
	WebSocketTimeOut[num]  = 0;
      Serial.print(num);
      Serial.println(" winsock Disconnected!");
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.print(num);
        Serial.print(" winsock connected ");
        Serial.println(ip.toString());
        // send message to client
        WebSocketSend( "Connected");
		if (BasicDebuggingOn == 1) WebSocketSend( "code~^`" + String(RunningProgramCurrentLine));
		SendAllTheVars();
      }
      break;
    case WStype_TEXT:
      if (lenght == 0)
      {
        WebSockMessage == "";
        break;
      }
      WebSockMessage = String((char*)payload);
      if (WebSockMessage == "OK") {
        WebSocketTimeOut[num] = millis();
        break;
      }
      // Serial.print(num);
      // Serial.print(" get text ");

      //Serial.println(WebSockMessage);

      if (WebSockMessage == F("cmd:stop"))
      {
        HaltBasic("");
        break;
      }
      if (WebSockMessage == F("cmd:run"))
      {
        String WebOut;
        RunningProgram = 1;
        RunningProgramCurrentLine = 0;
        WaitForTheInterpertersResponse = 0 ;
        numberButtonInUse = 0;
        HTMLout = "";
        TimerWaitTime = 0;
        TimerCBtime = 0;
        GraphicsEliments[0][0] = 0;
        WebOut = F(R"=====(  <meta http-equiv="refresh" content="0; url=./input?" />)=====");

        clear_stacks();
        server->send(200, "text/html", WebOut);
        break;
      }
      if (WebSockMessage == F("cmd:pause"))
      {
        RunningProgram = 0;
		//Serial.println("problem here 3");
        //WaitForTheInterpertersResponse = 1;
        break;
      }
      if (WebSockMessage == F("cmd:continue"))
      {
        RunningProgram = 1;
        //WaitForTheInterpertersResponse = 0;
        break;
      }


      if (WebSockMessage.startsWith(F("guievent:")))
      {
        //Serial.println(WebSockMessage.substring(9));
        break;
      }
      if (WebSockMessage == F("vars"))
      {
        SendAllTheVars();
        break;
      }
      if (WebSockMessage.startsWith(F("guicmd:")))
      {
        //Serial.println(WebSockMessage.substring(7));
        RunningProgram = 1;
        WaitForTheInterpertersResponse = 0;
        RunningProgramCurrentLine = WebSockMessage.substring(7).toInt() - 1;
        //Serial.println("Current line = " + String(RunningProgramCurrentLine));
        runTillWaitPart2();
        break;
      }
      if (WebSockMessage.startsWith(F("guichange~")))
      {
        //Serial.println(getValue(WebSockMessage, '~', 1).toInt());
        //Serial.println(getValue(String(WebSockMessage), '~', 2));
        AllMyVariables[getValue(WebSockMessage, '~', 1).toInt()].setVar(getValue(WebSockMessage, '~', 2));
        WebSocketSend( "var~^`" + String(getValue(WebSockMessage, '~', 1).toInt()) + "~^`" + String(AllMyVariables[getValue(WebSockMessage, '~', 1).toInt()].getVar()));
        break;
      }



      if (WebSockMessage.startsWith("event:"))
      {
        if (WebSockEventBranchLine > 0)
        {
          WebSockEventName = WebSockMessage.substring(6);
          // if the program is in wait, it returns to the previous line to wait again
          return_Stack.push(RunningProgramCurrentLine - WaitForTheInterpertersResponse); // push the current position in the return stack
          WaitForTheInterpertersResponse = 0;   //exit from the wait state but comes back again after the gosub
          RunningProgramCurrentLine = WebSockEventBranchLine + 1; // gosub after the WebSockEventBranch label
          WebSockEventBranchLine = - WebSockEventBranchLine; // this is to avoid to go again inside the branch; it will be restored back by the return command
        }
        WebSocketSend(  "_");
        break;
      }

      if (WebSockMessage.startsWith("change:"))
      {
        if (WebSockChangeBranchLine > 0)
        {
          WebSockChangeName = WebSockMessage.substring(7);
          // if the program is in wait, it returns to the previous line to wait again
          return_Stack.push(RunningProgramCurrentLine - WaitForTheInterpertersResponse); // push the current position in the return stack
          WaitForTheInterpertersResponse = 0;   //exit from the wait state but comes back again after the gosub
          RunningProgramCurrentLine = WebSockChangeBranchLine + 1; // gosub after the WebSockChangeBranch label
          WebSockChangeBranchLine = - WebSockChangeBranchLine; // this is to avoid to go again inside the branch; it will be restored back by the return command
        }
        WebSocketSend( "_");
        break;
      }
      break;
  }
}

