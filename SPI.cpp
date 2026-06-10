
#include "SPI.h"

SPIClass SPI;

static SPISettings current_settings(4000000, MSBFIRST, SPI_MODE0);
uint8_t SPIClass::initialized = 0;
uint8_t SPIClass::currentBitOrder = MSBFIRST;

//interrupt
uint8_t SPIClass::interruptMode = 0;
uint32_t SPIClass::interruptSave_0 = 0;
uint32_t SPIClass::interruptSave_1 = 0;
uint32_t SPIClass::interruptSave_2 = 0;
uint8_t SPIClass::interrupt_0_status = 0;
uint8_t SPIClass::interrupt_1_status = 0;
uint8_t SPIClass::interrupt_2_status = 0;

#define SPI_INT0_MASK (1 << 0)
#define SPI_INT1_MASK (1 << 1)
#define SPI_INT2_MASK (1 << 2 )

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
		MDR_RST_CLK -> SSP_CLOCK &= ~(0xFF << 0);
    MDR_RST_CLK -> SSP_CLOCK |= (1 << 24) | (0 << 0); // SSP1_CLK_EN | DIV=1

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
		
//    MDR_SSP1 -> CR1 = 0;
//    MDR_SSP1 -> CR0 =
//            (0x7 << 0) |  // 8 bit Data Word size
//            (0 << 4) |  // SPI Motorola 
//            (0 << 6) |  // SPO S334 v arduino CPOL
//            (0 << 7) |  // SPH v arduino CPHA
//            (0 << 8);   // SCR without diver HCLK
		
		MDR_SSP1 -> CR1 &= ~(1 << 1);
    MDR_SSP1 -> CR0  = (0 << 8) | (0x7 << 0);
    MDR_SSP1 -> CR1 |= (1 << 1); // SSE = 1
	
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
	uint32_t Old_PRIMASK = __get_PRIMASK();
	__disable_irq();
	
	
  switch (interruptNumber) {
  case 0: interrupt_0_status = 1; break;
  case 1: interrupt_1_status = 1; break;
  case 2: interrupt_2_status = 1; break;
	default:
    interruptMode = 2;
    break;
  }
	
  if (!interruptMode)
    interruptMode = 1;
  __set_PRIMASK(Old_PRIMASK);
}

void SPIClass::notUsingInterrupt(uint8_t interruptNumber)
{
	// Once in mode 2 we can't go back to 0 without a proper reference count
  if (interruptMode == 2)
    return;
	
  	uint32_t Old_PRIMASK = __get_PRIMASK();
	__disable_irq();
	
  switch (interruptNumber) {
  case 0: interrupt_0_status = 0; break;
  case 1: interrupt_1_status = 0; break;
  case 2: interrupt_2_status = 0; break;
  default:
    break;
    // this case can't be reached
  }
	
  if (!(interrupt_0_status | interrupt_1_status | interrupt_2_status) )
    interruptMode = 0;
  __set_PRIMASK(Old_PRIMASK);
}

void SPIClass::digitalPinFuncAlternative(uint8_t pin)
{ 
		uint32_t pos = pins[pin].bit * 2;
    digitalPinToPort(pin)->FUNC &= ~(0x3 << pos);
    digitalPinToPort(pin)->FUNC |=  (0x2 << pos);
}
