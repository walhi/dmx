#include <avr/io.h>
#include <avr/interrupt.h>
#include "signals.h"

// Инициализация портов ввода-вывода
void LEDInit(void);

// Запись в место n символа val. Переменная dp - наличие точки.
void digitSet(uint8_t n, char val, uint8_t dp);

// Запись беззнакового числа в семисегментный индикатор. До 999.
void uintToDigits(uint16_t value, uint8_t dp);

// Запуск динамической индикации.
void dynamicIndicationStart(void);

// Остановка динамической индикации и выключени семисегментного индикатора.
void dynamicIndicationStop(void);
