#include "analog.h"
#include "pins_arduino.h"
#include "MDR32F9Q2I.h"

// ADC1_CFG bits
#define ADON    (1 << 0)
#define GO      (1 << 1)
#define CHS     (0x1F << 4)
#define CHCH    (1 << 9)
#define M_REF   (1 << 11)
#define DIVCLK  (0xF << 12)

// ADC1_STATUS bits
#define EOCIF   (1 << 2)

// PER_CLOCK bits
#define PER_CLOCK_ADC     (1 << 17)
#define PER_CLOCK_PORTD   (1 << 24)

#define ADC_MCO_EN        (1 << 13)

#define ADC_MAX_HZ        14000000UL
#define TIMEOUT           100000UL

typedef struct {
    uint8_t arduino_pin;
    uint8_t adc_channel;
} AnalogPin;

static const AnalogPin analog_pins[NUM_ANALOG_PINS] = {
    {A0, 2},   // A0 = PD2 = ADC1_CH2
    {A1, 3},   // A1 = PD3 = ADC1_CH3
    {A2, 4},   // A2 = PD4 = ADC1_CH4
    {A3, 5},   // A3 = PD5 = ADC1_CH5
    {A4, 6},   // A4 = PD6 = ADC1_CH6
    {A5, 7},   // A5 = PD7 = ADC1_CH7
    {A6, 0},   // A6 = PD0 = ADC1_CH0
    {A7, 1},   // A7 = PD1 = ADC1_CH1
};

static uint8_t analog_resolution = 10;
static uint8_t analog_reference  = DEFAULT;
static uint8_t adc_initialized   = 0;

static void delay_us(uint32_t us)
{
    volatile uint32_t n = us * (SystemCoreClock / 1000000UL / 4);
    while (n--) { __asm__ volatile ("nop"); }
}

static const AnalogPin* findAnalogPin(uint8_t pin)
{
    uint8_t i;
    for (i = 0; i < NUM_ANALOG_PINS; i++) {
        if (analog_pins[i].arduino_pin == pin) return &analog_pins[i];
    }
    return 0;
}

static void adcClockInit(void)
{
    uint32_t sel;
    uint32_t adc_clk = SystemCoreClock;

    if      (adc_clk / 1UL  <= ADC_MAX_HZ) sel = 0x0;
    else if (adc_clk / 2UL  <= ADC_MAX_HZ) sel = 0x8;
    else if (adc_clk / 4UL  <= ADC_MAX_HZ) sel = 0x9;
    else if (adc_clk / 8UL  <= ADC_MAX_HZ) sel = 0xA;
    else if (adc_clk / 16UL <= ADC_MAX_HZ) sel = 0xB;
    else if (adc_clk / 32UL <= ADC_MAX_HZ) sel = 0xC;
    else if (adc_clk / 64UL <= ADC_MAX_HZ) sel = 0xD;
    else if (adc_clk /128UL <= ADC_MAX_HZ) sel = 0xE;
    else                                     sel = 0xF;

    MDR_RST_CLK->ADC_MCO_CLOCK = ADC_MCO_EN
                               | (sel << 8)
                               | (2 << 4)
                               | (2 << 0);
}

static void adcInit(void)
{
    if (adc_initialized) return;

    MDR_RST_CLK->PER_CLOCK |= (PER_CLOCK_ADC | PER_CLOCK_PORTD);
    adcClockInit();

    MDR_ADC->ADC1_CFG = (1 << 2);   // CLKS=1: use ADC_CLK
    MDR_ADC->ADC1_CFG |= ADON;       // power on
    delay_us(10);

    adc_initialized = 1;
}

static void pinAnalogMode(volatile MDR_PORT_TypeDef *port, uint8_t bit)
{
    port->FUNC   &= ~(3 << (bit * 2));
    port->OE     &= ~(1 << bit);
    port->ANALOG &= ~(1 << bit);
    port->PD     &= ~(1 << bit);
    port->PWR    &= ~(3 << (bit * 2));
}

void analogReadResolution(uint8_t bits)
{
    if (bits < 1)  bits = 1;
    if (bits > 32) bits = 32;
    analog_resolution = bits;
}

void analogReference(uint8_t type)
{
    analog_reference = type;
}

int analogRead(uint8_t pin)
{
    const AnalogPin *ap;
    uint32_t cfg, raw;
    uint32_t t;

    ap = findAnalogPin(pin);
    if (!ap) return 0;

    adcInit();

    pinAnalogMode(pins[ap->arduino_pin].port, pins[ap->arduino_pin].bit);

    cfg = MDR_ADC->ADC1_CFG;
    cfg &= ~(CHS | CHCH | M_REF);
    cfg |= (ap->adc_channel << 4);
    if (analog_reference == EXTERNAL) cfg |= M_REF;
    MDR_ADC->ADC1_CFG = cfg;

    MDR_ADC->ADC1_CFG |= GO;

    for (t = TIMEOUT; t > 0; t--) {
        if (MDR_ADC->ADC1_STATUS & EOCIF) break;
    }
    if (t == 0) return 0;

    raw = MDR_ADC->ADC1_RESULT & 0xFFF;

    if (analog_resolution < 12)      return raw >> (12 - analog_resolution);
    else if (analog_resolution > 12) return raw << (analog_resolution - 12);
    else                             return raw;
}