class basicVariable
{
  private:
  char *_name = NULL;
  String *_Text = NULL;
  float _Number;
  
  public:
  byte Format = 0;

  String getName()
  {
    if (_name != NULL)
      return String(_name);
    else
      return String("");
  }

  void setName(String n)    // the name is max 'VariablesNameLength' chars long
  {
    if (_name == NULL)
    {
      _name = (char *) malloc(VariablesNameLength + 1);  // allocate a buffer for 11 chars (10 + \0)
    }
    strncpy(_name, n.c_str(), VariablesNameLength + 1);
  }

  String getVar()
  {
    if (_Text != NULL)
      return *_Text;
    else
      return String("");
  }

  void setVar(String v)
  {
    if (_Text == NULL)
      _Text = new String();

    *_Text = v;
  }
  void remove()
  {
    delete _Text;
    free(_name);
    _Text = NULL;
    _name = NULL;
  }
  
} AllMyVariables[TotalNumberOfVariables];

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This class manage to index all the 'if' 'then' 'else' '{endif | end if}' lines
PROGMEM const char _ERR4[] = "Label already defined ";
class JUMPLIST
{
  private:
  struct hop
  {
    char _label[VariablesNameLength + 1];
    uint16_t _pos;
  };
  uint16_t _length = 0;

  hop **list = (hop **) malloc( sizeof(hop **));
  
  public:
  void add(String label, uint16_t pos)
  {
    if ( getPos(label) != -1)
    {
      PrintAndWebOut(String(_ERR4) + label);
      return;
    }
    list[_length] = new hop;
    strncpy(list[_length]->_label, label.c_str(), VariablesNameLength + 1);
    list[_length]->_pos = pos;
    _length++;
  }

  int getPos(String label)
  {
    uint16_t i;
    for (i=0; i<_length; i++)
    {
      if (strncmp(list[i]->_label, label.c_str(), VariablesNameLength + 1) == 0)
        return list[i]->_pos;
    }
    return -1;
  }

  void clear()
  {
    uint16_t i;
    for (i=0; i<_length; i++)
    {
      delete list[i];
    }
    _length = 0;
  }
  
} JumpList;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// this is used for the gosub / return 
class RETURN_STACK
{
  private:
  uint16_t stack[30]; // 30 gosub /return levels should be enough! 
  int _ptr = -1;

  public:
  void push(uint16_t pos)
  {
    _ptr = (_ptr + 1) % 30;
    stack[_ptr] = pos;
  }

  int pop()
  {
    if (_ptr < 0)
    {
      return -1; // error! return without gosub
    }
    _ptr--;
    return stack[_ptr + 1];
  }

  void clear()
  {
    _ptr = -1;
  }
  
}return_Stack;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// the error messages are defined here. 
// I don't know why, if I declare as usual String(F("message")) generate a memory conflict
PROGMEM const char _ERR1[] =  "Else Without IF line ";
PROGMEM const char _ERR2[] =  "End if Without IF line ";
PROGMEM const char _ERR3[] =  "If Without Endif line ";
PROGMEM const char _ERR5[] =  "Too much 'if else' nested ";
PROGMEM const char _ERR6[] =  "Too much 'if else' in the program ";
// this class works for IF THEN ELSE ENDIF
class IFBLOCKLIST
{
  private:
  struct IfBlock
  {
    uint16_t _ifpos;
    uint8_t _elsepos;   // in order to reduce memory, we limit the else and
    uint8_t _endifpos;  // endif position to max 255 lines after the if - this should be enough - 
  };

  int _ptr = -1;  // the stack pointer
  IfBlock stack[10]; // 10 nested if then else should be enough

  int _length = 0; // the list length
  IfBlock list[100]; // 100 max if then else should be enough
  
  public:
  void setIf(uint16_t pos)
  {
    if (_ptr >= 10)
    {
      PrintAndWebOut(String(_ERR5) + String(stack[_ptr]._ifpos));  // too much if endif nested
      return;
    }
    _ptr++;
    stack[_ptr]._ifpos = pos;
    stack[_ptr]._elsepos = 0;
    stack[_ptr]._endifpos = 0;
  }

  void setElse(uint16_t pos)
  {
    if ( _ptr <0 )
    {
      PrintAndWebOut(String(_ERR1) + String(stack[_ptr]._ifpos));  // endif without IF else without IF
      return;
    }
    stack[_ptr]._elsepos = pos - stack[_ptr]._ifpos;
  }
  
  void setEndif(uint16_t pos)
  {
    if ( _ptr <0 )
    {
      PrintAndWebOut(String(_ERR2) + String(pos));  // endif without IF
      return;
    }
    if (_length > 100)
    {
      PrintAndWebOut(String(_ERR6) + String(stack[_ptr]._ifpos));  // Too much 'if else' in the program
      return;
    }
    stack[_ptr]._endifpos = pos - stack[_ptr]._ifpos;

    list[_length]._ifpos = stack[_ptr]._ifpos;
    list[_length]._elsepos = stack[_ptr]._elsepos;
    list[_length]._endifpos = stack[_ptr]._endifpos;
    //Serial.println("Endif " + String(list[_length]->_ifpos) + " " +  String(list[_length]->_elsepos) + " " + String(list[_length]->_endifpos));
    
    _length++;
    _ptr--;
  }
  // gets the position of the associated else, if exists, endif if don't
  uint16_t getElse(uint16_t IFpos)
  {
    int i;
    for (i=0; i<_length; i++)
    {
      if ( list[i]._ifpos == IFpos)
      {
        if (list[i]._elsepos > 0)
          return list[i]._elsepos + list[i]._ifpos;
        else
          return list[i]._endifpos + list[i]._ifpos;
      }
    }
    return 0;
  }

