#include "uart.h"

#define USART_BAUDRATE 250000
#define UBRR_VALUE (F_CPU/16/USART_BAUDRATE-1)

extern uint8_t DMXData[512];

static uint8_t  DMXState          = 0;
static uint8_t  DMXBreak          = 0;
static uint16_t DMXCurrentChannel = 0;


void USARTInit(void)
{
	// init uart
	UBRRL = F_CPU/4000000 - 1;
	UBRRH =  0; // baud rate 250kbps
	UCSRA = 0; // clear error flags
	UCSRC = (1 << URSEL) | (1 << USBS) | (3 << UCSZ0); // 8 data bits, 2 stop bits, no parity (8N2)
	UCSRB = 0; // don't turn on UART jet...

	// Set baud rate
	UBRRH = (uint8_t)(UBRR_VALUE >> 8);
	UBRRL = (uint8_t)UBRR_VALUE;
	// Set frame format: 8data, 2stop bit
	UCSRC = (1 << URSEL) | (1 << USBS) | (3 << UCSZ0);

	// init timer0 for DMX timing
	TCCR0 = 2; // prescaler 8 => 1 clock is 2/3 us

	// - set Timer 1 prescaler to 64 and restart timer
	TCCR1B = 3;
	TCNT1 = 0;
	TIFR |= (1 << TOV1);


	// RS-485 RE/DE control
	RE_DE_U2_DDR_REG |= (1 << RE_DE_U2_PIN);
	RE_DE_U4_DDR_REG |= (1 << RE_DE_U4_PIN);

	RE_DE_U2_PORT_REG |= (1 << RE_DE_U2_PIN);
	RE_DE_U4_PORT_REG |= (1 << RE_DE_U4_PIN);

}

ISR(USART_RXC_vect)
{
	unsigned char UARTStatus = UCSRA;
	unsigned char UARTData = UDR;
	// Frame error (new packet)
	if (UARTStatus & (1<<FE)){
		DMXCurrentChannel = 0;
		DMXBreak = 1;
	} else if (DMXBreak == 1){
		DMXData[DMXCurrentChannel] = UARTData;
		DMXCurrentChannel++;
		DMXState = 1;
		if (DMXCurrentChannel >= 512) DMXBreak = 0;
	}
}
