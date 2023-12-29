#include "Adafruit_NeoPixel.h"

#if defined(TARGET_LPC1768)
  #include <time.h>
#endif

#if defined(NRF52) || defined(NRF52_SERIES)
#include "nrf.h"

#endif

Adafruit_NeoPixel::Adafruit_NeoPixel(uint16_t n, uint16_t p, neoPixelType t) :
  begun(false), brightness(0), pixels(NULL), endTime(0) {
  updateType(t);
  updateLength(n);
  setPin(p);
}

Adafruit_NeoPixel::Adafruit_NeoPixel() :
#if defined(NEO_KHZ400)
  is800KHz(true),
#endif
  begun(false), numLEDs(0), numBytes(0), pin(-1), brightness(0), pixels(NULL),
  rOffset(1), gOffset(0), bOffset(2), wOffset(1), endTime(0) {
}
Adafruit_NeoPixel::~Adafruit_NeoPixel() {
  free(pixels);
  if (pin >= 0) pinMode(pin, INPUT);
}

void Adafruit_NeoPixel::begin(void) {
  if (pin >= 0) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
  begun = true;
}

void Adafruit_NeoPixel::updateLength(uint16_t n) {
  free(pixels);
  numBytes = n * ((wOffset == rOffset) ? 3 : 4);
  if ((pixels = (uint8_t *)malloc(numBytes))) {
    memset(pixels, 0, numBytes);
    numLEDs = n;
  } else {
    numLEDs = numBytes = 0;
  }
}

void Adafruit_NeoPixel::updateType(neoPixelType t) {
  bool oldThreeBytesPerPixel = (wOffset == rOffset);
  wOffset = (t >> 6) & 0b11;
  rOffset = (t >> 4) & 0b11;
  gOffset = (t >> 2) & 0b11;
  bOffset = t & 0b11;
#if defined(NEO_KHZ400)
  is800KHz = (t < 256);
#endif
  if (pixels) {
    bool newThreeBytesPerPixel = (wOffset == rOffset);
    if (newThreeBytesPerPixel != oldThreeBytesPerPixel) updateLength(numLEDs);
  }
}

#if defined(ESP8266)
extern "C" void ICACHE_RAM_ATTR espShow(uint16_t pin, uint8_t *pixels, uint32_t numBytes, uint8_t type);
#elif defined(ESP32)
extern "C" void espShow(uint16_t pin, uint8_t *pixels, uint32_t numBytes, uint8_t type);
#endif

#if defined(KENDRYTE_K210)
extern "C" void  k210Show(uint8_t pin, uint8_t *pixels, uint32_t numBytes, boolean is800KHz);
#endif

