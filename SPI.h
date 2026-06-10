#ifndef SPI_H
#define SPI_H
#include <stdint.h>
#include <stddef.h>
#include "MDR32F9Q2I.h"
#include "Arduino.h"

#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3

#define MSBFIRST 0
#define LSBFIRST 1


#define SPI_CLOCK_DIV2 		2
#define SPI_CLOCK_DIV4 		4
#define SPI_CLOCK_DIV8 		8
#define SPI_CLOCK_DIV16 	16
#define SPI_CLOCK_DIV32 	32
#define SPI_CLOCK_DIV64 	64
#define SPI_CLOCK_DIV128	128



class SPISettings{
	public:
		//peredelat 
		SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {
			if (__builtin_constant_p(clock)) {
				init_AlwaysInline(clock, bitOrder, dataMode);
			} else {
				init_MightInline(clock, bitOrder, dataMode);
			}
    }
		SPISettings() {
			init_AlwaysInline(4000000, MSBFIRST, SPI_MODE0);
		}
	
private:
	void init_MightInline(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {
    init_AlwaysInline(clock, bitOrder, dataMode);
  }

  void init_AlwaysInline(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)
    __attribute__((__always_inline__)) {

    uint8_t clockDiv;

    if (__builtin_constant_p(clock)) { // __builtin_constant_p - provarka na zadanost peremennoi
      if (clock >= SystemCoreClock / 2) {
        clockDiv = 2;
      } else if (clock >= SystemCoreClock / 4) {
        clockDiv = 4;
      } else if (clock >= SystemCoreClock / 8) {
        clockDiv = 8;
      } else if (clock >= SystemCoreClock / 16) {
        clockDiv = 16;
      } else if (clock >= SystemCoreClock / 32) {
        clockDiv = 32;
      } else if (clock >= SystemCoreClock / 64) {
        clockDiv = 64;
      } else {
        clockDiv = 128;
      }
    } else {
      uint32_t clockSetting = SystemCoreClock / 2;
      clockDiv = 0;
      while (clockDiv < 6 && clock < clockSetting) {
        clockSetting /= 2;
        clockDiv++;
      }
    }

    // Pack into the SPISettings class
    SPI_Clock_Diver_CPSDVR = clockDiv;
		cpol = (dataMode & 0x2) ? (1 << 6) : 0;
		cpha = (dataMode & 0x1) ? (1 << 7) : 0;
		MDR_SSP1_cr0 = (0 << 8) | cpha | cpol | (0x7 << 0);
		currentBitOrder = bitOrder;
		}
  
	//dopisat ostalnie polia classa
	uint8_t SPI_Clock_Diver_CPSDVR;	
  uint32_t cpol;
  uint32_t cpha;
  uint32_t MDR_SSP1_cr0;
	uint8_t currentBitOrder;

		
  friend class SPIClass;
};




class SPIClass{
	public:
		// Initialize the SPI library
		static void begin();
		
		
	
	
	// If SPI is used from within an interrupt, this function registers
  // that interrupt with the SPI library, so beginTransaction() can
  // prevent conflicts.  The input interruptNumber is the number used
  // with attachInterrupt.  If SPI is used from a different interrupt
  // (eg, a timer), interruptNumber should be 255.
	static void usingInterrupt(uint8_t interruptNumber);
	// And this does the opposite.
	static void notUsingInterrupt(uint8_t interruptNumber);
	// Note: the usingInterrupt and notUsingInterrupt functions should
  // not to be called from ISR context or inside a transaction.
  // For details see:
  // https://github.com/arduino/Arduino/pull/2381
  // https://github.com/arduino/Arduino/pull/2449

  // Before using SPI.transfer() or asserting chip select pins,
  // this function is used to gain exclusive access to the SPI bus
  // and configure the correct settings.
	
