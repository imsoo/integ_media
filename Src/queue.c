/*      

    queue.c

*/


#include "queue.h"

#define QO_SIZE 1024
#define QI_SIZE 1024
unsigned char qo[QO_SIZE], qi[QI_SIZE];
unsigned int fi, ri, fo, ro;

void q_init(void)
{
   fi = ri = fo = ro = 0;
}

unsigned int qi_insert(unsigned char ch)
{
   if ((ri + 1) % QI_SIZE == fi)	// full
      return(0);
   ri = (ri + 1) % QI_SIZE;
   qi[ri] = ch;
   return(1);

}

unsigned int qi_delete(void)
{
   if (ri == fi)	// empty
      return(0);
   fi = (fi + 1) % QI_SIZE;
   return(qi[fi]);
}

unsigned int qi_remove(void)
{
  if (ri == fi)
    return(0);
  ri = (ri - 1) % QI_SIZE;
  return 1;
}

unsigned int qo_insert(unsigned char ch)
{
   if ((ro + 1) % QO_SIZE == fo)	// full
      return(0);
   ro = (ro + 1) % QO_SIZE;
   qo[ro] = ch;
   return(1);
}

unsigned int qo_delete(void)
{
   if (ro == fo)	// empty
      return(0);
   fo = (fo + 1) % QO_SIZE;
   return(qo[fo]);
}