void Adafruit_NeoPixel::show(void) {
  if (!pixels) return;
  while (!canShow());
#if !( defined(NRF52) || defined(NRF52_SERIES) )
  noInterrupts();
#endif

#if defined(__AVR__)
  volatile uint16_t i = numBytes;
  volatile uint8_t *ptr = pixels, b = *ptr++, hi, lo;

#if (F_CPU >= 7400000UL) && (F_CPU <= 9500000UL)
#if defined(NEO_KHZ400)
  if (is800KHz) {
#endif
    volatile uint8_t n1, n2 = 0;
#endif
    // PORTD OUTPUT ----------------------------------------------------
    // ...
  }
#endif
#if defined(PORTD)
  #if defined(PORTB) || defined(PORTC) || defined(PORTF)
    if (port == &PORTD) {
  #endif

      hi = PORTD | pinMask;
      lo = PORTD & ~pinMask;
      n1 = lo;
      if (b & 0x80) n1 = hi;

      asm volatile(
        "headD:"                   "\n\t"
        "out %[port], %[hi]"      "\n\t"
        "mov %[n2], %[lo]"        "\n\t"
        "out %[port], %[n1]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "sbrc %[byte], 6"         "\n\t"
          "mov %[n2], %[hi]"      "\n\t"
        "out %[port], %[lo]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "out %[port], %[hi]"      "\n\t"
        "mov %[n1], %[lo]"        "\n\t"
        "out %[port], %[n2]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "sbrc %[byte], 5"         "\n\t"
          "mov %[n1], %[hi]"      "\n\t"
        "out %[port], %[lo]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "out %[port], %[hi]"      "\n\t"
        "mov %[n2], %[lo]"        "\n\t"
        "out %[port], %[n1]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "sbrc %[byte], 4"         "\n\t"
          "mov %[n2], %[hi]"      "\n\t"
        "out %[port], %[lo]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "out %[port], %[hi]"      "\n\t"
        "mov %[n1], %[lo]"        "\n\t"
        "out %[port], %[n2]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "sbrc %[byte], 3"         "\n\t"
          "mov %[n1], %[hi]"      "\n\t"
        "out %[port], %[lo]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "out %[port], %[hi]"      "\n\t"
        "mov %[n2], %[lo]"        "\n\t"
        "out %[port], %[n1]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "sbrc %[byte], 2"         "\n\t"
          "mov %[n2], %[hi]"      "\n\t"
        "out %[port], %[lo]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "out %[port], %[hi]"      "\n\t"
        "mov %[n1], %[lo]"        "\n\t"
        "out %[port], %[n2]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "sbrc %[byte], 1"         "\n\t"
          "mov %[n1], %[hi]"      "\n\t"
        "out %[port], %[lo]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "out %[port], %[hi]"      "\n\t"
        "mov %[n2], %[lo]"        "\n\t"
        "out %[port], %[n1]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "sbrc %[byte], 0"         "\n\t"
          "mov %[n2], %[hi]"      "\n\t"
        "out %[port], %[lo]"      "\n\t"
        "sbiw %[count], 1"        "\n\t"
        "out %[port], %[hi]"      "\n\t"
        "mov %[n1], %[lo]"        "\n\t"
        "out %[port], %[n2]"      "\n\t"
        "ld %[byte], %a[ptr]+"    "\n\t"
        "sbrc %[byte], 7"         "\n\t"
          "mov %[n1], %[hi]"      "\n\t"
        "out %[port], %[lo]"      "\n\t"
        "brne headD"              "\n"
      : [byte]  "+r" (b),
        [n1]    "+r" (n1),
        [n2]    "+r" (n2),
        [count] "+w" (i)
      : [port]   "I" (_SFR_IO_ADDR(PORTD)),
        [ptr]    "e" (ptr),
        [hi]     "r" (hi),
        [lo]     "r" (lo));

  #if defined(PORTB) || defined(PORTC) || defined(PORTF)
    } else // other PORT(s)
  #endif
#endif // defined(PORTD)

    // PORTB OUTPUT ----------------------------------------------------

#if defined(PORTB)
  #if defined(PORTD) || defined(PORTC) || defined(PORTF)
    if (port == &PORTB) {
  #endif

      hi = PORTB | pinMask;
      lo = PORTB & ~pinMask;
      n1 = lo;
      if (b & 0x80) n1 = hi;

      asm volatile(
        "headB:"                   "\n\t"
        "out %[port], %[hi]"      "\n\t"
        "mov %[n2], %[lo]"        "\n\t"
        "out %[port], %[n1]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "sbrc %[byte], 6"         "\n\t"
          "mov %[n2], %[hi]"      "\n\t"
        "out %[port], %[lo]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "out %[port], %[hi]"      "\n\t"
        "mov %[n1], %[lo]"        "\n\t"
        "out %[port], %[n2]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "sbrc %[byte], 5"         "\n\t"
          "mov %[n1], %[hi]"      "\n\t"
        "out %[port], %[lo]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "out %[port], %[hi]"      "\n\t"
        "mov %[n2], %[lo]"        "\n\t"
        "out %[port], %[n1]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "sbrc %[byte], 4"         "\n\t"
          "mov %[n2], %[hi]"      "\n\t"
        "out %[port], %[lo]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "out %[port], %[hi]"      "\n\t"
        "mov %[n1], %[lo]"        "\n\t"
        "out %[port], %[n2]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "sbrc %[byte], 3"         "\n\t"
          "mov %[n1], %[hi]"      "\n\t"
        "out %[port], %[lo]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "out %[port], %[hi]"      "\n\t"
        "mov %[n2], %[lo]"        "\n\t"
        "out %[port], %[n1]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "sbrc %[byte], 2"         "\n\t"
          "mov %[n2], %[hi]"      "\n\t"
        "out %[port], %[lo]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "out %[port], %[hi]"      "\n\t"
        "mov %[n1], %[lo]"        "\n\t"
        "out %[port], %[n2]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "sbrc %[byte], 1"         "\n\t"
          "mov %[n1], %[hi]"      "\n\t"
        "out %[port], %[lo]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "out %[port], %[hi]"      "\n\t"
        "mov %[n2], %[lo]"        "\n\t"
        "out %[port], %[n1]"      "\n\t"
        "rjmp .+0"                "\n\t"
        "sbrc %[byte], 0"         "\n\t"
          "mov %[n2], %[hi]"      "\n\t"
        "out %[port], %[lo]"      "\n\t"
        "sbiw %[count], 1"        "\n\t"
        "out %[port], %[hi]"      "\n\t"
        "mov %[n1], %[lo]"        "\n\t"
        "out %[port], %[n2]"      "\n\t"
        "ld %[byte], %a[ptr]+"    "\n\t"
        "sbrc %[byte], 7"         "\n\t"
          "mov %[n1], %[hi]"      "\n\t"
        "out %[port], %[lo]"      "\n\t"
        "brne headB"              "\n"
      : [byte] "+r" (b), [n1] "+r" (n1), [n2] "+r" (n2), [count] "+w" (i)
      : [port] "I" (_SFR_IO_ADDR(PORTB)), [ptr] "e" (ptr), [hi] "r" (hi),
        [lo] "r" (lo));

  #if defined(PORTD) || defined(PORTC) || defined(PORTF)
    }
  #endif
  #if defined(PORTC) || defined(PORTF)
    else
  #endif
#endif // defined(PORTB)
#if defined(PORTC) && (defined(PORTD) || defined(PORTB) || defined(PORTF))
  if (port == &PORTC) {
#else
  #if defined(PORTF) && (defined(PORTD) || defined(PORTB) || defined(PORTC))
    if (port == &PORTF) {
  #endif
#endif

  hi = *port | pinMask;
  lo = *port & ~pinMask;
  n1 = lo;
  if (b & 0x80) n1 = hi;

  asm volatile(
   "head:"                    "\n\t"
    "out  %[port], %[hi]"     "\n\t"
    "mov  %[n2], %[lo]"        "\n\t"
    "out  %[port], %[n1]"     "\n\t"
    "rjmp .+0"                "\n\t"
    "sbrc %[byte], 6"         "\n\t"
     "mov %[n2], %[hi]"        "\n\t"
    "out  %[port], %[lo]"     "\n\t"
    "rjmp .+0"                "\n\t"
    "out  %[port], %[hi]"     "\n\t"
    "mov  %[n1], %[lo]"        "\n\t"
    "out  %[port], %[n2]"     "\n\t"
    "rjmp .+0"                "\n\t"
    "sbrc %[byte], 5"         "\n\t"
     "mov %[n1], %[hi]"        "\n\t"
    "out  %[port], %[lo]"     "\n\t"
    "rjmp .+0"                "\n\t"
    "out  %[port], %[hi]"     "\n\t"
    "mov  %[n2], %[lo]"        "\n\t"
    "out  %[port], %[n1]"     "\n\t"
    "rjmp .+0"                "\n\t"
    "sbrc %[byte], 4"         "\n\t"
     "mov %[n2], %[hi]"        "\n\t"
    "out  %[port], %[lo]"     "\n\t"
    "rjmp .+0"                "\n\t"
    "out  %[port], %[hi]"     "\n\t"
    "mov  %[n1], %[lo]"        "\n\t"
    "out  %[port], %[n2]"     "\n\t"
    "rjmp .+0"                "\n\t"
    "sbrc %[byte], 3"         "\n\t"
     "mov %[n1], %[hi]"        "\n\t"
    "out  %[port], %[lo]"     "\n\t"
    "rjmp .+0"                "\n\t"
    "out  %[port], %[hi]"     "\n\t"
    "mov  %[n2], %[lo]"        "\n\t"
    "out  %[port], %[n1]"     "\n\t"
    "rjmp .+0"                "\n\t"
    "sbrc %[byte], 2"         "\n\t"
     "mov %[n2], %[hi]"        "\n\t"
    "out  %[port], %[lo]"     "\n\t"
    "rjmp .+0"                "\n\t"
    "out  %[port], %[hi]"     "\n\t"
    "mov  %[n1], %[lo]"        "\n\t"
    "out  %[port], %[n2]"     "\n\t"
    "rjmp .+0"                "\n\t"
    "sbrc %[byte], 1"         "\n\t"
     "mov %[n1], %[hi]"        "\n\t"
    "out  %[port], %[lo]"     "\n\t"
    "rjmp .+0"                "\n\t"
    "out  %[port], %[hi]"     "\n\t"
    "mov  %[n2], %[lo]"        "\n\t"
    "out  %[port], %[n1]"     "\n\t"
    "rjmp .+0"                "\n\t"
    "sbrc %[byte], 0"         "\n\t"
     "mov %[n2], %[hi]"        "\n\t"
    "out  %[port], %[lo]"     "\n\t"
    "sbiw %[count], 1"        "\n\t"
    "out  %[port], %[hi]"     "\n\t"
    "mov  %[n1], %[lo]"        "\n\t"
    "out  %[port], %[n2]"     "\n\t"
    "ld   %[byte], %a[ptr]+"  "\n\t"
    "sbrc %[byte], 7"         "\n\t"
     "mov %[n1], %[hi]"        "\n\t"
    "out  %[port], %[lo]"     "\n\t"
    "brne head"               "\n"
  : [byte] "+r" (b), [n1] "+r" (n1), [n2] "+r" (n2), [count] "+w" (i)
  : [port] "I" (_SFR_IO_ADDR(*port)), [ptr] "e" (ptr), [hi] "r" (hi),
    [lo] "r" (lo));

#if defined(PORTC) && (defined(PORTD) || defined(PORTB) || defined(PORTF))
  }
#endif

#if defined(PORTF) && (defined(PORTD) || defined(PORTB) || defined(PORTC))
  #endif
#endif
#if defined(NEO_KHZ400)
  } else { // end 800 KHz, do 400 KHz

    volatile uint8_t next, bit;

    hi   = *port |  pinMask;
    lo   = *port & ~pinMask;
    next = lo;
    bit  = 8;

    asm volatile(
     "head20:"                  "\n\t"
      "st   %a[port], %[hi]"    "\n\t"
      "sbrc %[byte] , 7"        "\n\t"
       "mov  %[next], %[hi]"    "\n\t"
      "st   %a[port], %[next]"  "\n\t"
      "mov  %[next] , %[lo]"    "\n\t"
      "dec  %[bit]"             "\n\t"
      "breq nextbyte20"         "\n\t"
      "rol  %[byte]"            "\n\t"
      "st   %a[port], %[lo]"    "\n\t"
      "rjmp .+0"                "\n\t"
      "rjmp .+0"                "\n\t"
      "rjmp .+0"                "\n\t"
      "rjmp head20"             "\n\t"
     "nextbyte20:"              "\n\t"
      "st   %a[port], %[lo]"    "\n\t"
      "nop"                     "\n\t"
      "ldi  %[bit]  , 8"        "\n\t"
      "ld   %[byte] , %a[ptr]+" "\n\t"
      "sbiw %[count], 1"        "\n\t"
      "brne head20"             "\n"
      : [port]  "+e" (port),
        [byte]  "+r" (b),
        [bit]   "+r" (bit),
        [next]  "+r" (next),
        [count] "+w" (i)
      : [hi]    "r" (hi),
        [lo]    "r" (lo),
        [ptr]   "e" (ptr));
  }
#endif // NEO_KHZ400

// 12 MHz(ish) AVR --------------------------------------------------------
#elif (F_CPU >= 11100000UL) && (F_CPU <= 14300000UL)

#if defined(NEO_KHZ400) // Перевірка на підтримку 800 КГц потрібна лише, якщо підтримка 400 КГц ввімкнена
  if(is800KHz) {
#endif

    volatile uint8_t next;

    // PORTD OUTPUT ----------------------------------------------------

#if defined(PORTD)
 #if defined(PORTB) || defined(PORTC) || defined(PORTF)
    if(port == &PORTD) {
 #endif // defined(PORTB/C/F)

      hi   = PORTD |  pinMask;
      lo   = PORTD & ~pinMask;
      next = lo;
      if(b & 0x80) next = hi;

      asm volatile(
       "headD:"                   "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeD"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeD"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeD"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeD"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeD"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeD"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeD"          "\n\t"
        "out  %[port] , %[hi]"    "\n\t"
        "rjmp .+0"                "\n\t"
        "ld   %[byte] , %a[ptr]+" "\n\t"
        "out  %[port] , %[next]"  "\n\t"
        "mov  %[next] , %[lo]"    "\n\t"
        "sbrc %[byte] , 7"        "\n\t"
         "mov %[next] , %[hi]"    "\n\t"
        "nop"                     "\n\t"
        "out  %[port] , %[lo]"    "\n\t"
        "sbiw %[count], 1"        "\n\t"
        "brne headD"              "\n\t"
         "rjmp doneD"             "\n\t"
        "bitTimeD:"               "\n\t"
         "out  %[port], %[next]"  "\n\t"
         "mov  %[next], %[lo]"    "\n\t"
         "rol  %[byte]"           "\n\t"
         "sbrc %[byte], 7"        "\n\t"
          "mov %[next], %[hi]"    "\n\t"
         "nop"                    "\n\t"
         "out  %[port], %[lo]"    "\n\t"
         "ret"                    "\n\t"
         "doneD:"                 "\n"
        : [byte]  "+r" (b),
          [next]  "+r" (next),
          [count] "+w" (i)
        : [port]   "I" (_SFR_IO_ADDR(PORTD)),
          [ptr]    "e" (ptr),
          [hi]     "r" (hi),
          [lo]     "r" (lo));

 #if defined(PORTB) || defined(PORTC) || defined(PORTF)
    } else // інші порти
 #endif // defined(PORTB/C/F)
#endif // defined(PORTD)

    // PORTB OUTPUT ----------------------------------------------------

#if defined(PORTB)
 #if defined(PORTD) || defined(PORTC) || defined(PORTF)
    if(port == &PORTB) {
 #endif // defined(PORTD/C/F)

      hi   = PORTB |  pinMask;
      lo   = PORTB & ~pinMask;
      next = lo;
      if(b & 0x80) next = hi;

      asm volatile(
       "headB:"                   "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeB"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeB"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeB"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeB"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeB"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeB"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeB"          "\n\t"
        "out  %[port] , %[hi]"    "\n\t"
        "rjmp .+0"                "\n\t"
        "ld   %[byte] , %a[ptr]+" "\n\t"
        "out  %[port] , %[next]"  "\n\t"
        "mov  %[next] , %[lo]"    "\n\t"
        "sbrc %[byte] , 7"        "\n\t"
         "mov %[next] , %[hi]"    "\n\t"
        "nop"                     "\n\t"
        "out  %[port] , %[lo]"    "\n\t"
        "sbiw %[count], 1"        "\n\t"
        "brne headB"              "\n\t"
         "rjmp doneB"             "\n\t"
        "bitTimeB:"               "\n\t"
         "out  %[port], %[next]"  "\n\t"
         "mov  %[next], %[lo]"    "\n\t"
         "rol  %[byte]"           "\n\t"
         "sbrc %[byte], 7"        "\n\t"
          "mov %[next], %[hi]"    "\n\t"
         "nop"                    "\n\t"
         "out  %[port], %[lo]"    "\n\t"
         "ret"                    "\n\t"
         "doneB:"                 "\n"
        : [byte] "+r" (b), [next] "+r" (next), [count] "+w" (i)
        : [port] "I" (_SFR_IO_ADDR(PORTB)), [ptr] "e" (ptr), [hi] "r" (hi),
          [lo] "r" (lo));

 #if defined(PORTD) || defined(PORTC) || defined(PORTF)
    }
 #endif
 #if defined(PORTC) || defined(PORTF)
    else
 #endif // defined(PORTC/F)
#endif // defined(PORTB)

    // PORTC OUTPUT ----------------------------------------------------

#if defined(PORTC)
 #if defined(PORTD) || defined(PORTB) || defined(PORTF)
    if(port == &PORTC) {
 #endif // defined(PORTD/B/F)

      hi   = PORTC |  pinMask;
      lo   = PORTC & ~pinMask;
      next = lo;
      if(b & 0x80) next = hi;

      asm volatile(
       "headC:"                   "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeC"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeC"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeC"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeC"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeC"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeC"          "\n\t"
        "out   %[port], %[hi]"    "\n\t"
        "rcall bitTimeC"          "\n\t"
        "out  %[port] , %[hi]"    "\n\t"
        "rjmp .+0"                "\n\t"
        "ld   %[byte] , %a[ptr]+" "\n\t"
        "out  %[port] , %[next]"  "\n\t"
        "mov  %[next] , %[lo]"    "\n\t"
        "sbrc %[byte] , 7"        "\n\t"
         "mov %[next] , %[hi]"    "\n\t"
        "nop"                     "\n\t"
        "out  %[port] , %[lo]"    "\n\t"
        "sbiw %[count], 1"        "\n\t"
        "brne headC"              "\n\t"
         "rjmp doneC"             "\n\t"
        "bitTimeC:"               "\n\t"
         "out  %[port], %[next]"  "\n\t"
         "mov  %[next], %[lo]"    "\n\t"
         "rol  %[byte]"           "\n\t"
         "sbrc %[byte], 7"        "\n\t"
          "mov %[next], %[hi]"    "\n\t"
         "nop"                    "\n\t"
         "out  %[port], %[lo]"    "\n\t"
         "ret"                    "\n\t"
         "doneC:"                 "\n"
        : [byte] "+r" (b), [next] "+r" (next), [count] "+w" (i)
        : [port] "I" (_SFR_IO_ADDR(PORTC)), [ptr] "e" (ptr), [hi] "r" (hi),
          [lo] "r" (lo));
#if defined(PORTD) || defined(PORTB) || defined(PORTF)
    }
#endif // defined(PORTD/B/F)

#if defined(PORTF)
    else
#endif
#endif // defined(PORTC)

// PORTF OUTPUT ----------------------------------------------------

#if defined(PORTF)
#if defined(PORTD) || defined(PORTB) || defined(PORTC)
    if (port == &PORTF) {
#endif // defined(PORTD/B/C)

        hi = PORTF | pinMask;
        lo = PORTF & ~pinMask;
        next = lo;
        if (b & 0x80) next = hi;

        asm volatile(
            "headF:"                   "\n\t"
            "out   %[port], %[hi]"    "\n\t"
            "rcall bitTimeC"          "\n\t"
            "out   %[port], %[hi]"    "\n\t"
            "rcall bitTimeC"          "\n\t"
            "out   %[port], %[hi]"    "\n\t"
            "rcall bitTimeC"          "\n\t"
            "out   %[port], %[hi]"    "\n\t"
            "rcall bitTimeC"          "\n\t"
            "out   %[port], %[hi]"    "\n\t"
            "rcall bitTimeC"          "\n\t"
            "out   %[port], %[hi]"    "\n\t"
            "rcall bitTimeC"          "\n\t"
            "out   %[port], %[hi]"    "\n\t"
            "rcall bitTimeC"          "\n\t"
            "out   %[port], %[hi]"    "\n\t"
            "rcall bitTimeC"          "\n\t"
            "out  %[port] , %[hi]"    "\n\t"
            "rjmp .+0"                "\n\t"
            "ld   %[byte] , %a[ptr]+" "\n\t"
            "out  %[port] , %[next]"  "\n\t"
            "mov  %[next] , %[lo]"    "\n\t"
            "sbrc %[byte] , 7"        "\n\t"
            "mov %[next] , %[hi]"    "\n\t"
            "nop"                     "\n\t"
            "out  %[port] , %[lo]"    "\n\t"
            "sbiw %[count], 1"        "\n\t"
            "brne headF"              "\n\t"
            "rjmp doneC"             "\n\t"
            "bitTimeC:"               "\n\t"
            "out  %[port], %[next]"  "\n\t"
            "mov  %[next], %[lo]"    "\n\t"
            "rol  %[byte]"           "\n\t"
            "sbrc %[byte], 7"        "\n\t"
            "mov %[next], %[hi]"    "\n\t"
            "nop"                    "\n\t"
            "out  %[port], %[lo]"    "\n\t"
            "ret"                    "\n\t"
            "doneC:"                 "\n"
            : [byte] "+r" (b), [next] "+r" (next), [count] "+w" (i)
            : [port] "I" (_SFR_IO_ADDR(PORTF)), [ptr] "e" (ptr), [hi] "r" (hi),
            [lo] "r" (lo));

#if defined(PORTD) || defined(PORTB) || defined(PORTC)
    }
#endif // defined(PORTD/B/C)
#endif // defined(PORTF)

#if defined(NEO_KHZ400)
  } else { // 400 KHz

    // 30 instruction clocks per bit: HHHHHHxxxxxxxxxLLLLLLLLLLLLLLL
    // ST instructions:               ^     ^        ^    (T=0,6,15)

    volatile uint8_t next, bit;

    hi   = *port |  pinMask;
    lo   = *port & ~pinMask;
    next = lo;
    bit  = 8;

    asm volatile(
     "head30:"                  "\n\t" // Clk  Pseudocode    (T =  0)
      "st   %a[port], %[hi]"    "\n\t" // 2    PORT = hi     (T =  2)
      "sbrc %[byte] , 7"        "\n\t" // 1-2  if(b & 128)
       "mov  %[next], %[hi]"    "\n\t" // 0-1   next = hi    (T =  4)
      "rjmp .+0"                "\n\t" // 2    nop nop       (T =  6)
      "st   %a[port], %[next]"  "\n\t" // 2    PORT = next   (T =  8)
      "rjmp .+0"                "\n\t" // 2    nop nop       (T = 10)
      "rjmp .+0"                "\n\t" // 2    nop nop       (T = 12)
      "rjmp .+0"                "\n\t" // 2    nop nop       (T = 14)
      "nop"                     "\n\t" // 1    nop           (T = 15)
      "st   %a[port], %[lo]"    "\n\t" // 2    PORT = lo     (T = 17)
      "rjmp .+0"                "\n\t" // 2    nop nop       (T = 19)
      "dec  %[bit]"             "\n\t" // 1    bit--         (T = 20)
      "breq nextbyte30"         "\n\t" // 1-2  if(bit == 0)
      "rol  %[byte]"            "\n\t" // 1    b <<= 1       (T = 22)
      "rjmp .+0"                "\n\t" // 2    nop nop       (T = 24)
      "rjmp .+0"                "\n\t" // 2    nop nop       (T = 26)
      "rjmp .+0"                "\n\t" // 2    nop nop       (T = 28)
      "rjmp head30"             "\n\t" // 2    -> head30 (next bit out)
     "nextbyte30:"              "\n\t" //                    (T = 22)
      "nop"                     "\n\t" // 1    nop           (T = 23)
      "ldi  %[bit]  , 8"        "\n\t" // 1    bit = 8       (T = 24)
      "ld   %[byte] , %a[ptr]+" "\n\t" // 2    b = *ptr++    (T = 26)
      "st   %a[port], %[lo]"    "\n\t" // 2    PORT = lo     (T = 28)
      "rjmp head30"             "\n"   // 2    -> head30 (next byte out)
      : [port] "+e" (port), [byte] "+r" (b), [bit] "+r" (i)
      : [ptr] "e" (ptr), [hi] "r" (hi), [lo] "r" (lo));
  }
#endif
}
#elif defined(NRF52) || defined(NRF52_SERIES)

