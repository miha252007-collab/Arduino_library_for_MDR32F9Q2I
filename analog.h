#ifndef ANALOG_H
#define ANALOG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef A0
#define A0  14
#endif
#ifndef A1
#define A1  15
#endif
#ifndef A2
#define A2  16
#endif
#ifndef A3
#define A3  17
#endif
#ifndef A4
#define A4  18
#endif
#ifndef A5
#define A5  19
#endif
#define A6  20
#define A7  21

#define NUM_ANALOG_PINS  8

#define DEFAULT   0
#define EXTERNAL  1

void analogReadResolution(uint8_t bits);
void analogReference(uint8_t type);
int  analogRead(uint8_t pin);

#ifdef __cplusplus
}
#endif

#endif