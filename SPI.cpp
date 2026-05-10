#include "SPI1.h"

SPIClass SPI;

static SPISettings current_settings(4000000, MSBFIRST, SPI_MODE0);
uint8_t SPIClass::initialized = 0;
uint8_t SPIClass::currentBitOrder = MSBFIRST;

//#ifdef SPI_TRANSACTION_MISMATCH_LED
//uint8_t SPIClass::inTransactionFlag = 0;
//#endif



void SPIClass::begin()
{
	uint32_t Old_PRIMASK = __get_PRIMASK();
	__disable_irq();
	
	if (!initialized) {

		MDR_RST_CLK -> PER_CLOCK |= (1 <<  8); // SSP1 on

		// hclk settings
		MDR_RST_CLK->SSP_CLOCK &= ~(0xFF << 0);
    MDR_RST_CLK->SSP_CLOCK |= (1 << 24) | (0 << 0); // SSP1_CLK_EN | DIV=1

		pinMode(SS, OUTPUT);
		digitalWrite(SS, HIGH);
		
		pinMode(SCK, OUTPUT);
    pinMode(MOSI, OUTPUT);//ne rabotaet dlia alternativnoy func
    pinMode(MISO, INPUT);
		
		digitalPinFuncAlternative(SCK);
		digitalPinFuncAlternative(MOSI);
		digitalPinFuncAlternative(MISO);
		
		// hclk settings
//		MDR_RST_CLK -> SSP_CLOCK &= ~(0xFF << 0);
//		MDR_RST_CLK -> SSP_CLOCK |= (1 << 24) | (0 << 0);   // SSP1 enable | // HCLK(without divider HCLK)
		
    MDR_SSP1 -> CR1 = 0;
    MDR_SSP1 -> CR0 =
            (0x7 << 0) |  // 8 bit Data Word size
            (0 << 4) |  // SPI Motorola 
            (0 << 6) |  // SPO S334 v arduino CPOL
            (0 << 7) |  // SPH v arduino CPHA
            (0 << 8);   // SCR without diver HCLK
						
    MDR_SSP1->CR1 = (1 << 1); // SSE = 1
	}
	
	initialized++;
	
	__set_PRIMASK(Old_PRIMASK);

}


void SPIClass::end()
{
		uint32_t Old_PRIMASK = __get_PRIMASK();
	__disable_irq();
	
	if (initialized){
		initialized--;
		if (!initialized){
			MDR_SSP1 -> CR1 &= ~(1 << 1); // SSE off
			MDR_RST_CLK -> PER_CLOCK &= ~(1 << 8);// SPI1 off
			}
		}
	__set_PRIMASK(Old_PRIMASK);
}

void SPIClass::usingInterrupt(uint8_t interruptNumber)
{
}

void SPIClass::notUsingInterrupt(uint8_t interruptNumber)
{
}

void SPIClass::digitalPinFuncAlternative(uint8_t pin)
{ 
		uint32_t pos = pins[pin].bit * 2;
    digitalPinToPort(pin)->FUNC &= ~(0x3 << pos);
    digitalPinToPort(pin)->FUNC |=  (0x2 << pos);
}