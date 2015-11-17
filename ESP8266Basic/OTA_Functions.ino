//const char* host = "raw.githubusercontent.com";
//const int httpsPort = 443;
//
//// Use web browser to view and copy
//// SHA1 fingerprint of the certificate
//const char* fingerprint = "B0 74 BB EF 10 C2 DD 70 89 C8 EA 58 A2 F9 E1 41 00 D3 38 82";
//
//const char* url = "/esp8266/Basic/master/Flasher/Build/4M/ESP8266Basic.cpp.bin";
//
//
//
//
//bool BasicOTAupgrade()
//{
//  configTime(3 * 3600, 0, "pool.ntp.org");
//
//  // Use WiFiClientSecure class to create TLS connection
//  WiFiClientSecure client;
//  Serial.print("connecting to ");
//  Serial.println(host);
//  if (!client.connect(host, httpsPort)) {
//    Serial.println("connection failed");
//    return 0;
//  }
//
//  if (client.verify(fingerprint, host)) {
//    Serial.println("certificate matches");
//  } else {
//    Serial.println("certificate doesn't match");
//    return 0;
//  }
//
//  Serial.print("Starting OTA from: ");
//  Serial.println(url);
//
//  auto ret = ESPhttpUpdate.update(client, host, url);
//  // if successful, ESP will restart
//  Serial.println("update failed");
//  Serial.println((int) ret);
//}
