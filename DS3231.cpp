
#include "DS3231.h"
// not need them in the future...
#if defined(__AVR__)
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#endif // if defined(__AVR__)
// #include "WProgram.h"
#include <Arduino.h>


#define CLOCK_ADDRESS 0x68

#define SECONDS_FROM_1970_TO_2000 946684800
DS3231::DS3231() {
}

// utility code, some of this could be exposed in the DateTime API if needed
// to rewrite their code. -ADW
static const uint8_t daysInMonth[] PROGMEM = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
    if (y >= 2000)
        y -= 2000;
    uint16_t days = d;

    for (uint8_t i = 1; i < m; ++i) days += pgm_read_byte(daysInMonth + i - 1);
    if ((m > 2) && (y % 4 == 0))
        ++days;
    return days + 365 * y + (y + 3) / 4 - 1;
}

static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
    return ((days * 24L + h) * 60 + m) * 60 + s;
}
/*******************************************************************************
 * TO GET ALL DATE/TIME INFORMATION AT ONCE AND AVOID THE CHANCE OF ROLLOVER
 * DateTime implementation spliced in here from Jean-Claude Wippler's (JeeLabs)
 * RTClib, as modified by Limor Fried (Ladyada); source code at:
 * https://github.com/adafruit/RTClib
 ******************************************************************************/
// DateTime implementation - ignores time zones and DST changes

DateTime::DateTime(uint32_t t) {
    t -= SECONDS_FROM_1970_TO_2000; // bring to 2000 timestamp from 1970

    ss = t % 60;
    t /= 60;
    mm = t % 60;
    t /= 60;
    hh = t % 24;
    uint16_t days = t / 24;
    uint8_t  leap;

    for (yOff = 0;; ++yOff) {
        leap = yOff % 4 == 0;
        if (days < 365 + leap)
            break;
        days -= 365 + leap;
    }

    for (m = 1;; ++m) {
        uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
        if (leap && (m == 2))
            ++daysPerMonth;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    d = days + 1;
}

DateTime::DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
    if (year >= 2000)
        year -= 2000;
    yOff = year;
    m    = month;
    d    = day;
    hh   = hour;
    mm   = min;
    ss   = sec;
}

static uint8_t conv2d(const char* p) {
    uint8_t v = 0;

    if (('0' <= *p) && (*p <= '9'))
        v = *p - '0';
    return 10 * v + *++p - '0';
}
uint32_t DateTime::unixtime(void) const {
    uint32_t t;
    uint16_t days = date2days(yOff, m, d);

    t  = time2long(days, hh, mm, ss);
    t += SECONDS_FROM_1970_TO_2000; // seconds from 1970 to 2000

    return t;
}
// Get all date/time at once to avoid rollover (e.g., minute/second don't match)
static uint8_t bcd2bin(uint8_t val) {
    return val - 6 * (val >> 4);
}

static uint8_t bin2bcd(uint8_t val) {
    return val + 6 * (val / 10);
}

DateTime RTClib::now() {
    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0); // This is the first register address (Seconds)
    Wire.endTransmission();

    Wire.requestFrom(CLOCK_ADDRESS, 7);
    uint8_t ss = bcd2bin(Wire.read() & 0x7F);
    uint8_t mm = bcd2bin(Wire.read());
    uint8_t hh = bcd2bin(Wire.read());

    Wire.read();
    uint8_t  d = bcd2bin(Wire.read());
    uint8_t  m = bcd2bin(Wire.read());
    uint16_t y = bcd2bin(Wire.read()) + 2000;

    return DateTime(y, m, d, hh, mm, ss);
}

byte DS3231::getSecond() {
    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x00);
    Wire.endTransmission();

    Wire.requestFrom(CLOCK_ADDRESS, 1);
    return bcdToDec(Wire.read());
}

byte DS3231::getMinute() {
    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x01);
    Wire.endTransmission();

    Wire.requestFrom(CLOCK_ADDRESS, 1);
    return bcdToDec(Wire.read());
}

byte DS3231::getHour(bool& h12, bool& PM_time) {
    byte temp_buffer;
    byte hour;

    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x02);
    Wire.endTransmission();

    Wire.requestFrom(CLOCK_ADDRESS, 1);
    temp_buffer = Wire.read();
    h12         = temp_buffer & 0b01000000;
    if (h12) {
        PM_time = temp_buffer & 0b00100000;
        hour    = bcdToDec(temp_buffer & 0b00011111);
    } else {
        hour = bcdToDec(temp_buffer & 0b00111111);
    }
    return hour;
}

