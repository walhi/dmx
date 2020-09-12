#include <avr/io.h>
#include <avr/interrupt.h>
#include "signals.h"

// Запуск опроса состояния кнопок PLUS и MINUS.
void ADCStart(void);

// Остановка опроса состояния кнопок.
void ADCStop(void);
