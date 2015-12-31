//begin all of the i/o code
float UniversalPinIO(String PinCommand, String PinDesignaor, float PinValue)
{
  byte pin = PinDesignaor.toInt();
  PinDesignaor.toUpperCase();

  PinDesignaor.replace(".00", "");

  //PIN DESIGNATIONS FOR NODE MCU
  if (PinDesignaor == "D0")  pin = 16;
  if (PinDesignaor == "D1")  pin = 5;
  if (PinDesignaor == "D2")  pin = 4;
  if (PinDesignaor == "D3")  pin = 0;
  if (PinDesignaor == "D4")  pin = 2;
  if (PinDesignaor == "D5")  pin = 14;
  if (PinDesignaor == "D6")  pin = 12;
  if (PinDesignaor == "D7")  pin = 13;
  if (PinDesignaor == "D8")  pin = 15;
  if (PinDesignaor == "RX")  pin = 3;
  if (PinDesignaor == "TX")  pin = 1;


  if (PinCommand != "po" & PinCommand != "pi" & PinCommand != "pwi" & PinCommand != "pwo"& PinCommand != "servo" & PinCommand != "")
  {
    PinListOfStatus[pin] = PinCommand;
    PinCommand = "pi";
  }
  else
  {
    if (PinCommand == "pi")
    {
      if ((PinListOfStatus[pin] == "po") | ( PinListOfStatus[pin] == "pi") | (PinListOfStatus[pin] == "pwi") | (PinListOfStatus[pin] == "pwo")  | (PinListOfStatus[pin] == "servo") | ( PinListOfStatus[pin] == ""))
      {
        PinListOfStatus[pin] = PinCommand;
      }

    }
    else
    {
      PinListOfStatus[pin] = PinCommand;
    }
  }




  PinListOfStatusValues[pin] = PinValue;
  SetThePinMode(PinCommand, pin);
  if (PinCommand == "po") digitalWrite(pin, PinValue);
  else if (PinCommand == "pi") {
    PinListOfStatusValues[pin] = digitalRead(pin);
    return PinListOfStatusValues[pin];
  }
  else if (PinCommand == "pwi") {
    PinListOfStatusValues[pin] = analogRead(pin);
    return PinListOfStatusValues[pin] ;
  }
  else if (PinCommand == "pwo") analogWrite(pin, PinValue);
  else if (PinCommand == "servo") servoWrite(pin, PinValue);
  else if (PinCommand == "ai") return analogRead(A0);
  else if (PinCommand == "interrupt") return pin;
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
    //    if (pin == 6)   Servo6.detach();
    //    if (pin == 7)   Servo7.detach();
    //    if (pin == 8)   Servo8.detach();
    //    if (pin == 9)   Servo9.detach();
    //    if (pin == 10)   Servo10.detach();
    //    if (pin == 11)   Servo11.detach();
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
  //  if (pin == 6)
  //  {
  //    Servo6.attach(6);
  //    Servo6.write(ValueForIO);
  //  }
  //  if (pin == 7)
  //  {
  //    Servo7.attach(7);
  //    Servo7.write(ValueForIO);
  //  }
  //  if (pin == 8)
  //  {
  //    Servo8.attach(8);
  //    Servo8.write(ValueForIO);
  //  }
  //  if (pin == 9)
  //  {
  //    Servo9.attach(9);
  //    Servo9.write(ValueForIO);
  //  }
  //  if (pin == 10)
  //  {
  //    Servo10.attach(10);
  //    Servo10.write(ValueForIO);
  //  }
  //  if (pin == 11)
  //  {
  //    Servo11.attach(11);
  //    Servo11.write(ValueForIO);
  //  }
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








