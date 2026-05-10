#ifndef ARDUINO_H
#define ARDUINO_H

#include <stdint.h>
#include "pins_arduino.h"

// core
void init(void);

// digital
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);

// wiring TIMER
void SystemClock_Config(void);
uint32_t millis(void);
uint32_t micros(void);
void delay(uint32_t ms);
void delayMicroseconds(uint32_t us);

// constants
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

#define LOW 0
#define HIGH 1

#endif