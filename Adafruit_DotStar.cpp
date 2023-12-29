#include "Adafruit_DotStar.h"
#if !defined(__AVR_ATtiny85__)
#include <SPI.h>
#endif

#define USE_HW_SPI 255 ///< Assigned to dataPin to indicate 'hard' SPI

Adafruit_DotStar::Adafruit_DotStar(uint16_t n, uint8_t o)
    : numLEDs(n), dataPin(USE_HW_SPI), brightness(0), pixels(nullptr),
      rOffset(o & 3), gOffset((o >> 2) & 3), bOffset((o >> 4) & 3) {
  updateLength(n);
}

Adafruit_DotStar::Adafruit_DotStar(uint16_t n, uint8_t data, uint8_t clock,
                                   uint8_t o)
    : dataPin(data), clockPin(clock), brightness(0), pixels(nullptr),
      rOffset(o & 3), gOffset((o >> 2) & 3), bOffset((o >> 4) & 3) {
  updateLength(n);
}

Adafruit_DotStar::~Adafruit_DotStar(void) {
  free(pixels);
  if (dataPin == USE_HW_SPI)
    hw_spi_end();
  else
    sw_spi_end();
}

void Adafruit_DotStar::begin(void) {
  if (dataPin == USE_HW_SPI)
    hw_spi_init();
  else
    sw_spi_init();
}

void Adafruit_DotStar::updatePins(void) {
  sw_spi_end();
  dataPin = USE_HW_SPI;
  hw_spi_init();
}

void Adafruit_DotStar::updatePins(uint8_t data, uint8_t clock) {
  hw_spi_end();
  dataPin = data;
  clockPin = clock;
  sw_spi_init();
}

void Adafruit_DotStar::updateLength(uint16_t n) {
  free(pixels);
  uint16_t bytes = (rOffset == gOffset) ? n + ((n + 3) / 4) : n * 3;
  if ((pixels = (uint8_t *)malloc(bytes))) {
    numLEDs = n;
    clear();
  } else {
    numLEDs = 0;
  }
}

void Adafruit_DotStar::hw_spi_init(void) {
#ifdef __AVR_ATtiny85__
  PORTB &= ~(_BV(PORTB1) | _BV(PORTB2));
  DDRB |= _BV(PORTB1) | _BV(PORTB2);
#elif (SPI_INTERFACES_COUNT > 0) || !defined(SPI_INTERFACES_COUNT)
  SPI.begin();
#if defined(__AVR__) || defined(CORE_TEENSY) || defined(__ARDUINO_ARC__) ||    \
    defined(__ARDUINO_X86__)
  SPI.setClockDivider(SPI_CLOCK_DIV2);
#else
#ifdef ESP8266
  SPI.setFrequency(8000000L);
#elif defined(PIC32)
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  SPI.endTransaction();
#else
  SPI.setClockDivider((F_CPU + 4000000L) / 8000000L);
#endif
#endif
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
#endif
}

void Adafruit_DotStar::hw_spi_end(void) {
#ifdef __AVR_ATtiny85__
  DDRB &= ~(_BV(PORTB1) | _BV(PORTB2));
#elif (SPI_INTERFACES_COUNT > 0) || !defined(SPI_INTERFACES_COUNT)
  SPI.end();
#endif
}

void Adafruit_DotStar::sw_spi_init(void) {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
#ifdef __AVR__
  dataPort = portOutputRegister(digitalPinToPort(dataPin));
  clockPort = portOutputRegister(digitalPinToPort(clockPin));
  dataPinMask = digitalPinToBitMask(dataPin);
  clockPinMask = digitalPinToBitMask(clockPin);
  *dataPort &= ~dataPinMask;
  *clockPort &= ~clockPinMask;
#else
  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, LOW);
#endif
}

void Adafruit_DotStar::sw_spi_end() {
  pinMode(dataPin, INPUT);
  pinMode(clockPin, INPUT);
}

#ifdef __AVR_ATtiny85__
#define SPIBIT                                                                 \
  USICR = ((1 << USIWM0) | (1 << USITC));                                      \
  USICR =                                                                      \
      ((1 << USIWM0) | (1 << USITC) | (1 << USICLK));
static void spi_out(uint8_t n) {
  USIDR = n;
  SPIBIT SPIBIT SPIBIT SPIBIT SPIBIT SPIBIT SPIBIT SPIBIT
}
#elif (SPI_INTERFACES_COUNT > 0) || !defined(SPI_INTERFACES_COUNT)
#define spi_out(n) (void)SPI.transfer(n)
#endif

void Adafruit_DotStar::sw_spi_out(uint8_t n) {
  for (uint8_t i = 8; i--; n <<= 1) {
#ifdef __AVR__
    if (n & 0x80)
      *dataPort |= dataPinMask;
    else
      *dataPort &= ~dataPinMask;
    *clockPort |= clockPinMask;
    *clockPort &= ~clockPinMask;
#else
    if (n & 0x80)
      digitalWrite(dataPin, HIGH);
    else
      digitalWrite(dataPin, LOW);
    digitalWrite(clockPin