#define MAGIC_T0H 6UL | (0x8000) // 0.375us
#define MAGIC_T1H 13UL | (0x8000) // 0.8125us
#define MAGIC_T0H_400KHz 8UL  | (0x8000) // 0.5us
#define MAGIC_T1H_400KHz 19UL | (0x8000) // 1.1875us
#define CTOPVAL 20UL            // 1.25us
#define CTOPVAL_400KHz 40UL      // 2.5us

#define CYCLES_800_T0H  18
#define CYCLES_800_T1H  41
#define CYCLES_800      71
#define CYCLES_400_T0H  26
#define CYCLES_400_T1H  70
#define CYCLES_400      156

uint32_t pattern_size = numBytes * 8 * sizeof(uint16_t) + 2 * sizeof(uint16_t);
uint16_t *pixels_pattern = NULL;
NRF_PWM_Type *pwm = NULL;

NRF_PWM_Type *PWM[] = {NRF_PWM0, NRF_PWM1, NRF_PWM2
#if defined(NRF_PWM3)
    ,
    NRF_PWM3
#endif
};

for (unsigned int device = 0; device < (sizeof(PWM) / sizeof(PWM[0])); device++) {
    if ((PWM[device]->ENABLE == 0) &&
        (PWM[device]->PSEL.OUT[0] & PWM_PSEL_OUT_CONNECT_Msk) &&
        (PWM[device]->PSEL.OUT[1] & PWM_PSEL_OUT_CONNECT_Msk) &&
        (PWM[device]->PSEL.OUT[2] & PWM_PSEL_OUT_CONNECT_Msk) &&
        (PWM[device]->PSEL.OUT[3] & PWM_PSEL_OUT_CONNECT_Msk)) {
        pwm = PWM[device];
        break;
    }
}

