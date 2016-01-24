String ParseJSONValue(String data, String JSONfield, String JSONindex)
{
  //JSONfield = "time";
 // data = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
  data.replace(String(char(92)), "");
  data.replace(String(char(34)), "");



  data = String(data + "           ");
  int maxIndex = data.length() - 1;
  int j = 0;
  byte WaitingForQuote;
  String chunkVal = "";
  String ChunkReturnVal;
  for (int i = 0; i <= maxIndex ; i++)
  {
    if (data[i] == char(123) )
    {
      i++;
      while (i <= maxIndex && data[i] != char(125) ) {
        chunkVal.concat(data[i]);
        i++;
        delay(0);
      }
    }
  }

  data = chunkVal;
  chunkVal = "";
Serial.println(data);

  for (int i = 0; i <= maxIndex / 3; i++)
  {
    delay(0);
    chunkVal = getValue(data, ',', i);
    chunkVal.trim();
    if (chunkVal.startsWith(String(JSONfield + ":")))
    {
      chunkVal.remove(0, JSONfield.length() + 1);
      Serial.println(chunkVal);

    }
  }


}
