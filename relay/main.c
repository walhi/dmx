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


struct relay {
	uint16_t address;
	uint8_t value;
};

struct relay EEMEM settingsEEP[4] = {{0,1},{0,1},{0,1},{0,1}};
struct relay settings[4];

uint8_t DMXData[512];

uint8_t state = TO_WORK_ST;
uint8_t oldState;

void pinsInit (void)
{
	BUT_MODE_DDR_REG &= ~(1 << BUT_MODE_PIN);
	BUT_PLUS_DDR_REG &= ~(1 << BUT_PLUS_PIN);
	BUT_MINUS_DDR_REG &= ~(1 << BUT_MINUS_PIN);

	BUT_MODE_PORT_REG |= (1 << BUT_MODE_PIN);
	BUT_PLUS_PORT_REG |= (1 << BUT_PLUS_PIN);
	BUT_MINUS_PORT_REG |= (1 << BUT_MINUS_PIN);


	RELAY_CH1_DDR_REG |= (1 << RELAY_CH1_PIN);
	RELAY_CH2_DDR_REG |= (1 << RELAY_CH2_PIN);
	RELAY_CH3_DDR_REG |= (1 << RELAY_CH3_PIN);
	RELAY_CH4_DDR_REG |= (1 << RELAY_CH4_PIN);

	RELAY_CH1_PORT_REG |= (1 << RELAY_CH1_PIN);
	RELAY_CH2_PORT_REG |= (1 << RELAY_CH2_PIN);
	RELAY_CH3_PORT_REG |= (1 << RELAY_CH3_PIN);
	RELAY_CH4_PORT_REG |= (1 << RELAY_CH4_PIN);
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
	uint8_t i = 0;
	uint16_t* address = NULL;
	uint8_t* value = NULL;
	uint8_t addressError = 0;

	wdt_enable(WDTO_2S);

	eeprom_read_block (settings, &settingsEEP, sizeof(settings));

	pinsInit();
	USARTInit();
	LEDInit();
	dynamicIndicationStart();

	sei();

	digitSet(1, 'o', 0);
	digitSet(2, 'n', 0);
	digitSet(3, ' ', 0);
	_delay_ms(400);
	dynamicIndicationStop();

	// eeprom не прошит. Нужно настраивать устройство.
	for (i = 0; i < 4; i++) if (settings[i].address > 512) state = TO_CFG_ST;

	while(1){
		wdt_reset();
		switch (state){
		case TO_CH1_CFG_ST:
		case TO_CH2_CFG_ST:
		case TO_CH3_CFG_ST:
		case TO_CH4_CFG_ST:
			digitSet(1, 'c', 0);
			digitSet(2, 'h', 0);
			switch (state){
			case TO_CH1_CFG_ST:
				digitSet(3, 1, 0);
				state = TO_CH1_ADR_CFG_ST;
				break;
			case TO_CH2_CFG_ST:
				digitSet(3, 2, 0);
				state = TO_CH2_ADR_CFG_ST;
				break;
			case TO_CH3_CFG_ST:
				digitSet(3, 3, 0);
				state = TO_CH3_ADR_CFG_ST;
				break;
			case TO_CH4_CFG_ST:
				digitSet(3, 4, 0);
				state = TO_CH4_ADR_CFG_ST;
				break;
			}
			_delay_ms(500);
			break;
		case TO_CH1_ADR_CFG_ST:
		case TO_CH2_ADR_CFG_ST:
		case TO_CH3_ADR_CFG_ST:
		case TO_CH4_ADR_CFG_ST:
			digitSet(1, 'a', 0);
			digitSet(2, 'd', 0);
			digitSet(3, 'r', 0);
			switch (state){
			case TO_CH1_ADR_CFG_ST:
				address = &(settings[0].address);
				state = CH1_ADR_CFG_ST;
				break;
			case TO_CH2_ADR_CFG_ST:
				address = &(settings[1].address);
				state = CH2_ADR_CFG_ST;
				break;
			case TO_CH3_ADR_CFG_ST:
				address = &(settings[2].address);
				state = CH3_ADR_CFG_ST;
				break;
			case TO_CH4_ADR_CFG_ST:
				address = &(settings[3].address);
				state = CH4_ADR_CFG_ST;
				break;
			}
			_delay_ms(500);
			break;
		case CH1_ADR_CFG_ST:
		case CH2_ADR_CFG_ST:
		case CH3_ADR_CFG_ST:
		case CH4_ADR_CFG_ST:
			uintToDigits(*address, addressError);
			addressError = 0;
			if ((*address) > 512) (*address) = 0;
			if ((!(BUT_PLUS_PIN_REG & (1 << BUT_PLUS_PIN))) && ((*address) < 511)) (*address)++;
			if ((!(BUT_MINUS_PIN_REG & (1 << BUT_MINUS_PIN))) && ((*address) > 0)) (*address)--;
			for (i = 0; i < 4; i++){
				if ((&(settings[i].address) != address) && (settings[i].address == (*address))){
					//oldState = state;
					//state = ERROR_ADR_ST;
					//break;
					addressError = 1;
				}
			}
			if (modeButtonGet(0))
				switch (state){
				case CH1_ADR_CFG_ST:
					state = TO_CH1_VAL_CFG_ST;
					break;
				case CH2_ADR_CFG_ST:
					state = TO_CH2_VAL_CFG_ST;
					break;
				case CH3_ADR_CFG_ST:
					state = TO_CH3_VAL_CFG_ST;
					break;
				case CH4_ADR_CFG_ST:
					state = TO_CH4_VAL_CFG_ST;
					break;
				}
			_delay_ms(50);
			break;
		case ERROR_ADR_ST:
				digitSet(1, 'e', 0);
				digitSet(2, 'r', 0);
				digitSet(3, 'r', 0);
			_delay_ms(300);
			state = oldState;
			break;
		case TO_CH1_VAL_CFG_ST:
		case TO_CH2_VAL_CFG_ST:
		case TO_CH3_VAL_CFG_ST:
		case TO_CH4_VAL_CFG_ST:
				digitSet(1, 'v', 0);
				digitSet(2, 'a', 0);
				digitSet(3, 'l', 0);
			switch (state){
			case TO_CH1_VAL_CFG_ST:
				value = &(settings[0].value);
				state = CH1_VAL_CFG_ST;
				break;
			case TO_CH2_VAL_CFG_ST:
				value = &(settings[1].value);
				state = CH2_VAL_CFG_ST;
				break;
			case TO_CH3_VAL_CFG_ST:
				value = &(settings[2].value);
				state = CH3_VAL_CFG_ST;
				break;
			case TO_CH4_VAL_CFG_ST:
				value = &(settings[3].value);
				state = CH4_VAL_CFG_ST;
				break;
			}
			_delay_ms(500);
			break;
		case CH1_VAL_CFG_ST:
		case CH2_VAL_CFG_ST:
		case CH3_VAL_CFG_ST:
		case CH4_VAL_CFG_ST:
			uintToDigits(*value, ((*value) == 0));
			if (!(BUT_PLUS_PIN_REG & (1 << BUT_PLUS_PIN)) && ((*value) < 255)) (*value)++;
			if (!(BUT_MINUS_PIN_REG & (1 << BUT_MINUS_PIN)) && ((*value) > 1)) (*value)--;
			if (modeButtonGet(0))
				switch (state){
				case CH1_VAL_CFG_ST:
					state = TO_CH2_CFG_ST;
					break;
				case CH2_VAL_CFG_ST:
					state = TO_CH3_CFG_ST;
					break;
				case CH3_VAL_CFG_ST:
					state = TO_CH4_CFG_ST;
					break;
				case CH4_VAL_CFG_ST:
					state = SAVE_ST;
					break;
				}
			_delay_ms(50);
			break;
		case SAVE_ST:
			digitSet(1, 's', 0);
			digitSet(2, 'a', 0);
			digitSet(3, 'v', 0);
			eeprom_write_block (settings, &settingsEEP, sizeof(settings));
			//
			_delay_ms(1000);
			modeButtonGet(1);
			state = TO_WORK_ST;
			break;
		case TO_CFG_ST:
			dynamicIndicationStart();
			ADCStart();
			state = TO_CH1_CFG_ST;
			break;
		case TO_WORK_ST:
			dynamicIndicationStop();
			ADCStop();
			state = WORK_ST;
			break;
		case WORK_ST:
			if (DMXData[settings[0].address] >= settings[0].value){
				RELAY_CH1_PORT_REG &= ~(1 << RELAY_CH1_PIN);
			} else {
				RELAY_CH1_PORT_REG |= (1 << RELAY_CH1_PIN);
			}
			if (DMXData[settings[1].address] >= settings[1].value){
				RELAY_CH2_PORT_REG &= ~(1 << RELAY_CH2_PIN);
			} else {
				RELAY_CH2_PORT_REG |= (1 << RELAY_CH2_PIN);
			}
			if (DMXData[settings[2].address] >= settings[2].value){
				RELAY_CH3_PORT_REG &= ~(1 << RELAY_CH3_PIN);
			} else {
				RELAY_CH3_PORT_REG |= (1 << RELAY_CH3_PIN);
			}
			if (DMXData[settings[3].address] >= settings[3].value){
				RELAY_CH4_PORT_REG &= ~(1 << RELAY_CH4_PIN);
			} else {
				RELAY_CH4_PORT_REG |= (1 << RELAY_CH4_PIN);
			}

			if (modeButtonGet(0)) state = TO_CFG_ST;
			break;
		}
	}
	return 0;
}