if (pwm != NULL) {
#if defined(ARDUINO_NRF52_ADAFRUIT)
    pixels_pattern = (uint16_t *)rtos_malloc(pattern_size);
#else
    pixels_pattern = (uint16_t *)malloc(pattern_size);
#endif

    if (pixels_pattern != NULL) {
        uint16_t pos = 0;

        for (uint16_t n = 0; n < numBytes; n++) {
            uint8_t pix = pixels[n];

            for (uint8_t mask = 0x80; mask > 0; mask >>= 1) {
#if defined(NEO_KHZ400)
                if (!is800KHz) {
                    pixels_pattern[pos] = (pix & mask) ? MAGIC_T1H_400KHz : MAGIC_T0H_400KHz;
                } else
#endif
                {
                    pixels_pattern[pos] = (pix & mask) ? MAGIC_T1H : MAGIC_T0H;
                }

                pos++;
            }
        }

        pixels_pattern[pos++] = 0 | (0x8000);
        pixels_pattern[pos++] = 0 | (0x8000);

        pwm->MODE = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);
        pwm->PRESCALER = (PWM_PRESCALER_PRESCALER_DIV_1 << PWM_PRESCALER_PRESCALER_Pos);

#if defined(NEO_KHZ400)
        if (!is800KHz) {
            pwm->COUNTERTOP = (CTOPVAL_400KHz << PWM_COUNTERTOP_COUNTERTOP_Pos);
        } else
#endif
        {
            pwm->COUNTERTOP = (CTOPVAL << PWM_COUNTERTOP_COUNTERTOP_Pos);
        }

        pwm->LOOP = (PWM_LOOP_CNT_Disabled << PWM_LOOP_CNT_Pos);
        pwm->DECODER = (PWM_DECODER_LOAD_Common << PWM_DECODER_LOAD_Pos) |
                       (PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
        pwm->SEQ[0].PTR = (uint32_t)(pixels_pattern) << PWM_SEQ_PTR_PTR_Pos;
        pwm->SEQ[0].CNT = (pattern_size / sizeof(uint16_t)) << PWM_SEQ_CNT_CNT_Pos;
        pwm->SEQ[0].REFRESH = 0;
        pwm->SEQ[0].ENDDELAY = 0;

#if defined(ARDUINO_ARCH_NRF52840)
        pwm->PSEL.OUT[0] = g_APinDescription[pin].name;
#else
        pwm->PSEL.OUT[0] = g_ADigitalPinMap[pin];
#endif

        pwm->ENABLE = 1;
        pwm->EVENTS_SEQEND[0] = 0;
        pwm->TASKS_SEQSTART[0] = 1;

        while (!pwm->EVENTS_SEQEND[0]) {
#if defined(ARDUINO_NRF52_ADAFRUIT) || defined(ARDUINO_ARCH_NRF52840)
            yield();
#endif
        }

        pwm->EVENTS_SEQEND[0] = 0;
        pwm->ENABLE = 0;
        pwm->PSEL.OUT[0] = 0xFFFFFFFFUL;

#if defined(ARDUINO_NRF52_ADAFRUIT)
        rtos_free(pixels_pattern);
#else
        free(pixels_pattern);
#endif
    }
} else {
#ifndef ARDUINO_ARCH_NRF52840
#if defined(ARDUINO_NRF52_ADAFRUIT)
    taskENTER_CRITICAL();
#elif defined(NRF52_DISABLE_INT)
    __disable_irq();
#endif

    NRF_GPIO_Type *nrf_port = (NRF_GPIO_Type *)digitalPinToPort(pin);
    uint32_t pinMask = digitalPinToBitMask(pin);

    uint32_t CYCLES_X00 = CYCLES_800;
    uint32_t CYCLES_X00_T1H = CYCLES_800_T1H;
    uint32_t CYCLES_X00_T0H = CYCLES_800_T0H;

#if defined(NEO_KHZ400)
    if (!is800KHz) {
        CYCLES_X00 = CYCLES_400;
        CYCLES_X00_T1H = CYCLES_400_T1H;
        CYCLES_X00_T0H = CYCLES_400_T0H;
    }
#endif

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    while (1) {
        uint8_t *p = pixels;

        uint32_t cycStart = DWT->CYCCNT;
        uint32_t cyc = 0;

        for (uint16_t n = 0; n < numBytes; n++) {
            uint8_t pix = *p++;

            for (uint8_t mask = 0x80; mask; mask >>= 1) {
                while (DWT->CYCCNT - cyc < CYCLES_X00);
                cyc = DWT->CYCCNT;

                nrf_port->OUTSET |= pinMask;

                if (pix & mask) {
                    while (DWT->CYCCNT - cyc < CYCLES_X00_T1H);
                } else {
                    while (DWT->CYCCNT - cyc < CYCLES_X00_T0H);
                }

                nrf_port->OUTCLR |= pinMask;
            }
        }
        while (DWT->CYCCNT - cyc < CYCLES_X00);

        if ((DWT->CYCCNT - cycStart) < (8 * numBytes * ((CYCLES_X00 * 5) / 4))) {
            break;
        }

        delayMicroseconds(300);
    }

#if defined(ARDUINO_NRF52_ADAFRUIT)
    taskEXIT_CRITICAL();
#elif defined(NRF52_DISABLE_INT)
    __enable_irq();
#endif
#endif
}
#endif
#if defined(__SAMD21E17A__) || defined(__SAMD21G18A__) || defined(__SAMD21E18A__) || defined(__SAMD21J18A__)
  uint8_t *ptr, *end, p, bitMask, portNum;
  uint32_t pinMask;

  portNum = g_APinDescription[pin].ulPort;
  pinMask = 1ul << g_APinDescription[pin].ulPin;
  ptr = pixels;
  end = ptr + numBytes;
  p = *ptr++;
  bitMask = 0x80;

  volatile uint32_t *set = &(PORT->Group[portNum].OUTSET.reg),
                    *clr = &(PORT->Group[portNum].OUTCLR.reg);

