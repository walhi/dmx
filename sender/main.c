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
#include "defines.h"


struct resistor EEMEM settingsEEP[4] = {{0},{1},{2},{3}};
struct resistor settings[4];

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

// values for dmx_state
#define dmx_Off 0
#define dmx_NewPacket 1
#define dmx_InPacket 2
#define dmx_EndOfPacket 3
#define dmx_InBreak 4
#define dmx_InMAB 5

int	main(void)
{
	uint8_t i = 0;
	uint16_t* address = NULL;
	uint8_t addressError = 0;

	uint8_t dmx_state;
	uint16_t out_idx;			// index of next frame to send
	uint16_t packet_len = 512;

	wdt_enable(WDTO_2S);

	eeprom_read_block (settings, &settingsEEP, sizeof(settings));

	pinsInit();
	USARTInit();
	LEDInit();
	dynamicIndicationStart();
	ADCStart();

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
					state = TO_CH2_ADR_CFG_ST;
					break;
				case CH2_ADR_CFG_ST:
					state = TO_CH3_ADR_CFG_ST;
					break;
				case CH3_ADR_CFG_ST:
					state = TO_CH4_ADR_CFG_ST;
					break;
				case CH4_ADR_CFG_ST:
					state = SAVE_ST;
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
			ADCSelect(1);
			state = TO_CH1_CFG_ST;
			break;
		case TO_WORK_ST:
			dynamicIndicationStop();
			ADCSelect(0);
			USARTInit();
			state = WORK_ST;
			break;
		case WORK_ST:
			//DMXData[settings[0].address] = ;
			// do dmx transmission
			switch(dmx_state) {
			case dmx_NewPacket: {
				// start a new dmx packet:
				UCSRB |= (1 << TXEN);	// enable UART transmitter
				out_idx = 0;		// reset output channel index
				UCSRA |= (1 << TXC);	// reset Transmit Complete flag
				UDR =  0;		// send start byte
				dmx_state = dmx_InPacket;
				break;
			}
			case dmx_InPacket: {
				if(UCSRA & (1 << UDRE)) {
					// send next byte of dmx packet
					if(out_idx < packet_len) { UDR =  DMXData[out_idx++]; break; }
					else dmx_state = dmx_EndOfPacket;
				}
				else break;
			}
			case dmx_EndOfPacket: {
				if(UCSRA & (1 << TXC)) {
					// send a BREAK:
					UCSRB &= ~(1 << TXEN);	// disable UART transmitter
					DDRD |= (1 << PD1);		// pull TX pin low
					PORTD &= ~(1 << PD1);		// pull TX pin low

					SFIOR |= (1 << PSR10);	// reset timer prescaler
					TCNT0 = 123;		// 132 clks = 88us
					TIFR |= (1 << TOV0);	// clear timer overflow flag
					dmx_state = dmx_InBreak;
				}
				break;
			}
			case dmx_InBreak: {
				if(TIFR & (1 << TOV0)) {

					// end of BREAK: send MARK AFTER BREAK
					PORTD |= (1 << PD1);		// pull TX pin high
					SFIOR |= (1 << PSR10);	// reset timer prescaler
					TCNT0 = 243;		// 12 clks = 8us
					TIFR |= (1 << TOV0);	// clear timer overflow flag
					dmx_state = dmx_InMAB;
				}
				break;
			}
			case dmx_InMAB: {
				if(TIFR & (1 << TOV0)) {
					// end of MARK AFTER BREAK; start new dmx packet
					dmx_state = dmx_NewPacket;
				}
				break;
			}
			}

			if (modeButtonGet(0)) state = TO_CFG_ST;
			break;
		}
	}
	return 0;
}
