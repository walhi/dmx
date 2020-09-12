#include "uart.h"

#define USART_BAUDRATE 250000
#define UBRR_VALUE (F_CPU/16/USART_BAUDRATE-1)

extern uint8_t DMXData[512];

static uint8_t  DMXState          = 0;
static uint8_t  DMXBreak          = 0;
static uint16_t DMXCurrentChannel = 0;


void USARTInit(void)
{
    // Set baud rate
    UBRRH = (uint8_t)(UBRR_VALUE >> 8);
    UBRRL = (uint8_t)UBRR_VALUE;
    // Set frame format: 8data, 2stop bit
    UCSRC = (1 << URSEL) | (1 << USBS) | (3 << UCSZ0);
    // Enable reception
    UCSRB |= (1 << RXEN);
    // Enable interrupt
    UCSRB |= (1 << RXCIE);

		// RS-485 RE/DE control
		RE_DE_U2_DDR_REG |= (1 << RE_DE_U2_PIN);
		RE_DE_U4_DDR_REG |= (1 << RE_DE_U4_PIN);

		RE_DE_U2_PORT_REG &= ~(1 << RE_DE_U2_PIN);
		RE_DE_U4_PORT_REG &= ~(1 << RE_DE_U4_PIN);

}

uint8_t DMXStateGet(void)
{
	return DMXState;
}

void DMXStateClear(void)
{
	DMXState = 0;
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