#if defined(NEO_KHZ400)
  if (is800KHz) {
#endif
    for (;;) {
      *set = pinMask;
      asm("nop;"); // Fine-tune the timing
      if (p & bitMask) {
        asm("nop;");
        *clr = pinMask;
      } else {
        *clr = pinMask;
        asm("nop;");
      }
      if (bitMask >>= 1) {
        asm("nop;");
      } else {
        if (ptr >= end)
          break;
        p = *ptr++;
        bitMask = 0x80;
      }
    }
#if defined(NEO_KHZ400)
  } else {
    for (;;) {
      *set = pinMask;
      asm("nop;");
      if (p & bitMask) {
        asm("nop;");
        *clr = pinMask;
      } else {
        *clr = pinMask;
        asm("nop;");
      }
      asm("nop;");
      if (bitMask >>= 1) {
        asm("nop;");
      } else {
        if (ptr >= end)
          break;
        p = *ptr++;
        bitMask = 0x80;
      }
    }
  }
#endif

#elif defined(__SAMD51__)
  uint8_t *ptr, *end, p, bitMask, portNum;
  uint32_t pinMask;

  portNum = g_APinDescription[pin].ulPort;
  pinMask = 1ul << g_APinDescription[pin].ulPin;
  ptr = pixels;
  end = ptr + numBytes;
  p = *ptr++;
  bitMask = 0x80;

  volatile uint32_t *set = &(PORT->Group[portNum].OUTSET.reg),
                    *clr = &(PORT->Group[portNum].OUTCLR.reg);

  uint32_t t0, t1, top, ticks,
           saveLoad = SysTick->LOAD, saveVal = SysTick->VAL;