byte DS3231::getDoW() {
    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x03);
    Wire.endTransmission();

    Wire.requestFrom(CLOCK_ADDRESS, 1);
    return bcdToDec(Wire.read());
}

byte DS3231::getDate() {
    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x04);
    Wire.endTransmission();

    Wire.requestFrom(CLOCK_ADDRESS, 1);
    return bcdToDec(Wire.read());
}

byte DS3231::getMonth(bool& Century) {
    byte temp_buffer;
    byte hour;

    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x05);
    Wire.endTransmission();

    Wire.requestFrom(CLOCK_ADDRESS, 1);
    temp_buffer = Wire.read();
    Century     = temp_buffer & 0b10000000;
    return bcdToDec(temp_buffer & 0b01111111);
}

byte DS3231::getYear() {
    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x06);
    Wire.endTransmission();

    Wire.requestFrom(CLOCK_ADDRESS, 1);
    return bcdToDec(Wire.read());
}

void DS3231::setSecond(byte Second) {
    // This function also resets the Oscillator Stop Flag, which is set
    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x00);
    Wire.write(decToBcd(Second));
    Wire.endTransmission();
    byte temp_buffer = readControlByte(1);

    writeControlByte((temp_buffer & 0b01111111), 1);
}

void DS3231::setMinute(byte Minute) {
    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x01);
    Wire.write(decToBcd(Minute));
    Wire.endTransmission();
}

void DS3231::setHour(byte Hour) {
    // The hour must be in 24h format.

    bool h12;
    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x02);
    Wire.endTransmission();
    Wire.requestFrom(CLOCK_ADDRESS, 1);
    h12 = (Wire.read() & 0b01000000);

    if (h12) {
        if (Hour > 12) {
            Hour = decToBcd(Hour-12) | 0b01100000;
        } else {
            Hour = decToBcd(Hour) & 0b11011111;
        }
    } else {
        Hour = decToBcd(Hour) & 0b10111111;
    }

    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x02);
    Wire.write(Hour);
    Wire.endTransmission();
}

void DS3231::setDoW(byte DoW) {
    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x03);
    Wire.write(decToBcd(DoW));
    Wire.endTransmission();
}

void DS3231::setDate(byte Date) {
    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x04);
    Wire.write(decToBcd(Date));
    Wire.endTransmission();
}

void DS3231::setMonth(byte Month) {
    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x05);
    Wire.write(decToBcd(Month));
    Wire.endTransmission();
}

void DS3231::setYear(byte Year) {
    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x06);
    Wire.write(decToBcd(Year));
    Wire.endTransmission();
}

void DS3231::setClockMode(bool h12) {
    // One thing that bothers me about how I've written this is that
    // the clock will be set back an hour. Not sure how to do it better,
    // a very minimal risk.
    // the setHour() function doesn't change this mode.

    byte temp_buffer;
    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x02);
    Wire.endTransmission();
    Wire.requestFrom(CLOCK_ADDRESS, 1);
    temp_buffer = Wire.read();
    if (h12) {
        temp_buffer = temp_buffer | 0b01000000;
    } else {
        temp_buffer = temp_buffer & 0b10111111;
    }
    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x02);
    Wire.write(temp_buffer);
    Wire.endTransmission();
}

float DS3231::getTemperature() {
    // temperature as a floating-point value.
    // http://forum.arduino.cc/index.php/topic,22301.0.html

    byte  tMSB, tLSB;
    float temp3231;
    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x11);
    Wire.endTransmission();
    Wire.requestFrom(CLOCK_ADDRESS, 2);
    if (Wire.available()) {
        tMSB = Wire.read(); // 2's complement int portion
        tLSB = Wire.read(); // fraction portion

        temp3231 = ((((short)tMSB << 8) | (short)tLSB) >> 6) / 4.0;
    }
    else {
        temp3231 = -9999; // Some obvious error value
    }

    return temp3231;
}

