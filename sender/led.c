#include "led.h"

static uint8_t dig1 = 0;
static uint8_t dig2 = 0;
static uint8_t dig3 = 0;

void LEDInit(void)
{
	SEG_A_DDR_REG  |= 1 << SEG_A_PIN;
	SEG_B_DDR_REG  |= 1 << SEG_B_PIN;
	SEG_C_DDR_REG  |= 1 << SEG_C_PIN;
	SEG_D_DDR_REG  |= 1 << SEG_D_PIN;
	SEG_E_DDR_REG  |= 1 << SEG_E_PIN;
	SEG_F_DDR_REG  |= 1 << SEG_F_PIN;
	SEG_G_DDR_REG  |= 1 << SEG_G_PIN;
	SEG_DP_DDR_REG |= 1 << SEG_DP_PIN;

	DIG1_DDR_REG   |= 1 << DIG1_PIN;
	DIG2_DDR_REG   |= 1 << DIG2_PIN;
	DIG3_DDR_REG   |= 1 << DIG3_PIN;
}

uint8_t charToSegments(char val, uint8_t dp)
{
	if (dp != 0) dp = 0b10000000;
	switch (val)
		{
		case 0:
		case '0':
			return 0b0111111 | dp;
		case 1:
		case '1':
			return 0b0000110 | dp;
		case 2:
		case '2':
			return 0b1011011 | dp;
		case 3:
		case '3':
			return 0b1001111 | dp;
		case 4:
		case '4':
			return 0b1100110 | dp;
		case 5:
		case '5':
			return 0b1101101 | dp;
		case 6:
		case '6':
			return 0b1111101 | dp;
		case 7:
		case '7':
			return 0b0000111 | dp;
		case 8:
		case '8':
			return 0b1111111 | dp;
		case 9:
		case '9':
			return 0b1101111 | dp;
		case '-':
			return 0b1000000 | dp;
		case 'a':
			return 0b1110111 | dp;
		case 'b':
			return 0b1111100 | dp;
		case 'c':
			return 0b1011000 | dp;
		case 'd':
			return 0b1011110 | dp;
		case 'e':
			return 0b1111001 | dp;
		case 'f':
			return 0b1110001 | dp;
		case 'h':
			return 0b1110100 | dp;
		case 'l':
			return 0b0111000 | dp;
		case 'n':
			return 0b1010100 | dp;
		case 'o':
			return 0b1011100 | dp;
		case 'r':
			return 0b1010000 | dp;
		case 's':
			return 0b1101101 | dp;
		case 't':
			return 0b1111000 | dp;
		case 'v':
			return 0b0111110 | dp;
		default:
			return 0b0;
		}
}

void outSegments(uint8_t data)
{
	if (data & 0b00000001) SEG_A_PORT_REG  |= (1 << SEG_A_PIN);  else SEG_A_PORT_REG  &= ~(1 << SEG_A_PIN);
	if (data & 0b00000010) SEG_B_PORT_REG  |= (1 << SEG_B_PIN);  else SEG_B_PORT_REG  &= ~(1 << SEG_B_PIN);
	if (data & 0b00000100) SEG_C_PORT_REG  |= (1 << SEG_C_PIN);  else SEG_C_PORT_REG  &= ~(1 << SEG_C_PIN);
	if (data & 0b00001000) SEG_D_PORT_REG  |= (1 << SEG_D_PIN);  else SEG_D_PORT_REG  &= ~(1 << SEG_D_PIN);
	if (data & 0b00010000) SEG_E_PORT_REG  |= (1 << SEG_E_PIN);  else SEG_E_PORT_REG  &= ~(1 << SEG_E_PIN);
	if (data & 0b00100000) SEG_F_PORT_REG  |= (1 << SEG_F_PIN);  else SEG_F_PORT_REG  &= ~(1 << SEG_F_PIN);
	if (data & 0b01000000) SEG_G_PORT_REG  |= (1 << SEG_G_PIN);  else SEG_G_PORT_REG  &= ~(1 << SEG_G_PIN);
	if (data & 0b10000000) SEG_DP_PORT_REG |= (1 << SEG_DP_PIN); else SEG_DP_PORT_REG &= ~(1 << SEG_DP_PIN);
}

void outDigit(uint8_t n)
{
	DIG1_PORT_REG &= ~(1 << DIG1_PIN);
	DIG2_PORT_REG &= ~(1 << DIG2_PIN);
	DIG3_PORT_REG &= ~(1 << DIG3_PIN);

	switch (n)
		{
		case 1:
			outSegments(dig1);
			DIG1_PORT_REG |= (1 << DIG1_PIN);
			break;
		case 2:
			outSegments(dig2);
			DIG2_PORT_REG |= (1 << DIG2_PIN);
			break;
		case 3:
			outSegments(dig3);
			DIG3_PORT_REG |= (1 << DIG3_PIN);
			break;
		default:
			break;
		}
}

void uintToDigits(uint16_t value, uint8_t dp)
{
	if (value <= 999){
		dig1 = charToSegments(value / 100, dp);
		dig2 = charToSegments(value % 100 / 10, dp);
		dig3 = charToSegments(value % 10, dp);
	} else {
		dig1 = dig2 = dig3 = charToSegments('-', 0);
	}

}

void dynamicIndicationStart(void)
{
	TCCR0 = (1 << CS02) | (0 << CS01) | (0 << CS00);
	TIMSK |= 1 << TOIE0;
	TCNT0 = 0;
}

void dynamicIndicationStop(void)
{
	TCCR0 = (0 << CS02) | (0 << CS01) | (0 << CS00);
	TIMSK &= ~(1 << TOIE0);
	TCNT0 = 0;

	DIG1_PORT_REG &= ~(1 << DIG1_PIN);
	DIG2_PORT_REG &= ~(1 << DIG2_PIN);
	DIG3_PORT_REG &= ~(1 << DIG3_PIN);
}

void digitSet(uint8_t n, char val, uint8_t dp){
	switch (n){
	case 1:
		dig1 = charToSegments(val, dp);
		break;
	case 2:
		dig2 = charToSegments(val, dp);
		break;
	case 3:
		dig3 = charToSegments(val, dp);
		break;
	}
}

ISR (TIMER0_OVF_vect)
{
	static uint8_t n = 1;
	if (n > 3) n = 1;
	outDigit(n);
	n++;
}