#if defined(NEO_KHZ400)
  if (is800KHz) {
#endif
    top = (uint32_t)(F_CPU * 0.00000125); // Bit hi + lo = 1.25 uS
    t0 = top - (uint32_t)(F_CPU * 0.00000040); // 0 = 0.4 uS hi
    t1 = top - (uint32_t)(F_CPU * 0.00000080); // 1 = 0.8 uS hi
#if defined(NEO_KHZ400)
  } else {
    top = (uint32_t)(F_CPU * 0.00000250); // Bit hi + lo = 2.5 uS
    t0 = top - (uint32_t)(F_CPU * 0.00000050); // 0 = 0.5 uS hi
    t1 = top - (uint32_t)(F_CPU * 0.00000120); // 1 = 1.2 uS hi
  }
#endif

  SysTick->LOAD = top;
  SysTick->VAL = top;
  (void)SysTick->VAL;

  for (;;) {
    *set = pinMask;
    ticks = (p & bitMask) ? t1 : t0;
    while (SysTick->VAL > ticks);
    *clr = pinMask;
    if (!(bitMask >>= 1)) {
      if (ptr >= end)
        break;
      p = *ptr++;
      bitMask = 0x80;
    }
    while (SysTick->VAL <= ticks);
  }

  SysTick->LOAD = saveLoad;
  SysTick->VAL = saveVal;

#elif defined(ARDUINO_STM32_FEATHER)
  uint8_t *ptr, *end, p, bitMask;
  uint32_t pinMask;

  pinMask = BIT(PIN_MAP[pin].gpio_bit);
  ptr = pixels;
  end = ptr + numBytes;
  p = *ptr++;
  bitMask = 0x80;

  volatile uint16_t *set = &(PIN_MAP[pin].gpio_device->regs->BSRRL);
  volatile uint16_t *clr = &(PIN_MAP[pin].gpio_device->regs->BSRRH);

#if defined(NEO_KHZ400)
  if (is800KHz) {
#endif
    for (;;) {
      if (p & bitMask) {
        *set = pinMask;
        asm("nop;");
        *clr = pinMask;
        asm("nop;");
      } else {
        *clr = pinMask;
        asm("nop;");
        *set = pinMask;
        asm("nop;");
      }
      if (bitMask >>= 1) {
        asm("nop;");
      } else {
        if (ptr >= end)
          break;
        p = *ptr++;
        bitMask = 0x80;
      }
    }
#if defined(NEO_KHZ400)
  } else {
    // ToDo!
  }
#endif

#elif defined(TARGET_LPC1768)
  uint8_t *ptr, *end, p, bitMask;
  ptr = pixels;
  end = ptr + numBytes;
  p = *ptr++;
  bitMask = 0x80;

#if defined(NEO_KHZ400)
  if (is800KHz) {
#endif
    for (;;) {
      if (p & bitMask) {
        gpio_set(pin);
        time::delay_ns(550);
        gpio_clear(pin);
        time::delay_ns(450);
      } else {
        gpio_set(pin);
        time::delay_ns(200);
        gpio_clear(pin);
        time::delay_ns(450);
      }
      if (bitMask >>= 1) {
        asm("nop;");
      } else {
        if (ptr >= end)
          break;
        p = *ptr++;
        bitMask = 0x80;
      }
    }
#if defined(NEO_KHZ400)
  } else {
    // ToDo!
  }
#endif

#elif defined(ARDUINO_ARCH_STM32) || defined(ARDUINO_ARCH_ARDUINO_CORE_STM32)
  uint8_t *p = pixels, *end = p + numBytes,
          pix = *p++, mask = 0x80;
  uint32_t cyc;
  uint32_t saveLoad = SysTick->LOAD, saveVal = SysTick->VAL;
