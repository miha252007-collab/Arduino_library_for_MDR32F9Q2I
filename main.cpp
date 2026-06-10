#include "MDR32F9Q2I.h"
#include <stdint.h>
#include "core_cm3.h"                  
#include "Arduino.h"

int main( void ){
	init();
	////taktirovanie

	pinMode(A0,OUTPUT);
	while (1) {
        digitalWrite(A0, HIGH);
        delay(1000);
        digitalWrite(A0, LOW);
        delay(1000);
    }
}



 // CLK 
//	uint32_t x = DWT->CYCCNT;
// MDR_RST_CLK -> PER_CLOCK |= ( 1 << 21 );
 // PIN PA5 configuration ( HL3 )
// MDR_PORTA -> OE |= ( 1 << 5 );
// MDR_PORTA -> FUNC &= ~( 0x3 << 10 ); 
// MDR_PORTA -> ANALOG |= ( 1 << 5 );
// MDR_PORTA -> PULL &= ~( 1 << 5 ); 
// MDR_PORTA -> PULL &= ~( 1 << ( 5 + 16 ) );
// MDR_PORTA -> PD &= ~( 1 << ( 5 + 16 ) );
// MDR_PORTA -> PD &= ~( 1 << 5 ); 
// MDR_PORTA -> PWR |= ( 0x3 << 10 );//vkluchit v pinmode
// MDR_PORTA -> GFEN &= ~(1 << 5);

//uint8_t flag = 0;
// while(1){
//	 if (flag==0){
//		 flag=1;
//	 digitalWrite(5, HIGH);
//	 } else{ digitalWrite(5, LOW);
//		 flag=0;
//	 }
//	 for(int i =0; i<1133*1000;i++);
// }