void DS3231::getA1Time(byte& A1Day, byte& A1Hour, byte& A1Minute, byte& A1Second, byte& AlarmBits, bool& A1Dy, bool& A1h12, bool& A1PM) {
    byte temp_buffer;

    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x07);
    Wire.endTransmission();

    Wire.requestFrom(CLOCK_ADDRESS, 4);

    temp_buffer = Wire.read(); // Get A1M1 and A1 Seconds
    A1Second    = bcdToDec(temp_buffer & 0b01111111);
    AlarmBits = AlarmBits | (temp_buffer & 0b10000000)>>7;

    temp_buffer = Wire.read(); // Get A1M2 and A1 minutes
    A1Minute    = bcdToDec(temp_buffer & 0b01111111);
    AlarmBits = AlarmBits | (temp_buffer & 0b10000000)>>6;

    temp_buffer = Wire.read(); // Get A1M3 and A1 Hour
    AlarmBits = AlarmBits | (temp_buffer & 0b10000000)>>5;
    A1h12 = temp_buffer & 0b01000000;
    if (A1h12) {
        A1PM   = temp_buffer & 0b00100000;           // determine am/pm
        A1Hour = bcdToDec(temp_buffer & 0b00011111); // 12-hour
    } else {
        A1Hour = bcdToDec(temp_buffer & 0b00111111); // 24-hour
    }

    temp_buffer = Wire.read();                       // Get A1M4 and A1 Day/Date
    AlarmBits = AlarmBits | (temp_buffer & 0b10000000)>>4;
    A1Dy = (temp_buffer & 0b01000000)>>6;
    if (A1Dy) {
        A1Day = bcdToDec(temp_buffer & 0b00001111);
    } else {
        A1Day = bcdToDec(temp_buffer & 0b00111111);
    }
}

void DS3231::getA2Time(byte& A2Day, byte& A2Hour, byte& A2Minute, byte& AlarmBits, bool& A2Dy, bool& A2h12, bool& A2PM) {
    byte temp_buffer;

    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x0b);
    Wire.endTransmission();

    Wire.requestFrom(CLOCK_ADDRESS, 3);
    temp_buffer = Wire.read(); // Get A2M2 and A2 Minutes
    A2Minute    = bcdToDec(temp_buffer & 0b01111111);
    AlarmBits = AlarmBits | (temp_buffer & 0b10000000)>>3;

    temp_buffer = Wire.read(); // Get A2M3 and A2 Hour
    AlarmBits = AlarmBits | (temp_buffer & 0b10000000)>>2;
    A2h12 = temp_buffer & 0b01000000;
    if (A2h12) {
        A2PM   = temp_buffer & 0b00100000;           // determine am/pm
        A2Hour = bcdToDec(temp_buffer & 0b00011111); // 12-hour
    } else {
        A2Hour = bcdToDec(temp_buffer & 0b00111111); // 24-hour
    }

    temp_buffer = Wire.read();                       // Get A2M4 and A1 Day/Date
    AlarmBits = AlarmBits | (temp_buffer & 0b10000000)>>1;
    A2Dy = (temp_buffer & 0b01000000)>>6;
    if (A2Dy) {
        A2Day = bcdToDec(temp_buffer & 0b00001111);
    } else {
        A2Day = bcdToDec(temp_buffer & 0b00111111);
    }
}

void DS3231::setA1Time(byte A1Day, byte A1Hour, byte A1Minute, byte A1Second, byte AlarmBits, bool A1Dy, bool A1h12, bool A1PM) {
    byte temp_buffer;

    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x07); // A1 starts at 07h
    Wire.write(decToBcd(A1Second) | ((AlarmBits & 0b00000001) << 7));
    Wire.write(decToBcd(A1Minute) | ((AlarmBits & 0b00000010) << 6));
    if (A1h12) {
        if (A1Hour > 12) {
            A1Hour = A1Hour - 12;
            A1PM   = true;
        }
        if (A1PM) {
            // Convert the hour to BCD and add appropriate flags.
            temp_buffer = decToBcd(A1Hour) | 0b01100000;
        } else {
            // Convert the hour to BCD and add appropriate flags.
            temp_buffer = decToBcd(A1Hour) | 0b01000000;
        }
    } else {
        temp_buffer = decToBcd(A1Hour);
    }
    temp_buffer = temp_buffer | ((AlarmBits & 0b00000100)<<5);
    Wire.write(temp_buffer);
    temp_buffer = ((AlarmBits & 0b00001000)<<4) | decToBcd(A1Day);
    if (A1Dy) {
        temp_buffer = temp_buffer | 0b01000000;
    }
    Wire.write(temp_buffer);
    Wire.endTransmission();
}

