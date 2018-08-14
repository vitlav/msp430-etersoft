#include <msp430.h>

// TODO: почему-то не выводит в порт

void print(char *text)
{
	unsigned int i = 0;
	while(text[i] != '\0')
	{
		while (!(IFG2&UCA0TXIFG));		// Check if TX is ongoing
		UCA0TXBUF = text[i];			// TX -> Received Char + 1
		i++;
	}
}

void printNumber(unsigned int num)
{
	char buf[6];
	char *str = &buf[5];

	*str = '\0';

	do
	{
		unsigned long m = num;
		num /= 10;
		char c = (m - 10 * num) + '0';
		*--str = c;
	} while(num);

	print(str);
}



volatile long adcValue, tempC = 10;


#define LED	BIT6						// Green LED -> P1.6

#pragma vector = TIMER0_A0_VECTOR		// CCR0 Interrupt Vector
__interrupt void CCR0_ISR(void)
{
	P1OUT ^= LED;						// Toggle LED
	if(!(ADC10CTL1 & ADC10BUSY)) {				// Check if ADC Conversion is in progress
		ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start

		adcValue = ADC10MEM;					// Fetch ADC conversion result

		// C = ( (adcValue/1024)*1500mV)-986mV)*1/3.55mV
		tempC = ((adcValue - 673) * 423) / 1024;
		//tempC++;
		if (tempC > 60)
		    tempC = 10;

		TACCR0 = (60 - tempC) * 100;						// Set Timer Timeout Value
		TACTL |= MC_1 + TASSEL_1 + TACLR ;	// Set Mode -> Up Count, Clock -> ACLK, Clear Timer
	}
}

#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
	__bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}


void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 	// Stop watchdog timer

	if (CALBC1_1MHZ==0xFF)				// Check if calibration constant erased
	{
		while(1);						// do not load program
	}
	DCOCTL = 0;							// Select lowest DCO settings
	BCSCTL1 = CALBC1_1MHZ;				// Set DCO to 1 MHz
	DCOCTL = CALDCO_1MHZ;

	P1SEL = BIT1 + BIT2 ;				// Select UART RX/TX function on P1.1,P1.2
	P1SEL2 = BIT1 + BIT2;

	UCA0CTL1 |= UCSSEL_2;				// UART Clock -> SMCLK
	UCA0BR0 = 104;						// Baud Rate Setting for 1MHz 9600
	UCA0BR1 = 0;						// Baud Rate Setting for 1MHz 9600
	UCA0MCTL = UCBRS_1;					// Modulation Setting for 1MHz 9600
	UCA0CTL1 &= ~UCSWRST;				// Initialize UART Module
	

    P1DIR |= LED;						// Set LED pin -> Output
    P1OUT &=~ LED;						// Turn OFF LED

    TACCR0 = 1000;						// Set Timer Timeout Value
    TACCTL0 |= CCIE;					// Enable Overflow Interrupt
    TACTL |= MC_1 + TASSEL_1 + TACLR ;	// Set Mode -> Up Count, Clock -> ACLK, Clear Timer

    __bis_SR_register(LPM3_bits + GIE);	// Goto LPM3 (Only ACLK active), Enable CPU Interrupt

    ADC10CTL1 = INCH_10 + ADC10DIV_3;         	// ADC Channel -> 10 (Temp Sensor), CLK/4
    ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + ADC10IE;

							// Ref -> 1.5V, 64 CLK S&H, Interrupt Enabled
	__delay_cycles(100);						// Wait for reference to settle
	while(1)
	{
		printNumber(tempC);
		print("\r\n");
		__delay_cycles(10000);
		continue;
		//ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
		//__bis_SR_register(CPUOFF + GIE);        // LPM0 with interrupts enabled

		adcValue = ADC10MEM;					// Fetch ADC conversion result

		// C = ( (adcValue/1024)*1500mV)-986mV)*1/3.55mV
		tempC = ((adcValue - 673) * 423) / 1024;

		TACCR0 = tempC * 100;						// Set Timer Timeout Value
		//TACTL |= MC_1 + TASSEL_1 + TACLR ;	// Set Mode -> Up Count, Clock -> ACLK, Clear Timer

		__no_operation();                       // Required for breakpoint
	}
}

