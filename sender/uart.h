#include <avr/io.h>
#include <avr/interrupt.h>
#include "signals.h"

// Инициализация UART
void USARTInit(void);

// Состояние связи (DMX пакеты приходят)
uint8_t DMXStateGet(void);

// Сброс состояния. Вынесен в отдельную функцию, чтобы не занимать таймер.
void DMXStateClear(void);
