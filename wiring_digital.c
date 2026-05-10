#include "Arduino.h"
#include "pins_arduino.h"
#include "MDR32F9Q2I.h"
#include <stdint.h>

//proverit vse registri na oshibki esli oni sistoyat is 2 bitov

#define digitalPinToBitMask_32(pin)                (1 << ((pins[pin].bit) + 16))    
#define digitalPinValueLOW(pin)                    digitalPinToPort(pin)->RXTX &= ~digitalPinToBitMask(pin)
#define digitalPinValueHIGH(pin)                   digitalPinToPort(pin)->RXTX |= digitalPinToBitMask(pin)
#define digitalPinModeOUTPUT(pin)                  digitalPinToPort(pin)->OE |= digitalPinToBitMask(pin)
#define digitalPinModeINPUT(pin)                   digitalPinToPort(pin)->OE &= ~digitalPinToBitMask(pin)
#define digitalPinValueStatus(pin)                 ((digitalPinToPort(pin)->RXTX) & digitalPinToBitMask(pin))
#define digitalPinPullUpON(pin)                    digitalPinToPort(pin) -> PULL |= digitalPinToBitMask_32(pin)
#define digitalPinPullUpOFF(pin)                   digitalPinToPort(pin) -> PULL &= ~digitalPinToBitMask_32(pin)
#define digitalPinPullDownON(pin)                  digitalPinToPort(pin) -> PULL |= digitalPinToBitMask(pin)
#define digitalPinPullDownOFF(pin)                 digitalPinToPort(pin) -> PULL &= ~digitalPinToBitMask(pin)
#define digitalPinFuncPort(pin)                    digitalPinToPort(pin) -> FUNC &= ~( 0x3 << (pins[pin].bit)*2 )
#define digitalPinAnalogToDigitalMode(pin)         digitalPinToPort(pin) -> ANALOG |= digitalPinToBitMask(pin)
#define digitalPinPDSchmittTriggerOFF(pin)         digitalPinToPort(pin) -> PD &= ~digitalPinToBitMask(pin)
#define digitalPinPDmanagedDriverMode(pin)         digitalPinToPort(pin) -> PD &= ~digitalPinToBitMask_32(pin)
#define digitalPinPWRfastestPossibleFront(pin)     digitalPinToPort(pin) -> PWR |= ( 0x3 << (pins[pin].bit)*2 )
#define digitalPinGFENfilterOFF(pin)               digitalPinToPort(pin) -> GFEN &= ~digitalPinToBitMask(pin)

void enabling_port(	volatile MDR_PORT_TypeDef *port){
	if (port == MDR_PORTA) MDR_RST_CLK -> PER_CLOCK |= (1 << 21);
	if (port == MDR_PORTB) MDR_RST_CLK -> PER_CLOCK |= (1 << 22);
	if (port == MDR_PORTD) MDR_RST_CLK -> PER_CLOCK |= (1 << 24);
	if (port == MDR_PORTF) MDR_RST_CLK -> PER_CLOCK |= (1 << 29);

}



//enum PinMode { INPUT = 0x0, OUTPUT = 0x1, INPUT_PULLUP = 0x2 };

void pinMode(uint8_t pin, uint8_t mode){
		enabling_port(pins[pin].port);
	
		digitalPinFuncPort(pin); 
		digitalPinAnalogToDigitalMode(pin);
		digitalPinPDmanagedDriverMode(pin);
		digitalPinPDSchmittTriggerOFF(pin);
		digitalPinPWRfastestPossibleFront(pin);
		digitalPinGFENfilterOFF(pin);
	
    if (mode == OUTPUT){
			  uint32_t Old_PRIMASK = __get_PRIMASK();//zapisivaem status vseh prerivaniy c uchetom prioriteta
				__disable_irq();//otkluchenie vseh prerivaniy
			
        digitalPinModeOUTPUT(pin);
        digitalPinPullDownOFF(pin);
        digitalPinPullUpOFF(pin);
			
				__set_PRIMASK(Old_PRIMASK);// vozvrashem status vseh prerivaniy
    } else if (mode == INPUT){
				uint32_t Old_PRIMASK = __get_PRIMASK();
				__disable_irq();
			
        digitalPinModeINPUT(pin);
        digitalPinPullDownOFF(pin);
        digitalPinPullUpOFF(pin);
			
			  __set_PRIMASK(Old_PRIMASK);
    } else {  // INPUT_PULLUP
				uint32_t Old_PRIMASK = __get_PRIMASK();
				__disable_irq();
			
        digitalPinModeINPUT(pin);
        digitalPinPullUpON(pin);
        digitalPinPullDownOFF(pin);
			
				__set_PRIMASK(Old_PRIMASK);
    }
}

//enum DigitalWrite{ LOW = 0x0, HIGH = 0x1 }; 

void digitalWrite(uint8_t pin, uint8_t val){
		
		uint32_t Old_PRIMASK = __get_PRIMASK();
		__disable_irq();
	
    if (val == HIGH){
        digitalPinValueHIGH(pin);
    } else {
        digitalPinValueLOW(pin);
    }
		
		__set_PRIMASK(Old_PRIMASK);
}


int digitalRead(uint8_t pin){
    if (digitalPinValueStatus(pin)) { return HIGH; }
    return LOW;
}
