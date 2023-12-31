#pragma once

#include "Arduino.h"
#include <ESP8266WiFi.h>
extern "C" {
  #include "user_interface.h"
}
#include "language.h"
#include "Accesspoints.h"
#include "Stations.h"
#include "SSIDs.h"
#include "Scan.h"

extern SSIDs ssids;
extern Accesspoints accesspoints;
extern Stations stations;
extern Scan scan;

extern uint8_t wifi_channel;
extern uint8_t broadcast[6];
extern uint32_t currentTime;

extern bool macBroadcast(uint8_t* mac);
extern void getRandomMac(uint8_t* mac);
extern void setOutputPower(float dBm);
extern String macToStr(const uint8_t* mac);
extern String bytesToStr(const uint8_t* b, uint32_t size);
extern void setWifiChannel(uint8_t ch, bool force);
extern bool writeFile(String path, String& buf);
extern int8_t free80211_send(uint8_t* buffer, uint16_t len);

class Attack {
public:
    Attack();

    void start();
    void start(bool beacon, bool deauth, bool deauthAll, bool probe, bool output, uint32_t timeout);
    void stop();
    void update();

    void enableOutput();
    void disableOutput();
    void status();
    String getStatusJSON();

    bool deauthAP(int num);
    bool deauthStation(int num);
    bool deauthName(int num);
    bool deauthDevice(uint8_t* apMac, uint8_t* stMac, uint8_t reason, uint8_t ch);

    bool sendBeacon(uint8_t tc);
    bool sendBeacon(uint8_t* mac, const char* ssid, uint8_t ch, bool wpa2);

    bool sendProbe(uint8_t tc);
    bool sendProbe(uint8_t* mac, const char* ssid, uint8_t ch);

    bool sendPacket(uint8_t* packet, uint16_t packetSize, uint8_t ch, bool force_ch);

    bool isRunning();

    uint32_t getDeauthPkts();
    uint32_t getBeaconPkts();
    uint32_t getProbePkts();
    uint32_t getDeauthMaxPkts();
    uint32_t getBeaconMaxPkts();
    uint32_t getProbeMaxPkts();

    uint32_t getPacketRate();

private:
    void deauthUpdate();
    void deauthAllUpdate();
    void beaconUpdate();
    void probeUpdate();

    void updateCounter();

    bool running = false;
    bool output  = true;

    struct AttackType {
        bool     active        = false;
        uint16_t packetCounter = 0;
        uint16_t maxPkts       = 0;
        uint8_t  tc            = 0;
        uint32_t time          = 0;
    };

    AttackType deauth;
    AttackType beacon;
    AttackType probe;
    bool deauthAll = false;

    uint32_t deauthPkts = 0;
    uint32_t beaconPkts = 0;
    uint32_t probePkts  = 0;

    uint32_t tmpPacketRate = 0;
    uint32_t packetRate    = 0;

    uint8_t apCount = 0;
    uint8_t stCount = 0;
    uint8_t nCount  = 0;

    int8_t tmpID = -1;

    uint16_t packetSize      = 0;
    uint32_t attackTime      = 0;
    uint32_t attackStartTime = 0;
    uint32_t timeout         = 0;

    uint8_t mac[6] = { 0xAA, 0xBB, 0xCC, 0x00, 0x11, 0x22 };

    uint8_t deauthPacket[26] = {0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x00, 0x00, 0x01, 0x00};

    uint8_t probePacket[68] = {0x40, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c};

    uint8_t beaconPacket[109] = {0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x00, 0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, 0x64, 0x00, 0x31, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, 0x03, 0x01, 0x01, 0x30, 0x18, 0x01, 0x00, 0x00, 0x0f, 0xac, 0x02, 0x02, 0x00, 0x0f, 0xac, 0x04, 0x00, 0x0f, 0xac, 0x04, 0x01, 0x00, 0x0f, 0xac, 0x02, 0x00, 0x00};

};