	inline static void beginTransaction(SPISettings settings){
		if (interruptMode > 0) {
			uint32_t Old_PRIMASK = __get_PRIMASK();
			__disable_irq();

      if (interruptMode == 1) {
				
				interruptSave_0 = NVIC -> ICER[0] & (1 << EXT_INT1_IRQn);//old status interrupt 
				interruptSave_1 = NVIC -> ICER[0] & (1 << EXT_INT2_IRQn);
				interruptSave_2 = NVIC -> ICER[0] & (1 << EXT_INT4_IRQn);
				
        if (interrupt_0_status){
					NVIC_DisableIRQ(EXT_INT1_IRQn);
					//NVIC_ClearPendingIRQ(EXT_INT1_IRQn); //dlia chego?
				}
				 if (interrupt_1_status){
					NVIC_DisableIRQ(EXT_INT2_IRQn);
				}
				 if (interrupt_2_status){
					NVIC_DisableIRQ(EXT_INT4_IRQn);
				}
        __set_PRIMASK(Old_PRIMASK);
      }//ELSE
    }
		
		
		current_settings = settings;
		
    #ifdef SPI_TRANSACTION_MISMATCH_LED
    if (inTransactionFlag) {
      pinMode(SPI_TRANSACTION_MISMATCH_LED, OUTPUT);
      digitalWrite(SPI_TRANSACTION_MISMATCH_LED, HIGH);
    }
    inTransactionFlag = 1;
    #endif
		
    while (MDR_SSP1 -> SR & (1 << 4)) {
		}

    MDR_SSP1 -> CR1 &= ~(1 << 1); // SSE = 0

    MDR_SSP1 -> CPSR = settings.SPI_Clock_Diver_CPSDVR; 
    MDR_SSP1 -> CR0  = settings.MDR_SSP1_cr0 ;// nuzno dobavit ochistku

    MDR_SSP1 -> CR1 |= (1 << 1); // SSE = 1
		
		currentBitOrder = settings.currentBitOrder;//pomeniat
	}
	
	
	// Write to the SPI bus (MOSI pin) and also receive (MISO pin)
	
	inline static uint8_t transfer(uint8_t data){
		if (currentBitOrder == LSBFIRST) {
			data = reverse_bits(data);
		}
	
		while ( MDR_SSP1 -> SR & (1 << 4) );// zdem poka FIFO osvoboditsia	
		MDR_SSP1 -> DR = data;//otpravka dannih
		
		__NOP();
		
		while ( MDR_SSP1 -> SR & (1<<4) );// zdem poka FIFO primet dannie
		uint8_t result = (uint8_t)MDR_SSP1->DR;
		
		if (currentBitOrder == LSBFIRST) {
			result = reverse_bits(result);
		}
		
		return result; // poluchenie dannih
		/*
			* The following NOP introduces a small delay that can prevent the wait
			* loop form iterating when running at the maximum speed. This gives
			* about 10% more speed, even if it seems counter-intuitive. At lower
			* speeds it is unnoticed.
			*/
	}
	
	inline static uint16_t transfer16(uint16_t data){
		
		if (currentBitOrder == LSBFIRST) {
			data = reverse_bits16(data);
		}
		
		while (MDR_SSP1->SR & (1 << 4));
		MDR_SSP1 -> CR1 &= ~(1 << 1); // SSE = 0
		uint32_t OLD_CRO = MDR_SSP1 -> CR0;
		MDR_SSP1 -> CR0 |= (0xF << 0);
		MDR_SSP1 -> CR1 |= (1 << 1); // SSE = 1
	
		MDR_SSP1 -> DR = data;
		
		__NOP();
		while ( MDR_SSP1->SR & (1 << 4) );
		uint16_t result = (uint16_t)MDR_SSP1->DR;
		
		MDR_SSP1 -> CR1 &= ~(1 << 1); // SSE = 0	
		MDR_SSP1 -> CR0 = OLD_CRO;
		MDR_SSP1 -> CR1 |= (1 << 1); // SSE = 1
		
		if (currentBitOrder == LSBFIRST) result = reverse_bits16(result);
		return result;
	}
	
	inline static void transfer(void *buf, size_t count){
		if (count == 0) return;
	
		uint8_t *data_buffer = (uint8_t *)buf;
		size_t sent_count_bytes = 0;
		size_t received_count_bytes = 0;
	
		while (received_count_bytes < count) {

			//proverka na nalichie mesta v FIFO + zapis
			while ((MDR_SSP1 -> SR & (1 << 1)) && (sent_count_bytes < count)) {
				MDR_SSP1 -> DR = data_buffer[sent_count_bytes];
				sent_count_bytes++;
			}
			//proverka na nalichie dannih v FIFO + zapis
			while ((MDR_SSP1 -> SR & (1 << 2)) && (received_count_bytes < count)) {
				data_buffer[received_count_bytes] = (uint8_t)MDR_SSP1 -> DR;
				received_count_bytes++;
			}
		}
	}
	
