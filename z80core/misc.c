/* Misc.c: Z80 misc arithmetic & CPU control instructions.
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "env.h"

extern void cpu_halted_interrupts_disabled();

/*=========================================================================*
 *                            daa                                          *
 *=========================================================================*/
void daa()
{
   LOCAL UCHAR t;
    
   t=0;
    
   T(4);
    
   if(Z80_H || (((A & (UCHAR)0xF) > (UCHAR)9) ))
         t++;
    
   if(Z80_C || ((UCHAR)A) > (UCHAR)0x99)
   {
            t += 2;
            Z80_C = 1;
   }
    
    if ( Z80_N && !Z80_H )
      Z80_H=0;
    else
    {
       if ( Z80_N && Z80_H )
          Z80_H = (((UCHAR)(A & 0x0F)) < 6);
      else
           Z80_H = (((UCHAR)(A & 0x0F)) >= 0x0A);
    }
    
    switch(t)
    {
        case 1:
            A += (Z80_N)?(UCHAR)0xFA:(UCHAR)6; // -6:6
            break;
        case 2:
            A += (Z80_N)?(UCHAR)0xA0:(UCHAR)0x60; // -0x60:0x60
            break;
        case 3:
            A += (Z80_N)?(UCHAR)0x9A:(UCHAR)0x66; // -0x66:0x66
            break;
    }
    
   Z80_S = ( A & (UCHAR)BIT_7);
   Z80_Z = !A;
   Z80_P = parity(A);
   Z80_X = A & (UCHAR)BIT_5;
   Z80_Y = A & (UCHAR)BIT_3;
   Q = 1;
}

/*=========================================================================*
 *                            cpl                                          *
 *=========================================================================*/
void cpl()
{
   T(4);
   A = (UCHAR)~A;
   Z80_H = Z80_N = 1;
   Z80_X = A & (UCHAR)BIT_5;
   Z80_Y = A & (UCHAR)BIT_3;
   Q = 1;
}

/*=========================================================================*
 *                            neg                                          *
 *=========================================================================*/
void neg()
{
   T(8);
   Z80_S = ((A =(UCHAR)(~A + 1)) & (UCHAR)BIT_7);
   Z80_P = (A == (UCHAR)BIT_7);
   Z80_H = A & 0x0F;
   Z80_C = A;
   Z80_Z = !A;
   Z80_N = 1;
   Z80_X = A & (UCHAR)BIT_5;
   Z80_Y = A & (UCHAR)BIT_3;
   Q = 1;
}

/*=========================================================================*
 *                            ccf                                          *
 *=========================================================================*/
void ccf()
{
   T(4);
   Z80_C = !(Z80_H = Z80_C);
   Z80_N = 0;
   if (Q)
   {
   Z80_X = (A & (UCHAR)BIT_5) > 0;
   Z80_Y = (A & (UCHAR)BIT_3) > 0;
   }
   else
   {
   Z80_X = Z80_X || (A & (UCHAR)BIT_5);
   Z80_Y = Z80_Y || (A & (UCHAR)BIT_3);
   }
   Q = 1;
}

/*=========================================================================*
 *                            scf                                          *
 *=========================================================================*/
void scf()
{
   T(4);
   Z80_C = 1;
   Z80_H = Z80_N = 0;
   if (Q)
   {
   Z80_X = (A & (UCHAR)BIT_5) > 0;
   Z80_Y = (A & (UCHAR)BIT_3) > 0;
   }
   else
   {
   Z80_X = Z80_X || (A & (UCHAR)BIT_5);
   Z80_Y = Z80_Y || (A & (UCHAR)BIT_3);
   }
   Q = 1;
}

/*=========================================================================*
 *                            nop                                          *
 *=========================================================================*/
void nop()
{
   T(4);
   Q = 0;
}

/*=========================================================================*
 *                            halt                                         *
 *=========================================================================*/
void halt()
{
   if(!IFF1)
   {
       //Panic("HALT with interrupts disabled!");
       cpu_halted_interrupts_disabled();
   }

   /* This loop is cut because timing software is already
      working
   while(!int_time())
   {
      nop();
   }  */
   clock_ticks = INT_TIME + (clock_ticks % 4 );
   Q = 0;
}

/*=========================================================================*
 *                            di                                           *
 *=========================================================================*/
void di()
{
   T(4);

   // Instead of delay setting IFF1/IFF2, play with int Time
   // not entirely compatible with the original chipset
   //
   // Useless for DI
   //
   //if(int_time())
   // clock_ticks = INT_TIME - 3;

   IFF1 = IFF2 = 0;
   Q = 0;
}

/*=========================================================================*
 *                            ei                                           *
 *=========================================================================*/
void ei()
{
   T(4);

   // Instead of delay setting IFF1/IFF", play with int Time
   // not entirely compatible with the original chipset
   if(int_time())
      clock_ticks = INT_TIME - 3;

   IFF1 = IFF2 = 1;
   Q = 0;
}

/*=========================================================================*
 *                            im_0                                         *
 *=========================================================================*/
void im_0()
{
   T(8);
   _IM = 0;
   Q = 0;
}

/*=========================================================================*
 *                            im_1                                         *
 *=========================================================================*/
void im_1()
{
   T(8);
   _IM = 1;
   Q = 0;
}

/*=========================================================================*
 *                            im_2                                         *
 *=========================================================================*/
void im_2()
{
   T(8);
   _IM = 2;
   Q = 0;
}


/*** auxiliary routines  ***/

/*=========================================================================*
 *                            dummyED                                      *
 *=========================================================================*/
void dummyED()
{
   T(8);
   /* invalid opcode (non existant) */
   Q = 0;
}

void dummyIX()
{
   T(4);
   PC--;
   Q = 0;
}

void dummyIY()
{
   T(4);
   PC--;
   Q = 0;
}

/* No longer used! --- replace by an array
 */

/*=========================================================================*
 *                            parity                                       *
 *=========================================================================*/
/* parity -- in Z80 parity is even */
/* USHORT parity(num)
UCHAR num;
{
   LOCAL UCHAR parit;
   LOCAL UCHAR number;

   number = num;
   parit = 1;
   while(number)
   {
      if((UCHAR)0x01 & number)
	 parit ^= (UCHAR)1;
      number >>= 1;
   }
   return((USHORT)parit);
}
*/
/* Misc.c */

