#include "Arduino.h"
#include "MDR32F9Q2I.h"
#include "core_cm3.h"
#include <stdint.h>

void SystemClock_Config(void){
	MDR_RST_CLK->HS_CONTROL |= (1<<0);
	
	while (!(MDR_RST_CLK->CLOCK_STATUS & (1 << 2))); // HSE ready
	
	MDR_RST_CLK->PLL_CONTROL =
        (0x1 << 8) |   // MUL = 2 (x2)
        (1 << 2);    // PLL ON
	
	while (!(MDR_RST_CLK->CLOCK_STATUS & (1 << 1))); // PLL ready
	
	MDR_RST_CLK -> CPU_CLOCK = 
				(0x2 <<0) | //CPU_C1 = HSE
				(1 << 2)  | // CPU_C2 = PLL
				(1 << 8); // HCLK=CPU_C3 enable
	
////	SystemCoreClockUpdate();
	SystemCoreClock = 16000000UL;
}





void yield(void);

__attribute__((weak)) void yield(void){
}


//void cli(void){//Clear Interrupts
//	__disable_irq()
//}// kak v arduino
//void sei(void){
//    __enable_irq();
//}


#define SysTick_CTRL_COUNTFLAG_Bit_In_Register          (1 << 16)      
#define SysTick_CTRL_COUNTFLAG_Status                   ((SysTick-> CTRL) & SysTick_CTRL_COUNTFLAG_Bit_In_Register)
#define SysTick_VAL_Timer_Value                         (SysTick -> VAL)       

volatile uint32_t timer_millis = 0;

void SysTick_Init(void){
//	NVIC_SetPriority(SysTick_IRQn, 0);
  SysTick -> LOAD = SystemCoreClock / 500UL - 1; //default value = 8000000//
  SysTick -> VAL  = 0;//obnulenie timera, ochis	henie COUNTFLAG
	
//	NVIC_SetPriority(SysTick_IRQn, 0);
//	NVIC_EnableIRQ(SysTick_IRQn);
	
  SysTick -> CTRL =
      SysTick_CTRL_CLKSOURCE_Msk |  SysTick_CTRL_TICKINT_Msk   | SysTick_CTRL_ENABLE_Msk;     
				// CPU clock(HCLK Taktorovaie) | interrupt enable(vkluchenie prerivaniya) | start(razreshaet rabotu timera)	
}

//__STATIC_INLINE uint32_t SysTick_Config(uint32_t ticks)
//{
//  if ((ticks - 1) > SysTick_LOAD_RELOAD_Msk)  return (1);      /* Reload value impossible */

//  SysTick->LOAD  = ticks - 1;                                  /* set reload register */
//  NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority for Systick Interrupt */
//  SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
//  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
//                   SysTick_CTRL_TICKINT_Msk   |
//                   SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */
//  return (0);                                                  /* Function successful */
//}


void SysTick_Handler(void){
	timer_millis ++;
}



uint32_t millis(void){
	uint32_t millis_now;
	int32_t Old_PRIMASK = __get_PRIMASK();
	__disable_irq();
	
	millis_now = timer_millis;
	
	__set_PRIMASK(Old_PRIMASK);//void s85 dokumentazii
	return millis_now;
}


//uint32_t micros(void){
//    return timer_millis * 1000UL;
//}
uint32_t micros(void)
{
    uint32_t ms;
    uint32_t ticks;

    do {
        ms = timer_millis;
        ticks = SysTick->VAL;
    }
    while (ms != timer_millis);

    uint32_t reload = SysTick->LOAD + 1;

    return (ms * 1000UL) +
           ((reload - ticks) * 1000UL) / reload;
}
//uint32_t micros(void){
//	uint32_t millis_now, ticks;
////	uint32_t Old_PRIMASK = __get_PRIMASK();
//	
////	 __disable_irq();
//	
//	millis_now = timer_millis;
//	ticks = SysTick_VAL_Timer_Value;
//	uint32_t ctrl = SysTick -> CTRL; //SysTick_CTRL_COUNTFLAG_Status
//	
//	//proverka perepolneniy
//	if ((ctrl & SysTick_CTRL_COUNTFLAG_Msk)&& (ticks > 0)) {
//        millis_now +=1;
//    }
//		
////	__set_PRIMASK(Old_PRIMASK);
//	uint32_t reload = SysTick -> LOAD + 1;
//  uint32_t elapsed = reload - ticks;

//  // perevod chastoti v ms
//  return (millis_now * 1000UL) + (elapsed * 1000UL) / reload;	
//}


void delay(uint32_t ms){
	uint32_t start = micros(); //we will know time now(tochnost - microsekundi)
	
	while (ms > 0) {
		yield();
		while ( ms > 0 && (micros() - start) >= 1000) {
			ms --;
			start += 1000;
		}
	}
}


//void delayMicroseconds(uint32_t us){
//	//pri SystemCoreClock >= 1000000
//	uint32_t ticks_needed = us * (SystemCoreClock / 1000000UL);
//  uint32_t ticks_passed = 0;

//  uint32_t reload = SysTick->LOAD + 1;
//  uint32_t number_last_cycle = SysTick -> VAL;
//	
//	while (ticks_passed < ticks_needed){
//		uint32_t number_current_cycle = SysTick->VAL;
//		
//		if (number_last_cycle >= number_current_cycle){// bez perepolneniya
//			ticks_passed += (number_last_cycle - number_current_cycle);
//			
//    }else{// s perepolneniem
//      ticks_passed += (number_last_cycle + (reload - number_current_cycle));
//		}
//		number_last_cycle = number_current_cycle;
//   }
//}
void delayMicroseconds(uint32_t us)
{
    uint32_t start = micros();

    while ((uint32_t)(micros() - start) < us);
}

void init(void){
//	__enable_irq() // est v yadre arduino
//	SystemInit();     // taktirovanie yadra sbros
  SystemClock_Config(); // taktirovanie yadra on
  SysTick_Init();   //delay(vse timeri)
	__enable_irq();
	
}
