#include "pins_arduino.h"

#define Number_Bit_In_Port_0                                  0
#define Number_Bit_In_Port_1                                  1
#define Number_Bit_In_Port_2                                  2
#define Number_Bit_In_Port_3                                  3
#define Number_Bit_In_Port_4                                  4
#define Number_Bit_In_Port_5                                  5
#define Number_Bit_In_Port_6                                  6
#define Number_Bit_In_Port_7                                  7

const PinMapping pins[] = {
    { MDR_PORTB, Number_Bit_In_Port_6 },  // 0    
    { MDR_PORTB, Number_Bit_In_Port_5 },  // 1   
    { MDR_PORTB, Number_Bit_In_Port_1 },  // 2
    { MDR_PORTB, Number_Bit_In_Port_2 },  // 3
    { MDR_PORTB, Number_Bit_In_Port_0 },  // 4
    { MDR_PORTA, Number_Bit_In_Port_5 },  // 5
    { MDR_PORTA, Number_Bit_In_Port_4 },  // 6  
    { MDR_PORTA, Number_Bit_In_Port_7 },  // 7
    { MDR_PORTA, Number_Bit_In_Port_6 },  // 8
    { MDR_PORTA, Number_Bit_In_Port_3 },  // 9
    { MDR_PORTA, Number_Bit_In_Port_2 },  // 10
    { MDR_PORTA, Number_Bit_In_Port_1 },  // 11
    { MDR_PORTF, Number_Bit_In_Port_3 },  // 12 MISO
    { MDR_PORTF, Number_Bit_In_Port_1 },  // 13 SCK zadaet taktoviy signal
    { MDR_PORTD, Number_Bit_In_Port_2 },  // A0
    { MDR_PORTD, Number_Bit_In_Port_3 },  // A1
    { MDR_PORTD, Number_Bit_In_Port_4 },  // A2
    { MDR_PORTD, Number_Bit_In_Port_5 },  // A3
    { MDR_PORTD, Number_Bit_In_Port_6 },  // A4
    { MDR_PORTD, Number_Bit_In_Port_7 },  // A5
		{ MDR_PORTF, Number_Bit_In_Port_2 },  // SS vibor podkluchonnogo ustroistva	XP2.1 20 libo 5
 		{ MDR_PORTF, Number_Bit_In_Port_0 },  // MOSI HL4  21 zamknut s PA2
};