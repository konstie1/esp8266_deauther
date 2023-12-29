#ifndef DS3231_h
#define DS3231_h

#include <Arduino.h>
#include <Wire.h>

class DateTime {
public:
    DateTime(uint32_t t = 0);
    DateTime(uint16_t year, uint8_t month, uint8_t day,
             uint8_t hour = 0, uint8_t min = 0, uint8_t sec = 0);
    DateTime(const char* date, const char* time);
    uint16_t year() const { return 2000 + yOff; }
    uint8_t month() const { return m; }
    uint8_t day() const { return d; }
    uint8_t hour() const { return hh; }
    uint8_t minute() const { return mm; }
    uint8_t second() const { return ss; }
    uint8_t dayOfTheWeek() const;
    long secondstime() const;
    uint32_t unixtime(void) const;

protected:
    uint8_t yOff, m, d, hh, mm, ss;
};

class RTClib {
public:
    static DateTime now();
};

class DS3231 {
public:
    DS3231();

    byte getSecond();
    byte getMinute();
    byte getHour(bool& h12, bool& PM_time);
    byte getDoW();
    byte getDate();
    byte getMonth(bool& Century);
    byte getYear();

    void setSecond(byte Second);
    void setMinute(byte Minute);
    void setHour(byte Hour);
    void setDoW(byte DoW);
    void setDate(byte Date);
    void setMonth(byte Month);
    void setYear(byte Year);
    void setClockMode(bool h12);

    float getTemperature();

    void getA1Time(byte& A1Day, byte& A1Hour, byte& A1Minute, byte& A1Second, byte& AlarmBits, bool& A1Dy, bool& A1h12, bool& A1PM);
    void getA2Time(byte& A2Day, byte& A2Hour, byte& A2Minute, byte& AlarmBits, bool& A2Dy, bool& A2h12, bool& A2PM);
    void setA1Time(byte A1Day, byte A1Hour, byte A1Minute, byte A1Second, byte AlarmBits, bool A1Dy, bool A1h12, bool A1PM);
    void setA2Time(byte A2Day, byte A2Hour, byte A2Minute, byte AlarmBits, bool A2Dy, bool A2h12, bool A2PM);
    void turnOnAlarm(byte Alarm);
    void turnOffAlarm(byte Alarm);
    bool checkAlarmEnabled(byte Alarm);
    bool checkIfAlarm(byte Alarm);

    void enableOscillator(bool TF, bool battery, byte frequency);
    void enable32kHz(bool TF);
    bool oscillatorCheck();

private:
    byte decToBcd(byte val);
    byte bcdToDec(byte val);
    byte readControlByte(bool which);
    void writeControlByte(byte control, bool which);
};

#endif
