#include <msp430g2553.h>

//#include <msp430.h>

//#define red_LED   BIT0
//#define grn_LED   BIT6
//#define BTN       BIT3

int delay(int t)
{
  int i, s;
 
  s = 0;
  for (i = 0; i < t; i++)
    s += i;
 
  return s;
}
 
int main(void)
{
  WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

  P1OUT = 0;
  P1DIR |= BIT0;		// P1.0 (Red) -> Output
  P1DIR |= BIT6;		// P1.6 (Green) -> Output

  P1DIR &= ~BIT3;		// P1.3 (SW2) -> Input

// Если у вас LaunchPad версии 1.5, используйте эти две строчки
//  P1REN |= BTN; //разрешаем подтяжку
//  P1OUT |= BTN; //подтяжка вывода P1.3 вверх
  P1REN |= BIT3;		// P1.3 Pull Up/Down Enable
  P1OUT |= BIT3;		// P1.3 Pull Up Enable

  while (1)
  {
    if (P1IN & BIT3)
        P1OUT |= BIT0;
    else
        P1OUT |= BIT6;
    delay(10000);
    P1OUT &= ~(BIT0 | BIT6);
    delay(10000);
  }
}
