//wifi code here

bool ConnectToTheWIFI(String NetworkName, String NetworkPassword, String NetworkStaticIP, String NetworkGateway, String NetworkSubnet)
{
  if (wifiApStaModeOn == 0) WiFi.mode(WIFI_STA);
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
      Serial.println(F("Failed Wifi Connect "));
      return 0;
    }
  }

delay(1000);

  if (NetworkStaticIP != "" & NetworkGateway != "" & NetworkSubnet != "" )
  {

    //NetworkStaticIP += ".";
    //NetworkGateway += ".";
    //NetworkSubnet += ".";
    //IPAddress ip(     getValue(NetworkStaticIP, '.', 0).toInt(), getValue(NetworkStaticIP, '.', 1).toInt(), getValue(NetworkStaticIP, '.', 2).toInt(), getValue(NetworkStaticIP, '.', 3).toInt());
    //IPAddress gateway(getValue(NetworkGateway,  '.', 0).toInt(), getValue(NetworkGateway, '.', 1).toInt(), getValue(NetworkGateway, '.', 2).toInt(), getValue(NetworkGateway, '.', 3).toInt());
    //IPAddress subnet( getValue(NetworkSubnet,   '.', 0).toInt(), getValue(NetworkSubnet, '.', 1).toInt(), getValue(NetworkSubnet, '.', 2).toInt(), getValue(NetworkSubnet, '.', 3).toInt());
    IPAddress ip = StringToIP(NetworkStaticIP);
    IPAddress gateway = StringToIP(NetworkGateway);
    IPAddress subnet = StringToIP(NetworkSubnet);
    WiFi.config(ip, gateway, subnet);
  }

delay(1000);

//  if (WiFi.localIP().toString().endsWith(".0"))
//  {
//    Serial.println(WiFi.localIP());
//    CreateAP("", "");
//    return 0;
//  }
//  else
  {
    //configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    configTime(LoadDataFromFile("TimeZone").toFloat() * 3600, LoadDataFromFile("DaylightSavings").toInt(), "pool.ntp.org", "time.nist.gov");
    Serial.println("");
    Serial.print(F("Connected to "));
    Serial.println(ssid);
    Serial.print(F("IP address : "));
    Serial.println(WiFi.localIP());
    SaveDataToFile("WIFIname" ,  NetworkName);
    SaveDataToFile("WIFIpass", NetworkPassword);
    return 1;
  }


}




void CreateAP(String NetworkName, String NetworkPassword, String NetworkStaticIP, String NetworkGateway, String NetworkSubnet)
{
  if (wifiApStaModeOn == 0)WiFi.mode(WIFI_AP);
  Serial.println(F("Creating WIFI access point"));


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

  if (NetworkStaticIP != "" & NetworkGateway != "" & NetworkSubnet != "" )
  {
    IPAddress ip = StringToIP(NetworkStaticIP);
    IPAddress gateway = StringToIP(NetworkGateway);
    IPAddress subnet = StringToIP(NetworkSubnet);
    WiFi.softAPConfig(ip, gateway, subnet);  // this is the right command to change IP for STA mode
    Serial.print(F("Station IP:"));
    Serial.println(WiFi.softAPIP().toString());
  }
  
  SaveDataToFile("APname" ,  NetworkName);
  SaveDataToFile("APpass", NetworkPassword);
}

