String Parsifal(String data, String param)
{
  int i, last;
  String tmp;
  int st, en;
  int en1, en2, en3;
  int pos;
  String ReturnVal;
  // extract the pieces of the param separated by '.'
  st = 0;
  pos = 0;
  last = 0;
  for (i=0; i<5; i++)
  {
      en = param.indexOf('.', st);
      if (en == -1)
      {
         en = param.length();
         last = 1;
      }
      tmp = "\"" + param.substring(st, en) +"\":";
//      Serial.println(tmp);
      st = en+1;
      pos = data.indexOf(tmp, pos+1);
//      Serial.println(pos);
      if ((pos == -1) || (last == 1)) break;

  }
  if (pos != -1)
  {
      pos = pos + tmp.length();
      en = 10000;
      en1 = data.indexOf('"', pos+1);
      en2 = data.indexOf('}', pos+1);
      en3 = data.indexOf(']', pos+1);
      if ((en1 > 0) && (en1 < en)) en = en1;
      if ((en2 > 0) && (en2 < en)) en = en2;
      if ((en3 > 0) && (en3 < en)) en = en3;
//      Serial.print("start");
//      Serial.println(pos);
//      Serial.print("end");
//      Serial.println(en);
      if (data[pos] == '"')
        pos++;
      ReturnVal = data.substring(pos, en);
      return ReturnVal;
//      return ("forse");
  }

  return F("not found");
}