  // gets the position of the associated endif
  uint16_t getEndIf(uint16_t Elsepos)
  {
    int i;
    for (i=0; i<_length; i++)
    {
      if ( (list[i]._elsepos + list[i]._ifpos) == Elsepos)
      {
        return list[i]._endifpos + list[i]._ifpos;
      }
    }
    return 0;
  }
  
  // check if all the if have been closed by endif
  int check()
  {
    if (_ptr != -1)
      PrintAndWebOut(String(_ERR3) + String(stack[_ptr]._ifpos) + " " + String(_ptr));  // if without endif
    empty_stack();
  }

  // empty the stack; the stack is used only during the search of the positions then needs to be cleaned
  void empty_stack()
  {
    _ptr = -1;
  }

  // empty the complete structure; this needs to be done at each file reload
  void clear()
  {
    uint16_t i;
    empty_stack();
    _length = 0;   
  }

  
} IfBlockList;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this class works for FOR NEXT
class FORNEXT
{
  private:
  struct LOOP
  {
    char _var[VariablesNameLength + 1];
    float _init_val;
    float _step;
    float _end_val;
    uint16_t _pos;
  } stack[10];     // 10 levels for next should be enough!

  int _ptr = -1;
  
  public:
  bool setFor(String ForLine, uint16_t position)
  {
    String v;
    int to_pos, equal_pos, step_pos;
    to_pos = ForLine.lastIndexOf(" to");
    equal_pos = ForLine.indexOf('=');
    step_pos = ForLine.lastIndexOf(" step");

    if ( (to_pos == -1) || (equal_pos == -1) )
    {
      return false; // the for is not valid
    }
      
    _ptr = (_ptr + 1) % 10; // this avoid crashes if the number of nested for loops is > 10
    v = ForLine.substring(4, equal_pos);
    v.trim();
    strncpy(stack[_ptr]._var, v.c_str(), VariablesNameLength + 1);

    v = ForLine.substring(equal_pos + 1, to_pos);    
    stack[_ptr]._init_val = v.toFloat();
    
    if (step_pos == -1) // no step defined
    {
      stack[_ptr]._step = 1;
      v = ForLine.substring(to_pos + 3);
      stack[_ptr]._end_val = v.toFloat(); 
    }
    else
    {
      v = ForLine.substring(to_pos + 3, step_pos);
      stack[_ptr]._end_val = v.toFloat(); 
      v = ForLine.substring(step_pos + 5);
      stack[_ptr]._step = v.toFloat();
    }

    SetMeThatVar(String(stack[_ptr]._var), FloatToString(stack[_ptr]._init_val), PARSER_TRUE);
    stack[_ptr]._pos = position;

    return true;
  }

  // returns the position of the next line
  // if the number is 0, this means that we must continue so the loop is over
  // if the number is -1, this means error (next without for)
  int checkNext(String NextLine)
  {
    float f;

    if (_ptr < 0)
    {
      return -1;  // next without for error
    }
    f = VarialbeLookup(String(stack[_ptr]._var)).toFloat() + stack[_ptr]._step;
    SetMeThatVar(String(stack[_ptr]._var), FloatToString(f), PARSER_TRUE);

    // here we must take care of the step direction
    // if positive or negative, the comparison is different
    float a;
    float b;
    if (stack[_ptr]._step > 0)
    {
      a = f;
      b = stack[_ptr]._end_val;
    }
    else
    {
      b = f;
      a = stack[_ptr]._end_val;
    }
    
    if (a > b)
    {
      // the loop is over
      _ptr--;
      return 0;
    }
    else
    {
      return stack[_ptr]._pos;
    }
  }

  void clear()
  {
    _ptr = -1;
  }
} forNextStack;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this class works for do ... loop {until | while}
class DOLOOP  
{
  private:
  uint16_t _pos[10];  // 10 nested loops should be enough!
  int _ptr = -1;
  
  public:
  void add(uint16_t pos)
  {
    _ptr = (_ptr + 1) % 10; // this avoid crashes if the number of nested for loops is > 10
    _pos[_ptr] = pos;
  }

  int checkLoop(String LoopLine)
  {
    int until_pos, while_pos, cond_pos;
    String cond;
    if (_ptr < 0)
    {
      return -1;  // loop without do error
    }
    until_pos = LoopLine.indexOf(" until ");
    while_pos = LoopLine.indexOf(" while ");
    cond_pos = until_pos + while_pos;
    if ( cond_pos == -2 )
    {
      return -2; // condition not valid
    }

    cond = LoopLine.substring(cond_pos + 8);

    evaluate(cond); // the result is in the global variable numeric_value; -1 = true; 0 = false;
    
    int r = 0;
    if (until_pos != -1)     // this is a tricky way to reverse the condition from true to false if until is selected
      r = 1;

    r = r + numeric_value;

    if (r == 0)
    {
      // the loop is over
      _ptr--;
      return 0;
    }
    else
    {
      return _pos[_ptr];
    }
  }

  void clear()
  {
    _ptr = -1;
  }
    
} doLoopStack;



