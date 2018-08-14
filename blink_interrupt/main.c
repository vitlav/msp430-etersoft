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

volatile char sw = 0;

#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR(void) {
//    switch(P1IFG & BIT3) {
//        case BIT3:
            P1IFG &= ~BIT3;    // обнуляем флаг прерывания для P1.3

            //BCSCTL1 = bcs_vals[i];
            //DCOCTL = dco_vals[i];
            //if (++i == 3)
            //    i = 0;
            if (sw) {
                sw = 0;
                P1IES &= ~BIT3;  // прерывание по переходу из 0 в 1
            } else
            {
                sw = 1;
                P1IES |= BIT3;  // прерывание по переходу из 1 в 0
            }

            return;
/*        default:
            P1IFG = 0;   // Возможно в этом нет необходимости, но обнуляем
                         // флаги всех прерываний в P1, на всякий случай. 
                         // Хотя лучше было бы добавить обработчик ошибки.
            return;
    }
*/
} // P1_ISR

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


  P1IES |= BIT3;  // прерывание по переходу из 1 в 0, 
                  // устанавливается соответствующим битом IES.x = 1.
  P1IFG &= ~BIT3; // Для предотвращения немедленного срабатывания прерывания,
                  // обнуляем его флаг для P1.3 до разрешения прерываний
  P1IE |= BIT3;   // Разрешаем прерывания для P1.3

  _BIS_SR(GIE);
  // LPM4 — режим полного сна, нам не нужно здесь
  //__bis_SR_register(LPM4_bits + GIE); // Enter LPM4 and Enable CPU Interrupt

  while (1)
  {
//    if (P1IN & BIT3)
    if (sw)
        P1OUT |= BIT0;
    else
        P1OUT |= BIT6;
    delay(10000);
    //__delay_cycles(400000);
    P1OUT &= ~(BIT0 | BIT6);
    //__delay_cycles(400000);
    delay(10000);
  }
}
