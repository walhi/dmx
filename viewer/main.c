#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdlib.h>
#include "signals.h"
#include "uart.h"
#include "adc.h"
#include "led.h"
#include "state.h"

uint8_t DMXData[512];

uint8_t state = TO_SHOW_ST;
uint8_t oldState;

void pinsInit (void)
{
	BUT_MODE_DDR_REG &= ~(1 << BUT_MODE_PIN);
	BUT_PLUS_DDR_REG &= ~(1 << BUT_PLUS_PIN);
	BUT_MINUS_DDR_REG &= ~(1 << BUT_MINUS_PIN);

	BUT_MODE_PORT_REG |= (1 << BUT_MODE_PIN);
	BUT_PLUS_PORT_REG |= (1 << BUT_PLUS_PIN);
	BUT_MINUS_PORT_REG |= (1 << BUT_MINUS_PIN);
}

uint8_t modeButtonGet (uint8_t clear)
{
	static uint8_t oldPinState = 0;// = (BUT_MODE_PIN_REG & (1 << BUT_MODE_PIN)) == 0;
	uint8_t pinState = (BUT_MODE_PIN_REG & (1 << BUT_MODE_PIN)) == 0;

	if (clear) oldPinState = pinState;
	if (pinState != oldPinState){
		oldPinState = pinState;
		if (!pinState) return 1;
	}
	return 0;
}

int	main(void)
{
	uint16_t address = 3;
	uint8_t addressError = 0;

	wdt_enable(WDTO_2S);

	pinsInit();
	USARTInit();
	LEDInit();
	dynamicIndicationStart();

	sei();

	digitSet(1, 'o', 0);
	digitSet(2, 'n', 0);
	digitSet(3, ' ', 0);
	_delay_ms(400);

	while(1){
		wdt_reset();
		switch (state){
		case TO_ADDR_SEL_ST:
			digitSet(1, 'a', 0);
			digitSet(2, 'd', 0);
			digitSet(3, 'r', 0);
			ADCStart();
			_delay_ms(500);
			state = ADDR_SEL_ST;
			break;
		case ADDR_SEL_ST:
			uintToDigits(address, addressError);
			addressError = 0;
			if ((address) > 512) (address) = 0;
			if ((!(BUT_PLUS_PIN_REG & (1 << BUT_PLUS_PIN))) && (address < 511)) address++;
			if ((!(BUT_MINUS_PIN_REG & (1 << BUT_MINUS_PIN))) && (address > 0)) address--;
			if (modeButtonGet(0)) state = TO_SHOW_ST;
			_delay_ms(50);
			break;
		case ERROR_ADR_ST:
			digitSet(1, 'e', 0);
			digitSet(2, 'r', 0);
			digitSet(3, 'r', 0);
			_delay_ms(300);
			state = oldState;
			break;
		case TO_SHOW_ST:
			digitSet(1, 'r', 0);
			digitSet(2, 'u', 0);
			digitSet(3, 'n', 0);
			ADCStop();
			_delay_ms(500);
			state = SHOW_ST;
			break;
		case SHOW_ST:
			uintToDigits(DMXData[address], addressError);
			if (modeButtonGet(0)) state = TO_ADDR_SEL_ST;
			break;
		}
	}
	return 0;
}
