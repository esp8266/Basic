
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


  if (EmailClient.connect(EmailServer.c_str(), Emailport) == 1) {
    Serial.println(F("connected"));
  } else {
    Serial.println(F("connection failed"));
    return 0;
  }

  if (!eRcv()) return 0;

  Serial.println(F("Sending hello"));
  // replace 1.2.3.4 with your Arduino's ip
  EmailClient.println("EHLO 1.2.3.4");
  if (!eRcv()) return 0;

  Serial.println(F("Sending auth login"));
  EmailClient.println("auth login");
  if (!eRcv()) return 0;

  Serial.println(F("Sending User"));
  // Change to your base64 encoded user
  EmailClient.println(GetMeBase64(EmailSMTPuser));

  if (!eRcv()) return 0;

  Serial.println(F("Sending Password"));
  // change to your base64 encoded password
  EmailClient.println(GetMeBase64(EmailSMTPpassword));





  if (!eRcv()) return 0;

  // change to your email address (sender)
  Serial.println(F("Sending From"));
  EmailClient.println(String("MAIL From: <" +  EmailFrom + ">"));
  if (!eRcv()) return 0;

  // change to recipient address
  Serial.println(F("Sending To"));
  EmailClient.println(String("RCPT To: <" + EmailTo + ">"));
  if (!eRcv()) return 0;

  Serial.println(F("Sending DATA"));
  EmailClient.println("DATA");
  if (!eRcv()) return 0;

  Serial.println(F("Sending email"));

  // change to recipient address
  EmailClient.println(String("To: You <" + EmailTo + ">"));

  // change to your address
  EmailClient.println(String("From: Me <" +  EmailFrom + ">"));

  EmailClient.println(String("Subject: " + EmailSubject + "\r\n"));

  EmailClient.println(EmailBody);

  EmailClient.println(".");

  if (!eRcv()) return 0;

  Serial.println(F("Sending QUIT"));
  EmailClient.println("QUIT");
  if (!eRcv()) return 0;

  EmailClient.stop();

  Serial.println(F("disconnected"));

  return 1;
}

byte eRcv()
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;

  while (!EmailClient.available()) {
    delay(1);
    loopCount++;

    // if nothing received for 10 seconds, timeout
    if (loopCount > 10000) {
      EmailClient.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  respCode = EmailClient.peek();

  while (EmailClient.available())
  {
    thisByte = EmailClient.read();
    Serial.write(thisByte);
  }

  if (respCode >= '4')
  {
    byte thisByte = 0;
    int loopCount = 0;

    EmailClient.println(F("QUIT"));

    while (!EmailClient.available()) {
      delay(1);
      loopCount++;

      // if nothing received for 10 seconds, timeout
      if (loopCount > 10000) {
        EmailClient.stop();
        Serial.println(F("\r\nTimeout"));
        return 0;
      }
    }

    while (EmailClient.available())
    {
      thisByte = EmailClient.read();
      Serial.write(thisByte);
    }

    EmailClient.stop();

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

