#ifndef PINS_ARDUINO_H
#define PINS_ARDUINO_H

#include "MDR32F9Q2I.h"
#include <stdint.h>


#define A0                                                 14
#define A1                                                 15
#define A2                                                 16
#define A3                                                 17
#define A4                                                 18
#define A5                                                 19

#define SS																								20//PF2 SS
#define SCK  	                                           	13//PF1 SCK
#define MOSI 	                                           	21//PF0 MOSI
#define MISO																							12//PF3 MISO

typedef struct{
    volatile MDR_PORT_TypeDef *port;
    uint8_t bit;
} PinMapping;

extern const PinMapping pins[];

#define digitalPinToBitMask(pin)                   (1 << (pins[pin].bit))
#define digitalPinToPort(pin)                      (pins[pin].port)

#endif