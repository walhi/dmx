#include "adc.h"

#define PLUS_BUTTON_CH ((1 << ADLAR) | (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0))
#define MINUS_BUTTON_CH ((1 << ADLAR) | (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0))

uint8_t virtualDDRReg = 0;
uint8_t virtualPORTReg = 0;
uint8_t virtualPINReg = 0;

void ADCStart (void)
{
	ADMUX = PLUS_BUTTON_CH;
	ADCSRA |= (1 << ADEN) | (1 << ADIE);
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADCSRA |= (1 << ADSC);
}

void ADCStop (void)
{
	ADMUX = 0;
	ADCSRA = 0;
}

ISR(ADC_vect)
{
	if (ADMUX == PLUS_BUTTON_CH){
		if (ADCH > 100){
			BUT_PLUS_PIN_REG |= (1 << BUT_PLUS_PIN);
		} else {
			BUT_PLUS_PIN_REG &= ~(1 << BUT_PLUS_PIN);
		}
		ADMUX = MINUS_BUTTON_CH;
	} else if (ADMUX == MINUS_BUTTON_CH){
		if (ADCH > 100){
			BUT_MINUS_PIN_REG |= (1 << BUT_MINUS_PIN);
		} else {
			BUT_MINUS_PIN_REG &= ~(1 << BUT_MINUS_PIN);
		}
		ADMUX = PLUS_BUTTON_CH;
	} else {
		ADMUX = PLUS_BUTTON_CH;
	}
	ADCSRA |= (1 << ADSC);
}