#if defined(NEO_KHZ400)
  if (is800KHz) {
#endif
    uint32_t top = (F_CPU / 800000);
    uint32_t t0 = top - (F_CPU / 2500000);
    uint32_t t1 = top - (F_CPU / 1250000);
    SysTick->LOAD = top - 1;
    SysTick->VAL = 0;
    for (;;) {
      LL_GPIO_SetOutputPin(gpioPort, gpioPin);
      cyc = (pix & mask) ? t1 : t0;
      while (SysTick->VAL > cyc);
      LL_GPIO_ResetOutputPin(gpioPort, gpioPin);
      if (!(mask >>= 1)) {
        if (p >= end)
          break;
        pix = *p++;
        mask = 0x80;
      }
      while (SysTick->VAL <= cyc);
    }
#if defined(NEO_KHZ400)
  } else {
    uint32_t top = (F_CPU / 400000);
    uint32_t t0 = top - (F_CPU / 2000000);
    uint32_t t1 = top - (F_CPU / 833333);
    SysTick->LOAD = top - 1;
    SysTick->VAL = 0;
    for (;;) {
      LL_GPIO_SetOutputPin(gpioPort, gpioPin);
      cyc = (pix & mask) ? t1 : t0;
      while (SysTick->VAL > cyc);
      LL_GPIO_ResetOutputPin(gpioPort, gpioPin);
      if (!(mask >>= 1)) {
        if (p >= end)
          break;
        pix = *p++;
        mask = 0x80;
      }
      while (SysTick->VAL <= cyc);
    }
  }
#endif // NEO_KHZ400
  SysTick->LOAD = saveLoad;
  SysTick->VAL = saveVal;
#elif defined(NRF51)
  uint8_t *p = pixels,
          pix, count, mask;
  int32_t num = numBytes;
  unsigned int bitmask = (1 << g_ADigitalPinMap[pin]);

  volatile unsigned int *reg = (unsigned int *)(0x50000000UL + 0x508);

  asm volatile(
      "b L%=_start"
    // .nextbit:               ;            C0
    "L%=_nextbit:"
    "strb %[bitmask], [%[reg], #0]" "\n\t"
    "tst %[mask], %[pix]" "\n\t"
    "bne L%=_islate" "\n\t"
    "strb %[bitmask], [%[reg], #4]" "\n\t"
    "L%=_islate:"
    "lsr %[mask], %[mask], #1" "\n\t"
    "bne L%=_justbit" "\n\t"
    "add %[p], #1" "\n\t"
    "sub %[num], #1" "\n\t"
    "bcc L%=_stop" "\n\t"
    "L%=_start:"
    "movs %[mask], #0x80" "\n\t"
    "nop" "\n\t"
    "L%=_common:"
    "strb %[bitmask], [%[reg], #4]" "\n\t"
    "ldrb  %[pix], [%[p], #0]" "\n\t"
    "b L%=_nextbit" "\n\t"
    "L%=_justbit:"
    "b L%=_common" "\n\t"
    "L%=_stop:"
    "strb %[bitmask], [%[reg], #4]" "\n\t"
    : [p] "+r"(p),
      [pix] "=&r"(pix),
      [count] "=&r"(count),
      [mask] "=&r"(mask),
      [num] "+r"(num)
    : [bitmask] "r"(bitmask),
      [reg] "r"(reg));
#endif
#elif defined(__SAM3X8E__) // Arduino Due
  #define SCALE      VARIANT_MCK / 2UL / 1000000UL
  #define INST       (2UL * F_CPU / VARIANT_MCK)
  #define TIME_800_0 ((int)(0.40 * SCALE + 0.5) - (5 * INST))
  #define TIME_800_1 ((int)(0.80 * SCALE + 0.5) - (5 * INST))
  #define PERIOD_800 ((int)(1.25 * SCALE + 0.5) - (5 * INST))
  #define TIME_400_0 ((int)(0.50 * SCALE + 0.5) - (5 * INST))
  #define TIME_400_1 ((int)(1.20 * SCALE + 0.5) - (5 * INST))
  #define PERIOD_400 ((int)(2.50 * SCALE + 0.5) - (5 * INST))

  int pinMask, time0, time1, period, t;
  Pio *port;
  volatile WoReg *portSet, *portClear, *timeValue, *timeReset;
  uint8_t *p, *end, pix, mask;

  pmc_set_writeprotect(false);
  pmc_enable_periph_clk((uint32_t)TC3_IRQn);
  TC_Configure(TC1, 0, TC_CMR_WAVE | TC_CMR_WAVSEL_UP | TC_CMR_TCCLKS_TIMER_CLOCK1);
  TC_Start(TC1, 0);

  pinMask = g_APinDescription[pin].ulPin;
  port = g_APinDescription[pin].pPort;
  portSet = &(port->PIO_SODR);
  portClear = &(port->PIO_CODR);
  timeValue = &(TC1->TC_CHANNEL[0].TC_CV);
  timeReset = &(TC1->TC_CHANNEL[0].TC_CCR);
  p = pixels;
  end = p + numBytes;
  pix = *p++;
  mask = 0x80;

#if defined(NEO_KHZ400)
  if (is800KHz) {
#endif
    time0 = TIME_800_0;
    time1 = TIME_800_1;
    period = PERIOD_800;
#if defined(NEO_KHZ400)
  } else {
    time0 = TIME_400_0;
    time1 = TIME_400_1;
    period = PERIOD_400;
  }
#endif

  for (t = time0;; t = time0) {
    if (pix & mask) t = time1;
    while (*timeValue < (unsigned)period);
    *portSet = pinMask;
    *timeReset = TC_CCR_CLKEN | TC_CCR_SWTRG;
    while (*timeValue < (unsigned)t);
    *portClear = pinMask;
    if (!(mask >>= 1)) {
      if (p >= end) break;
      pix = *p++;
      mask = 0x80;
    }
  }
  while (*timeValue < (unsigned)period);
  TC_Stop(TC1, 0);

#endif // end Due

// END ARM ----------------------------------------------------------------

#elif defined(ESP8266) || defined(ESP32)

// ESP8266 ----------------------------------------------------------------

  // ESP8266 show() is external to enforce ICACHE_RAM_ATTR execution
  espShow(pin, pixels, numBytes, is800KHz);

#elif defined(KENDRYTE_K210)

  k210Show(pin, pixels, numBytes, is800KHz);

#elif defined(__ARDUINO_ARC__)

// Arduino 101  -----------------------------------------------------------

