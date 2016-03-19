
//Code here adapted from the example
//http://playground.arduino.cc/Code/Email?action=sourceblock&num=2

/*
   Email client sketch for IDE v1.0.5 and w5100/w5200
   Posted 7 May 2015 by SurferTim
*/


byte sendEmail(String EmailTo, String EmailFrom, String EmailSubject, String EmailBody)
{
  byte thisByte = 0;
  byte respCode;

  EmailServer.trim();
  EmailSMTPuser.trim();
  EmailSMTPpassword.trim();


  if (client.connect(EmailServer.c_str(), Emailport) == 1) {
    Serial.println(F("connected"));
  } else {
    Serial.println(F("connection failed"));
    return 0;
  }

  if (!eRcv()) return 0;

  //Serial.println(F("Sending hello"));
  // replace 1.2.3.4 with your Arduino's ip
  client.println(F("EHLO 1.2.3.4"));
  if (!eRcv()) return 0;

  //Serial.println(F("Sending auth login"));
  client.println(F("auth login"));
  if (!eRcv()) return 0;

  //Serial.println(F("Sending User"));
  // Change to your base64 encoded user
  client.println(GetMeBase64(EmailSMTPuser));

  if (!eRcv()) return 0;

  //Serial.println(F("Sending Password"));
  // change to your base64 encoded password
  client.println(GetMeBase64(EmailSMTPpassword));





  if (!eRcv()) return 0;

  // change to your email address (sender)
  //Serial.println(F("Sending From"));
  client.println(String(F("MAIL From: <")) +  EmailFrom + ">");
  if (!eRcv()) return 0;

  // change to recipient address
  //Serial.println(F("Sending To"));
  client.println(String(F("RCPT To: <")) + EmailTo + ">");
  if (!eRcv()) return 0;

  //Serial.println(F("Sending DATA"));
  client.println(F("DATA"));
  if (!eRcv()) return 0;

  //Serial.println(F("Sending email"));

  // change to recipient address
  client.println(String(F("To: You <")) + EmailTo + ">");

  // change to your address
  client.println(String(F("From: Me <")) +  EmailFrom + ">");

  client.println(String(F("Subject: ")) + EmailSubject + "\r\n");

  client.println(EmailBody);

  client.println(".");

  if (!eRcv()) return 0;

  //Serial.println(F("Sending QUIT"));
  client.println(F("QUIT"));
  if (!eRcv()) return 0;

  client.stop();

  //Serial.println(F("disconnected"));

  return 1;
}

byte eRcv()
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;

  while (!client.available()) {
    delay(1);
    loopCount++;

    // if nothing received for 10 seconds, timeout
    if (loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  respCode = client.peek();

  while (client.available())
  {
    thisByte = client.read();
    Serial.write(thisByte);
  }

  if (respCode >= '4')
  {
    byte thisByte = 0;
    int loopCount = 0;

    client.println(F("QUIT"));

    while (!client.available()) {
      delay(1);
      loopCount++;

      // if nothing received for 10 seconds, timeout
      if (loopCount > 10000) {
        client.stop();
        Serial.println(F("\r\nTimeout"));
        return 0;
      }
    }

    while (client.available())
    {
      thisByte = client.read();
      Serial.write(thisByte);
    }

    client.stop();

    Serial.println(F("disconnected"));
    return 0;
  }

  return 1;
}


String GetMeBase64(String StringToConvert)
{
  int str_len = StringToConvert.length() + 1;
  char input[str_len];
  StringToConvert.toCharArray(input, str_len);
  int inputLen = sizeof(input) - 1;

  int encodedLen = base64_enc_len(inputLen);
  char encoded[encodedLen];

  base64_encode(encoded, input, inputLen);

  return encoded;
}

