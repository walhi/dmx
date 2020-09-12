#include "adc.h"

#define ADLAR_VAL (1 << ADLAR)
#define PLUS_BUTTON_CH (ADLAR_VAL | (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0))
#define MINUS_BUTTON_CH (ADLAR_VAL | (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0))

#define P1_CH (ADLAR_VAL | (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0))
#define P2_CH (ADLAR_VAL | (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (1 << MUX0))
#define P3_CH (ADLAR_VAL | (0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (0 << MUX0))
#define P4_CH (ADLAR_VAL | (0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (1 << MUX0))

uint8_t virtualDDRReg = 0;
uint8_t virtualPORTReg = 0;
uint8_t virtualPINReg = 0;

uint8_t useButtons = 0;

extern uint8_t DMXData[512];
extern struct resistor settings[4];

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

void ADCSelect (uint8_t buttons)
{
	useButtons = buttons;
}


ISR(ADC_vect)
{
	switch (ADMUX){
	case PLUS_BUTTON_CH:
		if (ADCH > 100){
			BUT_PLUS_PIN_REG |= (1 << BUT_PLUS_PIN);
		} else {
			BUT_PLUS_PIN_REG &= ~(1 << BUT_PLUS_PIN);
		}
		ADMUX = MINUS_BUTTON_CH;
		break;
	case MINUS_BUTTON_CH:
		if (ADCH > 100){
			BUT_MINUS_PIN_REG |= (1 << BUT_MINUS_PIN);
		} else {
			BUT_MINUS_PIN_REG &= ~(1 << BUT_MINUS_PIN);
		}
		ADMUX = P1_CH;
		break;
	case P1_CH:
		DMXData[settings[0].address] = ADCH;
		ADMUX = P2_CH;
		break;
	case P2_CH:
		DMXData[settings[1].address] = ADCH;
		ADMUX = P3_CH;
		break;
	case P3_CH:
		DMXData[settings[2].address] = ADCH;
		ADMUX = P4_CH;
		break;
	case P4_CH:
		DMXData[settings[3].address] = ADCH;
		//break; // Он тут не нужен, чтобы сработало дополнительно правило default
	default:
		if (useButtons){
			ADMUX = PLUS_BUTTON_CH;
		} else {
			ADMUX = P1_CH;
		}
	}
	ADCSRA |= (1 << ADSC);
}
