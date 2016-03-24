String Parsifal(String data, String param)
{
  int i, last,idx;
  String tmp;
  int st, en;
  int en1;
  int pos;
  String ReturnVal;

  // update cicciocb 02/03/2016
  // it's now possible to specify arguments in square brackets as, for example "weather[5].description"
  // 
  // to do that, we simply multiply the arguments with square brackes :
  // weather[3].temp[2].id -> weather.weather.weather.temp.temp.id
  // the arguments shall be greather than 0 (starts from 1)
  
  while (param.indexOf('[') != -1)
  {
      // look for '['
      st = param.indexOf('[');
      en = param.indexOf(']', st);
      if ((st != -1) && (en != -1))
      {
          // valid brackets found
          // look now for the start of the argument
          // it can start from the '.' or from the beginning of the string
          pos = param.lastIndexOf('.', st);
          if (pos != -1) // '.'  found; starts from that position until the '['
          {
              en1 = pos+1;
          }
          else  //'.' not found; starts from the beginning of the string until the '['
          {
              en1 = 0;
          }
  
          tmp = param.substring(en1, st);
  
          // get the number inside the brackets
          idx = 0;
          idx = param.substring(st + 1, en).toInt();
  
          if (idx == 0) // it the number is 0 or a bracket is badly placed
            return F("invalid brackets");
          
          String s = "";
          for (i = 0; i < idx; i++)
          {
              if (i == 0)
                  s = tmp;
              else
                  s += "." + tmp;
          }
          tmp = param.substring(en1, en +1);
          param.replace(tmp,s);
      }
      else
      {     
          if ((st != -1) || (en != -1)) // if only one bracket is present
          {
              return F("invalid brackets");
          }
      }
  }

//Serial.println(param);
  
  // extract the pieces of the param separated by '.'
  st = 0;
  pos = 0;
  last = 0;
  for (i=0; i<100; i++)   // limits at max 100 items
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
      for (en = pos + 1; en < data.length(); en++)
      {
        if (data[en] == '"' || data[en] == '}' || data[en] == ']' ||data[en] == ',' )   // the value terminate with " or } or ] or ,
          break;
      }

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

