// Digits control
#define DIG1_DDR_REG DDRB
#define DIG1_PORT_REG PORTB
#define DIG1_PIN PB3

#define DIG2_DDR_REG DDRB
#define DIG2_PORT_REG PORTB
#define DIG2_PIN PB4

#define DIG3_DDR_REG DDRB
#define DIG3_PORT_REG PORTB
#define DIG3_PIN PB5


// Segments control
#define SEG_A_DDR_REG DDRD
#define SEG_A_PORT_REG PORTD
#define SEG_A_PIN PD5

#define SEG_B_DDR_REG DDRD
#define SEG_B_PORT_REG PORTD
#define SEG_B_PIN PD3

#define SEG_C_DDR_REG DDRB
#define SEG_C_PORT_REG PORTB
#define SEG_C_PIN PB1

#define SEG_D_DDR_REG DDRD
#define SEG_D_PORT_REG PORTD
#define SEG_D_PIN PD7

#define SEG_E_DDR_REG DDRD
#define SEG_E_PORT_REG PORTD
#define SEG_E_PIN PD6

#define SEG_F_DDR_REG DDRD
#define SEG_F_PORT_REG PORTD
#define SEG_F_PIN PD4

#define SEG_G_DDR_REG DDRB
#define SEG_G_PORT_REG PORTB
#define SEG_G_PIN PB2

#define SEG_DP_DDR_REG DDRB
#define SEG_DP_PORT_REG PORTB
#define SEG_DP_PIN PB0

// Buttons
#define BUT_MODE_DDR_REG DDRC
#define BUT_MODE_PORT_REG PORTC
#define BUT_MODE_PIN_REG PINC
#define BUT_MODE_PIN PC4

extern uint8_t virtualDDRReg;
extern uint8_t virtualPORTReg;
extern uint8_t virtualPINReg;

#define BUT_PLUS_DDR_REG virtualDDRReg
#define BUT_PLUS_PORT_REG virtualPORTReg
#define BUT_PLUS_PIN_REG virtualPINReg
#define BUT_PLUS_PIN 0

#define BUT_MINUS_DDR_REG virtualDDRReg
#define BUT_MINUS_PORT_REG virtualPORTReg
#define BUT_MINUS_PIN_REG virtualPINReg
#define BUT_MINUS_PIN 1


//relay control
#define RELAY_CH1_DDR_REG DDRC
#define RELAY_CH1_PORT_REG PORTC
#define RELAY_CH1_PIN_REG PINC
#define RELAY_CH1_PIN PC0

#define RELAY_CH2_DDR_REG DDRC
#define RELAY_CH2_PORT_REG PORTC
#define RELAY_CH2_PIN_REG PINC
#define RELAY_CH2_PIN PC1

#define RELAY_CH3_DDR_REG DDRC
#define RELAY_CH3_PORT_REG PORTC
#define RELAY_CH3_PIN_REG PINC
#define RELAY_CH3_PIN PC2

#define RELAY_CH4_DDR_REG DDRC
#define RELAY_CH4_PORT_REG PORTC
#define RELAY_CH4_PIN_REG PINC
#define RELAY_CH4_PIN PC3

// RS-485 Control
#define RE_DE_U2_DDR_REG DDRD
#define RE_DE_U2_PORT_REG PORTD
#define RE_DE_U2_PIN_REG PIND
#define RE_DE_U2_PIN PD2

#define RE_DE_U4_DDR_REG DDRC
#define RE_DE_U4_PORT_REG PORTC
#define RE_DE_U4_PIN_REG PINC
#define RE_DE_U4_PIN PC5