	// After performing a group of transfers and releasing the chip select
  // signal, this function allows others to access the SPI bus
	inline static void endTransaction(void){
		//__enable_irq()
		
		
		#ifdef SPI_TRANSACTION_MISMATCH_LED
    if (!inTransactionFlag){
      pinMode(SPI_TRANSACTION_MISMATCH_LED, OUTPUT);
      digitalWrite(SPI_TRANSACTION_MISMATCH_LED, HIGH);
    }
    inTransactionFlag = 0;
    #endif
		
		//prerivaniya
		  if (interruptMode > 0) {
					uint32_t Old_PRIMASK = __get_PRIMASK();
					__disable_irq();

				if (interruptMode == 1) {
					if (interruptSave_0){
						NVIC -> ICER[0] |= (interruptSave_0 << EXT_INT1_IRQn);
					}
					if (interruptSave_1){
						NVIC -> ICER[0] |= (interruptSave_1 << EXT_INT2_IRQn);
					}
					if (interruptSave_2){
						NVIC -> ICER[0] |= (interruptSave_2 << EXT_INT4_IRQn);
					}
				}
				__set_PRIMASK(Old_PRIMASK);
			}
	}
	
	static void end();

	//	 inline static void setBitOrder(uint8_t bitOrder) {
//    if (bitOrder == LSBFIRST) SPCR |= _BV(DORD);
//    else SPCR &= ~(_BV(DORD));
//  }
	
	 // This function is deprecated.  New applications should use
  // beginTransaction() to configure SPI settings.
  inline static void setBitOrder(uint8_t bitOrder) {
		currentBitOrder = bitOrder;
		
//    if (bitOrder == LSBFIRST) currentBitOrder = LSBFIRST; //old realization
//    else currentBitOrder = MSBFIRST;
  }
	
  // This function is deprecated.  New applications should use
  // beginTransaction() to configure SPI settings.
  inline static void setDataMode(uint8_t dataMode) {
		
    while (MDR_SSP1 -> SR & (1 << 4)) {
		} // wait BSY = 0
		
		MDR_SSP1->CR1 &= ~(1 << 1); // SSE = 0
		
    uint32_t cpol = (dataMode & 0x2) ? (1 << 6) : 0;
		uint32_t cpha = (dataMode & 0x1) ? (1 << 7) : 0;
		
		MDR_SSP1->CR0 = (MDR_SSP1->CR0 & ~0xC0) | (cpol | cpha);
		
		MDR_SSP1 -> CR1 |= (1 << 1); // SSE = 1
  }
	
  // This function is deprecated.  New applications should use
  // beginTransaction() to configure SPI settings.
  inline static void setClockDivider(uint8_t clockDiv) {
    while (MDR_SSP1 -> SR & (1 << 4)) {
		}
    MDR_SSP1 -> CR1 &= ~(1 << 1); // SSE = 0
    MDR_SSP1 -> CPSR = clockDiv; 
    MDR_SSP1 -> CR1 |= (1 << 1); // SSE = 1
		
  }
	
  // These undocumented functions should not be used.  SPI.transfer()
  // polls the hardware flag which is automatically cleared as the
  // AVR responds to SPI's interrupt
  inline static void attachInterrupt();
  inline static void detachInterrupt();
	
	private:
		static SPISettings current_settings;
		static void digitalPinFuncAlternative(uint8_t pin);
		static uint8_t initialized;
		static uint8_t currentBitOrder;
	
		static uint8_t interruptMode;
		static uint32_t interruptSave_0;
		static uint32_t interruptSave_1;
		static uint32_t interruptSave_2;
		static uint8_t interrupt_0_status;
		static uint8_t interrupt_1_status;
		static uint8_t interrupt_2_status;
	
		static uint8_t reverse_bits(uint8_t b) {
			b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
			b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
			b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
			return b;
		}
		
		static uint16_t reverse_bits16(uint16_t b) {
			b = (b & 0xAAAA) >> 1 | (b & 0x5555) << 1;
			b = (b & 0xCCCC) >> 2 | (b & 0x3333) << 2;
			b = (b & 0xF0F0) >> 4 | (b & 0x0F0F) << 4;
			b = (b & 0xFF00) >> 8 | (b & 0x00FF) << 8;
			return b;
		}
	
	
		#ifdef SPI_TRANSACTION_MISMATCH_LED
		static uint8_t inTransactionFlag;
		#endif
};

extern SPIClass SPI;

#endif
