#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "signals.h"
#include "defines.h"

// Запуск опроса состояния кнопок PLUS и MINUS.
void ADCStart(void);

// Остановка опроса состояния кнопок.
void ADCStop(void);

void ADCSelect (uint8_t buttons);

#endif
