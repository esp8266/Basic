String GetRidOfurlCharacters(String urlChars)
{

  int i;
  char c;
  const char *s;
  String result = "";
  for (i=0; i<urlChars.length(); i++)
  {
    if (urlChars[i] == '%')  // if the char found is '%'
    {
      // start of encoded char
      //take the 2 chars following the % in hexadecimal and convert to char
      //if the value is 0, jumps considering that the % shall be left as is
      s = urlChars.substring(i+1,i+3).c_str(); // get the 'C' format of the string; this is required by strtol
      c = strtol(s,NULL,16);  // convert the 2-char string from Hex to char
      if (c != 0)  // is not '0' ?
      {
        result.concat(c);   // add the char to the result
        i+=2;
      }
      else
      {
        result.concat(urlChars[i]);  // else take the original char
      }
      
    }
    else
    {
     result.concat(urlChars[i]);  // else take the original char
    }
    
  }

  return result;
  
}
