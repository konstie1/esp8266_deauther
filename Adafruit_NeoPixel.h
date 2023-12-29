#ifndef ADAFRUIT_NEOPIXEL_H
#define ADAFRUIT_NEOPIXEL_H

#ifdef ARDUINO
  #if (ARDUINO >= 100)
  #include <Arduino.h>
  #else
  #include <WProgram.h>
  #include <pins_arduino.h>
  #endif
#endif

#ifdef TARGET_LPC1768
  #include <Arduino.h>
#endif

#define NEO_RGB  ((0<<6) | (0<<4) | (1<<2) | (2))
#define NEO_RBG  ((0<<6) | (0<<4) | (2<<2) | (1))
#define NEO_GRB  ((1<<6) | (1<<4) | (0<<2) | (2))
#define NEO_GBR  ((2<<6) | (2<<4) | (0<<2) | (1))
#define NEO_BRG  ((1<<6) | (1<<4) | (2<<2) | (0))
#define NEO_BGR  ((2<<6) | (2<<4) | (1<<2) | (0))

#define NEO_WRGB ((0<<6) | (1<<4) | (2<<2) | (3))
#define NEO_WRBG ((0<<6) | (1<<4) | (3<<2) | (2))
#define NEO_WGRB ((0<<6) | (2<<4) | (1<<2) | (3))
#define NEO_WGBR ((0<<6) | (3<<4) | (1<<2) | (2))
#define NEO_WBRG ((0<<6) | (2<<4) | (3<<2) | (1))
#define NEO_WBGR ((0<<6) | (3<<4) | (2<<2) | (1))

#define NEO_RWGB ((1<<6) | (0<<4) | (2<<2) | (3))
#define NEO_RWBG ((1<<6) | (0<<4) | (3<<2) | (2))
#define NEO_RGWB ((2<<6) | (0<<4) | (1<<2) | (3))
#define NEO_RGBW ((3<<6) | (0<<4) | (1<<2) | (2))
#define NEO_RBWG ((2<<6) | (0<<4) | (3<<2) | (1))
#define NEO_RBGW ((3<<6) | (0<<4) | (2<<2) | (1))

#define NEO_GWRB ((1<<6) | (2<<4) | (0<<2) | (3))
#define NEO_GWBR ((1<<6) | (3<<4) | (0<<2) | (2))
#define NEO_GRWB ((2<<6) | (1<<4) | (0<<2) | (3))
#define NEO_GRBW ((3<<6) | (1<<4) | (0<<2) | (2))
#define NEO_GBWR ((2<<6) | (3<<4) | (0<<2) | (1))
#define NEO_GBRW ((3<<6) | (2<<4) | (0<<2) | (1))

#define NEO_BWRG ((1<<6) | (2<<4) | (3<<2) | (0))
#define NEO_BWGR ((1<<6) | (3<<4) | (2<<2) | (0))
#define NEO_BRWG ((2<<6) | (1<<4) | (3<<2) | (0))
#define NEO_BRGW ((3<<6) | (1<<4) | (2<<2) | (0))
#define NEO_BGWR ((2<<6) | (3<<4) | (1<<2) | (0))
#define NEO_BGRW ((3<<6) | (2<<4) | (1<<2) | (0))

#define NEO_KHZ800 0x0000
#ifndef __AVR_ATtiny85__
#define NEO_KHZ400 0x0100
#endif

#ifdef NEO_KHZ400
typedef uint16_t neoPixelType;
#else
typedef uint8_t  neoPixelType;
#endif

static const uint8_t PROGMEM _NeoPixelSineTable[256] = {
  // Содержание таблицы опущено для экономии места
};

static const uint8_t PROGMEM _NeoPixelGammaTable[256] = {
  // Содержание таблицы опущено для экономии места
};

class Adafruit_NeoPixel {

 public:

  Adafruit_NeoPixel(uint16_t n, uint16_t pin=6, neoPixelType type=NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel(void);
  ~Adafruit_NeoPixel();

  void              begin(void);
  void              show(void);
  void              setPin(uint16_t p);
  void              setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
  void              setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
  void              setPixelColor(uint16_t n, uint32_t c);
  void              fill(uint32_t c=0, uint16_t first=0, uint16_t count=0);
  void              setBrightness(uint8_t);
  void              clear(void);
  void              updateLength(uint16_t n);
  void              updateType(neoPixelType t);
  bool canShow(void) {
    if (endTime > micros()) {
      endTime = micros();
    }
    return (micros() - endTime) >= 300L;
  }
  uint8_t *getPixels(void) const { return pixels; };
  uint8_t getBrightness(void) const;
  int16_t getPin(void) const { return pin; };
  uint16_t numPixels(void) const { return numLEDs; }
  uint32_t getPixelColor(uint16_t n) const;
  static uint8_t sine8(uint8_t x) {
    return pgm_read_byte(&_NeoPixelSineTable[x]);
  }
  static uint8_t gamma8(uint8_t x) {
    return pgm_read_byte(&_NeoPixelGammaTable[x]);
  }
  static uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
  }
  static uint32_t Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    return ((uint32_t)w << 24) | ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
  }
  static uint32_t ColorHSV(uint16_t hue, uint8_t sat=255, uint8_t val=255);
  static uint32_t gamma32(uint32_t x);

 private:

  const uint16_t    numLEDs;       // Суммарное количество светодиодов в ленте
  int16_t           pin;           // Номер вывода микроконтроллера, к которому подключены светодиоды
  uint8_t           brightness;
  uint8_t           *pixels;       // Массив цветов светодиодов (3 байта на каждый светодиод)
  uint8_t           rOffset;       // Смещение для красного цвета
  uint8_t           gOffset;       // Смещение для зеленого цвета
  uint8_t           bOffset;       // Смещение для синего цвета
  uint8_t           wOffset;       // Смещение для белого цвета (если используется)
  uint8_t           rgbw;          // Соотношение между RGB и W в цветовой модели
  neoPixelType      stripType;     // Тип ленты (GRB, RGB, RGBW, и т.д.)
  volatile uint32_t endTime;        // Время завершения последнего вывода данных на ленту

};

#endif // ADAFRUIT_NEOPIXEL_H