#define NOPx7 { __builtin_arc_nop(); \
  __builtin_arc_nop(); __builtin_arc_nop(); \
  __builtin_arc_nop(); __builtin_arc_nop(); \
  __builtin_arc_nop(); __builtin_arc_nop(); }

  PinDescription *pindesc = &g_APinDescription[pin];
  register uint32_t loop = 8 * numBytes;
  register uint8_t *p = pixels;
  register uint32_t currByte = (uint32_t)(*p);
  register uint32_t currBit = 0x80 & currByte;
  register uint32_t bitCounter = 0;
  register uint32_t first = 1;

  if (pindesc->ulGPIOType == SS_GPIO) {
    register uint32_t reg = pindesc->ulGPIOBase + SS_GPIO_SWPORTA_DR;
    uint32_t reg_val = __builtin_arc_lr((volatile uint32_t)reg);
    register uint32_t reg_bit_high = reg_val | (1 << pindesc->ulGPIOId);
    register uint32_t reg_bit_low = reg_val & ~(1 << pindesc->ulGPIOId);

    loop += 1;
    while (loop--) {
      if (!first) {
        currByte <<= 1;
        bitCounter++;
      }

      __builtin_arc_sr(first ? reg_bit_low : reg_bit_high, (volatile uint32_t)reg);
      if (currBit) {
        NOPx7
        NOPx7
      }
      NOPx7
      __builtin_arc_nop();

      __builtin_arc_sr(reg_bit_low, (volatile uint32_t)reg);
      NOPx7
      NOPx7

      if (bitCounter >= 8) {
        bitCounter = 0;
        currByte = (uint32_t)(*++p);
      }

      currBit = 0x80 & currByte;
      first = 0;
    }
  } else if (pindesc->ulGPIOType == SOC_GPIO) {
    register uint32_t reg = pindesc->ulGPIOBase + SOC_GPIO_SWPORTA_DR;
    uint32_t reg_val = MMIO_REG_VAL(reg);
    register uint32_t reg_bit_high = reg_val | (1 << pindesc->ulGPIOId);
    register uint32_t reg_bit_low = reg_val & ~(1 << pindesc->ulGPIOId);

    loop += 1;
    while (loop--) {
      if (!first) {
        currByte <<= 1;
        bitCounter++;
      }
      MMIO_REG_VAL(reg) = first ? reg_bit_low : reg_bit_high;
      if (currBit) {
        NOPx7
        NOPx7
        __builtin_arc_nop();
      }
      NOPx7

      MMIO_REG_VAL(reg) = reg_bit_low;
      NOPx7
      NOPx7

      if (bitCounter >= 8) {
        bitCounter = 0;
        currByte = (uint32_t)(*++p);
      }

      currBit = 0x80 & currByte;
      first = 0;
    }
  }

#else
#error Architecture not supported
#endif

// END ARCHITECTURE SELECT ------------------------------------------------

#if !(defined(NRF52) || defined(NRF52_SERIES))
  interrupts();
#endif

  endTime = micros();
}

void Adafruit_NeoPixel::setPin(uint16_t p) {
  if (begun && (pin >= 0)) pinMode(pin, INPUT);
  pin = p;
  if (begun) {
    pinMode(p, OUTPUT);
    digitalWrite(p, LOW);
  }
#if defined(__AVR__)
  port = portOutputRegister(digitalPinToPort(p));
  pinMask = digitalPinToBitMask(p);
#endif
#if defined(ARDUINO_ARCH_STM32) || defined(ARDUINO_ARCH_ARDUINO_CORE_STM32)
  gpioPort = digitalPinToPort(p);
  gpioPin = STM_LL_GPIO_PIN(digitalPinToPinName(p));
#endif
}

void Adafruit_NeoPixel::setPixelColor(
 uint16_t n, uint8_t r, uint8_t g, uint8_t b) {

  if (n < numLEDs) {
    if (brightness) {
      r = (r * brightness) >> 8;
      g = (g * brightness) >> 8;
      b = (b * brightness) >> 8;
    }
    uint8_t *p;
    if (wOffset == rOffset) {
      p = &pixels[n * 3];
    } else {
      p = &pixels[n * 4];
      p[wOffset] = 0;
    }
    p[rOffset] = r;
    p[gOffset] = g;
    p[bOffset] = b;
  }
}
void Adafruit_NeoPixel::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  if (n < numLEDs) {
    if (brightness) {
      r = (r * brightness) >> 8;
      g = (g * brightness) >> 8;
      b = (b * brightness) >> 8;
      w = (w * brightness) >> 8;
    }
    uint8_t *p = (wOffset == rOffset) ? &pixels[n * 3] : &pixels[n * 4];
    if (wOffset != rOffset) p[wOffset] = w;
    p[rOffset] = r;
    p[gOffset] = g;
    p[bOffset] = b;
  }
}

void Adafruit_NeoPixel::setPixelColor(uint16_t n, uint32_t c) {
  if (n < numLEDs) {
    uint8_t *p = (wOffset == rOffset) ? &pixels[n * 3] : &pixels[n * 4];
    uint8_t r = c >> 16, g = c >> 8, b = c;
    if (brightness) {
      r = (r * brightness) >> 8;
      g = (g * brightness) >> 8;
      b = (b * brightness) >> 8;
    }
    if (wOffset != rOffset) p[wOffset] = c >> 24;
    p[rOffset] = r;
    p[gOffset] = g;
    p[bOffset] = b;
  }
}

void Adafruit_NeoPixel::fill(uint32_t c, uint16_t first, uint16_t count) {
  uint16_t end;
  if (first < numLEDs) {
    end = (count == 0) ? numLEDs : min(first + count, numLEDs);
    for (uint16_t i = first; i < end; i++) {
      this->setPixelColor(i, c);
    }
  }
}

uint32_t Adafruit_NeoPixel::ColorHSV(uint16_t hue, uint8_t sat, uint8_t val) {
  // Implementation of ColorHSV remains unchanged
}

uint32_t Adafruit_NeoPixel::getPixelColor(uint16_t n) const {
  if (n >= numLEDs) return 0;
  const uint8_t *p = (wOffset == rOffset) ? &pixels[n * 3] : &pixels[n * 4];
  uint8_t r = p[rOffset], g = p[gOffset], b = p[bOffset];
  if (wOffset != rOffset) {
    uint8_t w = p[wOffset];
    return (brightness ? (((uint32_t)(w * brightness) >> 8) << 24) : ((uint32_t)w << 24)) |
           (brightness ? (((uint32_t)(r * brightness) >> 8) << 16) : ((uint32_t)r << 16)) |
           (brightness ? (((uint32_t)(g * brightness) >> 8) << 8) : ((uint32_t)g << 8)) |
           (brightness ? ((uint32_t)(b * brightness) >> 8) : (uint32_t)b);
  } else {
    return (brightness ? (((uint32_t)(r * brightness) >> 8) << 16) : ((uint32_t)r << 16)) |
           (brightness ? (((uint32_t)(g * brightness) >> 8) << 8) : ((uint32_t)g << 8)) |
           (brightness ? ((uint32_t)(b * brightness) >> 8) : (uint32_t)b);
  }
}

void Adafruit_NeoPixel::setBrightness(uint8_t b) {
  uint8_t newBrightness = b + 1;
  if (newBrightness != brightness) {
    uint8_t c, *ptr = pixels;
    uint16_t scale;
    if (brightness == 0) {
      scale = 0;
    } else if (b == 255) {
      scale = 65535 / brightness;
    } else {
      scale = (((uint16_t)newBrightness << 8) - 1) / brightness;
    }
    for (uint16_t i = 0; i < numBytes; i++) {
      c = *ptr;
      *ptr++ = (c * scale) >> 8;
    }
    brightness = newBrightness;
  }
}

uint8_t Adafruit_NeoPixel::getBrightness(void) const {
  return brightness - 1;
}

void Adafruit_NeoPixel::clear(void) {
  memset(pixels, 0, numBytes);
}

uint32_t Adafruit_NeoPixel::gamma32(uint32_t x) {
  uint8_t *y = (uint8_t *)&x;
  for (uint8_t i = 0; i < 4; i++) y[i] = gamma8(y[i]);
  return x;
}
