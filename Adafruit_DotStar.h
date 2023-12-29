#ifndef _ADAFRUIT_DOT_STAR_H_
#define _ADAFRUIT_DOT_STAR_H_

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#include <pins_arduino.h>
#endif

#define DOTSTAR_RGB (0 | (1 << 2) | (2 << 4)) ///< Передавати як R, G, B
#define DOTSTAR_RBG (0 | (2 << 2) | (1 << 4)) ///< Передавати як R, B, G
#define DOTSTAR_GRB (1 | (0 << 2) | (2 << 4)) ///< Передавати як G, R, B
#define DOTSTAR_GBR (2 | (0 << 2) | (1 << 4)) ///< Передавати як G, B, R
#define DOTSTAR_BRG (1 | (2 << 2) | (0 << 4)) ///< Передавати як B, R, G
#define DOTSTAR_BGR (2 | (1 << 2) | (0 << 4)) ///< Передавати як B, G, R
#define DOTSTAR_MONO 0 ///< Однокольорова стрічка РОЗРОБКА НЕ ВИКОРИСТОВУВАТИ, використовуйте RGB наразі

static const uint8_t PROGMEM _DotStarSineTable[256] = {
    // Таблиця синусів (0-255)
    // ...
};

static const uint8_t PROGMEM _DotStarGammaTable[256] = {
    // Таблиця гамма-корекції (0-255)
    // ...
};

class Adafruit_DotStar {

public:
  Adafruit_DotStar(uint16_t n, uint8_t o = DOTSTAR_BRG);
  Adafruit_DotStar(uint16_t n, uint8_t d, uint8_t c, uint8_t o = DOTSTAR_BRG);
  ~Adafruit_DotStar(void);

  void begin(void);
  void show(void);
  void setPixelColor(uint16_t n, uint32_t c);
  void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
  void fill(uint32_t c = 0, uint16_t first = 0, uint16_t count = 0);
  void setBrightness(uint8_t);
  void clear();
  void updateLength(uint16_t n);
  void updatePins(void);
  void updatePins(uint8_t d, uint8_t c);
  uint8_t *getPixels(void) const { return pixels; };
  uint8_t getBrightness(void) const;
  uint16_t numPixels(void) const { return numLEDs; };
  uint32_t getPixelColor(uint16_t n) const;
  static uint8_t sine8(uint8_t x);
  static uint8_t gamma8(uint8_t x);
  static uint32_t Color(uint8_t r, uint8_t g, uint8_t b);
  static uint32_t ColorHSV(uint16_t hue, uint8_t sat = 255, uint8_t val = 255);
  static uint32_t gamma32(uint32_t x);

private:
  // Приватні члени класу
  // ...

  void hw_spi_init(void);
  void hw_spi_end(void);
  void sw_spi_init(void);
  void sw_spi_out(uint8_t n);
  void sw_spi_end(void);
};

#endif // _ADAFRUIT_DOT_STAR_H_
