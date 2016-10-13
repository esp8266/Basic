//begin all of the i/o code
float UniversalPinIO(String PinCommand, String PinDesignaor, float PinValue)
{
  byte pin = PinDesignaor.toInt();

  // Let's only do NodeMCU pin translation if pin value is 0 (meaning PinDesignaor is probably a string value).  We'll check that in a moment.
  if (pin==0) {   
    PinDesignaor.replace(F(".00"), "");
    if (PinDesignaor != F("0")) {   //  If PinDesignator is "0", pin is already 0 and we can skip the next 11 if statements and toUpper() function.

      // Usually this block is bypassed because NodeMCU pin translation is done as converted constants in EVAL.INO. 
      // This translation happens when a NodeMCU pin designation (eg. "D7") is passed as a string variable instead of a constant.  Rare.
      PinDesignaor.toUpperCase();
    
      //PIN DESIGNATIONS FOR NODE MCU
      if (PinDesignaor == F("D0"))  pin = 16;
      if (PinDesignaor == F("D1"))  pin = 5;
      if (PinDesignaor == F("D2"))  pin = 4;
      if (PinDesignaor == F("D3"))  pin = 0;
      if (PinDesignaor == F("D4"))  pin = 2;
      if (PinDesignaor == F("D5"))  pin = 14;
      if (PinDesignaor == F("D6"))  pin = 12;
      if (PinDesignaor == F("D7"))  pin = 13;
      if (PinDesignaor == F("D8"))  pin = 15;
      if (PinDesignaor == F("RX"))  pin = 3;
      if (PinDesignaor == F("TX"))  pin = 1;
    }
  }

  if (PinCommand == F("ai")) return analogRead(A0);
  if (PinCommand == F("laststat"))
  {
    return PinListOfStatusValues[pin];
  }

  if (PinCommand != F("po") & PinCommand != F("pi") & PinCommand != F("pwi") & PinCommand != F("pwo") & PinCommand != F("servo") & PinCommand != "")
  {
	  
    PinListOfStatus[pin] = PinCommand;
    PinCommand = F("pi");
	SetThePinMode(PinCommand, pin);
  }
  else
  {
    if (PinCommand == F("pi"))
    {
      if ((PinListOfStatus[pin] == F("po")) | ( PinListOfStatus[pin] == F("pi")) | (PinListOfStatus[pin] == F("pwi")) | (PinListOfStatus[pin] == F("pwo"))  | (PinListOfStatus[pin] == F("servo")) | ( PinListOfStatus[pin] == ""))
      {
		  SetThePinMode(PinCommand, pin);
        PinListOfStatus[pin] = PinCommand;
      }

    }
    else
    {
		SetThePinMode(PinCommand, pin);
      PinListOfStatus[pin] = PinCommand;
    }
  }




  PinListOfStatusValues[pin] = PinValue;
  
  if (PinCommand == F("po")) digitalWrite(pin, PinValue);
  else if (PinCommand == F("pi")) {
    PinListOfStatusValues[pin] = digitalRead(pin);
    return PinListOfStatusValues[pin];
  }
  else if (PinCommand == F("pwi")) {
    PinListOfStatusValues[pin] = analogRead(pin);
    return PinListOfStatusValues[pin] ;
  }
  else if (PinCommand == F("pwo")) analogWrite(pin, PinValue);
  else if (PinCommand == F("servo")) servoWrite(pin, PinValue);
  else if (PinCommand == F("ai")) return analogRead(A0);
  else if (PinCommand == F("interrupt")) return pin;
  return 0;
}

void SetThePinMode(String PinCommand, byte pin)
{
  if (PinListOfStatus[pin] == PinCommand) return;
  if (PinListOfStatus[pin].startsWith("[") & PinCommand == "pi") return;
  
  delay(0);

  pinMode(pin, OUTPUT);
  analogWrite(pin, 0);


  if (PinCommand != F("servo")) servos[ ((pin>5) ? pin-6 : pin) ].detach();   // Shifts any high range pins down to the servo[6] to servo[10] positions.

  if (PinCommand == F("po") | PinCommand == F("pwo") ) pinMode(pin, OUTPUT);
  if (PinCommand == F("pi") | PinCommand == F("pwi") ) pinMode(pin, INPUT);
}

void servoWrite(byte pin, int ValueForIO)
{
  byte controller;
  controller = (pin>5) ? pin-6 : pin;   // Map any high range pins (12-16) to servo objects 6-10.
  servos[controller].attach(pin);
  servos[controller].write(ValueForIO);
}


//  End all of the I/O code


