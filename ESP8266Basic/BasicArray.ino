const int TotalNumberOfArrays = 50;

const char PROGMEM SUBSCR_OUT[] = "arrays: subscript out of range";
class basicArray
{
  private:
  char *_name;
  // this permit to share (and simplify) the handling of the memory allocated to the buffers
  // in fact the Strings or the floats will use the same memory pointer
  union 
  {
      void *base = NULL;
      String **str;
      float *flo;
  } ptr;

  
  public:
  byte Format = PARSER_FALSE;
  uint16_t nbElements = 0;
  
  int dim(String Name, int elements, int format)
  {
    Format = format;
    nbElements = elements;    
    //remove();  // in case of redim, the original is deleted
    _name = (char *) malloc(VariablesNameLength + 1);  // allocate a buffer for 11 chars (10 + \0)
    strncpy(_name, Name.c_str(), VariablesNameLength + 1);    
    if (format == PARSER_TRUE)
    {
      ptr.base = malloc(sizeof(float) * (nbElements + 1));
    }
    else
      if (format == PARSER_STRING)
      {
        ptr.base = malloc(sizeof(String) * (nbElements + 1));
        for (int i = 0; i<=nbElements; i++)
          ptr.str[i] = NULL;
      }
      else
      {
         SendErrorMsg(F("DIM() : format not valid"));
      }
  } 

  String getName()
  {
    if (_name != NULL)
      return String(_name);
    else
      return String("");
  } 

  float getFloat(int index)
  {
    if (ptr.base == NULL) return 0;
    if (index > nbElements)
      {   SendErrorMsg(String(SUBSCR_OUT));  return 0; } 
    return ptr.flo[index];
  }

  String getString(int index)
  {
    if (ptr.base == NULL) return "";
    if (index > nbElements)
      {   SendErrorMsg(String(SUBSCR_OUT));  return ""; } 
    if ( (Format == PARSER_STRING) && (ptr.str[index] != NULL) )
       return *ptr.str[index];
    else
      return "";
  }   

  void setFloat(int index, float val)
  {
    if (index > nbElements)
      {   SendErrorMsg(String(SUBSCR_OUT));  return; } 
    ptr.flo[index] = val;
  }    

  void setString(int index, String val)
  {
    if (index > nbElements)
      {   SendErrorMsg(String(SUBSCR_OUT));  return; } 
    if (ptr.str[index] == NULL)
      ptr.str[index] = new String(val);
    else
      *ptr.str[index] = val;
  } 
    
  void remove()
  {
    if (Format == PARSER_STRING)
    {
      for (int i = 0; i<=nbElements; i++)
      {
        if (ptr.str[i] != NULL)
          delete ptr.str[i];
      }
    }
    free(ptr.base);
    free(_name);
    Format = PARSER_FALSE;
    nbElements = 0;   
    ptr.base = NULL;
    _name = NULL;
  }
  
} basic_arrays[TotalNumberOfArrays];


int Search_Array(String Name)
{
  for (int i=0; i<TotalNumberOfArrays; i++)
  {
    if (basic_arrays[i].getName() == Name)
    {
      return i;
    }
  }
  return -1;
}

int Search_First_Available_Array()
{
  for (int i=0; i<TotalNumberOfArrays; i++)
  {
    if ( basic_arrays[i].Format == PARSER_FALSE)
    {
      return i;
    }
  }
  return -1;
}