void DS3231::setA2Time(byte A2Day, byte A2Hour, byte A2Minute, byte AlarmBits, bool A2Dy, bool A2h12, bool A2PM) {
    byte temp_buffer;

    Wire.beginTransmission(CLOCK_ADDRESS);
    Wire.write(0x0b); // A1 starts at 0bh
    Wire.write(decToBcd(A2Minute) | ((AlarmBits & 0b00010000) << 3));
    if (A2h12) {
        if (A2Hour > 12) {
            A2Hour = A2Hour - 12;
            A2PM   = true;
        }
        if (A2PM) {
            // Convert the hour to BCD and add appropriate flags.
            temp_buffer = decToBcd(A2Hour) | 0b01100000;
        } else {
            // Convert the hour to BCD and add appropriate flags.
            temp_buffer = decToBcd(A2Hour) | 0b01000000;
        }
    } else {
        temp_buffer = decToBcd(A2Hour);
    }
    temp_buffer = temp_buffer | ((AlarmBits & 0b00100000)<<2);
    Wire.write(temp_buffer);
    temp_buffer = ((AlarmBits & 0b01000000)<<1) | decToBcd(A2Day);
    if (A2Dy) {
        temp_buffer = temp_buffer | 0b01000000;
    }
    Wire.write(temp_buffer);
    Wire.endTransmission();
}

void DS3231::turnOnAlarm(byte Alarm) {
    byte temp_buffer = readControlByte(0);
    if (Alarm == 1) {
        temp_buffer = temp_buffer | 0b00000101;
    } else {
        temp_buffer = temp_buffer | 0b00000110;
    }
    writeControlByte(temp_buffer, 0);
}

void DS3231::turnOffAlarm(byte Alarm) {
    // Leaves interrupt pin alone.
    byte temp_buffer = readControlByte(0);
    if (Alarm == 1) {
        temp_buffer = temp_buffer & 0b11111110;
    } else {
        temp_buffer = temp_buffer & 0b11111101;
    }
    writeControlByte(temp_buffer, 0);
}

bool DS3231::checkAlarmEnabled(byte Alarm) {
    byte result      = 0x0;
    byte temp_buffer = readControlByte(0);

    if (Alarm == 1) {
        result = temp_buffer & 0b00000001;
    } else {
        result = temp_buffer & 0b00000010;
    }
    return result;
}

bool DS3231::checkIfAlarm(byte Alarm) {
    // Turns flag off, also.
    byte result;
    byte temp_buffer = readControlByte(1);

    if (Alarm == 1) {
        result = temp_buffer & 0b00000001;
        temp_buffer = temp_buffer & 0b11111110;
    } else {
        result = temp_buffer & 0b00000010;
        temp_buffer = temp_buffer & 0b11111101;
    }
    writeControlByte(temp_buffer, 1);
    return result;
}

void DS3231::enableOscillator(bool TF, bool battery, byte frequency) {
    // if battery is true, turns on even for battery-only operation,
    // frequency must be 0, 1, 2, or 3.
    // 1 = 1.024 kHz
    // 3 = 8.192 kHz (Default if frequency byte is out of range)
    if (frequency > 3) frequency = 3;
    byte temp_buffer = readControlByte(0) & 0b11100111;

    if (battery) {
        temp_buffer = temp_buffer | 0b01000000;
    } else {
        temp_buffer = temp_buffer & 0b10111111;
    }
    if (TF) {
        temp_buffer = temp_buffer & 0b01111011;
    } else {
        temp_buffer = temp_buffer | 0b10000000;
    }
    frequency   = frequency << 3;
    temp_buffer = temp_buffer | frequency;
    writeControlByte(temp_buffer, 0);
}

void DS3231::enable32kHz(bool TF) {
    byte temp_buffer = readControlByte(1);

    if (TF) {
        temp_buffer = temp_buffer | 0b00001000;
    } else {
        temp_buffer = temp_buffer & 0b11110111;
    }
    writeControlByte(temp_buffer, 1);
}

bool DS3231::oscillatorCheck() {
    // If this is the case, the time is probably not correct.
    byte temp_buffer = readControlByte(1);
    bool result      = true;

    if (temp_buffer & 0b10000000) {
        result = false;
    }
    return result;
}
byte DS3231::decToBcd(byte val) {
    return (val/10*16) + (val%10);
}

byte DS3231::bcdToDec(byte val) {
    return (val/16*10) + (val%16);
}

byte DS3231::readControlByte(bool which) {
    // first byte (0) is 0x0e, second (1) is 0x0f
    Wire.beginTransmission(CLOCK_ADDRESS);
    if (which) {
        Wire.write(0x0f);
    } else {
        Wire.write(0x0e);
    }
    Wire.endTransmission();
    Wire.requestFrom(CLOCK_ADDRESS, 1);
    return Wire.read();
}

void DS3231::writeControlByte(byte control, bool which) {
    // which=false -> 0x0e, true->0x0f.
    Wire.beginTransmission(CLOCK_ADDRESS);
    if (which) {
        Wire.write(0x0f);
    } else {
        Wire.write(0x0e);
    }
    Wire.write(control);
    Wire.endTransmission();
}