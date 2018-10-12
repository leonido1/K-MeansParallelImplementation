
#include "ompSupport.h"

void setBoolArrToFalseValues(boolean* bolArr) 
{
  int i = 0;

  for (i; i < THEARDS_ON_MACHINE; i++)
    bolArr[i] = False;

}

boolean orReduceBololeanArr(boolean* bolArr)
{
  int i = 0;
  boolean result= False;

  for (i; i < THEARDS_ON_MACHINE; i++)
  {
    if (bolArr[i] == True)
      result = True;
  
  }
  return result;
}