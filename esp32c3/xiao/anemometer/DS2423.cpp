/*
 *   DS2423.cpp
 *
 *   by Joe Bechter
 *
 *   (C) 2012, bechter.com
 *
 *   All files, software, schematics and designs are provided as-is with no warranty.
 *   All files, software, schematics and designs are for experimental/hobby use.
 *   Under no circumstances should any part be used for critical systems where safety,
 *   life or property depends upon it. You are responsible for all use.
 *   You are free to use, modify, derive or otherwise extend for your own non-commercial purposes provided
 *       1. No part of this software or design may be used to cause injury or death to humans or animals.
 *       2. Use is non-commercial.
 *       3. Credit is given to the author (i.e. portions © bechter.com), and provide a link to the original source.
 *
 */

#include "DS2423.h"

DS2423::DS2423(OneWire *ow, uint8_t *address) {
    _ow = ow;
    _address = address;
};

void DS2423::begin(uint8_t counter) {
    _errorA = false;
    _errorB = false;
    _counter = counter & (DS2423_COUNTER_A | DS2423_COUNTER_B);
    if (counter & DS2423_COUNTER_A) {
        _errorA = true;
    } else if (counter & DS2423_COUNTER_B) {
        _errorB = true;
    } else {
        _errorA = true;
        _errorB = true;
    }
    _countA = 0;
    _countB = 0;
    _timestamp = 0;
}

void DS2423::update() {
    _timestamp = millis();
    if (_counter & DS2423_COUNTER_A) {
        readCounter(DS2423_COUNTER_A);
    }
    if (_counter & DS2423_COUNTER_B) {
        readCounter(DS2423_COUNTER_B);
    }
}

unsigned long DS2423::getTimestamp() {
    return _timestamp;
}

uint32_t DS2423::getCount(uint8_t counter) {
    if (counter == DS2423_COUNTER_A) {
        return _countA;
    } else if (counter == DS2423_COUNTER_B) {
        return _countB;
    } else {
        return 0;
    }
}

boolean DS2423::isError() {
    return _errorA || _errorB;
}

void DS2423::readCounter(uint8_t counter) {
    uint8_t data[45];

    data[0] = DS2423_READ_MEMORY_COMMAND;
    data[1] = (counter == DS2423_COUNTER_B ? DS2423_PAGE_TWO : DS2423_PAGE_ONE);
    data[2] = 0x01;
    _ow->reset();
    _ow->select(_address);
    _ow->write(data[0], 0);
    _ow->write(data[1], 0);
    _ow->write(data[2], 0);
    for (int j = 3; j < 45; j++) {
        data[j] = _ow->read();
    }
    _ow->reset();
    uint32_t count = (uint32_t)data[38];
    for (int j = 37; j >= 35; j--) {
        count = (count << 8) + (uint32_t)data[j];
    }
    uint16_t crc = _ow->crc16(data, 43);
    uint8_t *crcBytes = (uint8_t *)&crc;
    uint8_t crcLo = ~data[43];
    uint8_t crcHi = ~data[44];
    boolean error = (crcLo != crcBytes[0]) || (crcHi != crcBytes[1]);
    if (counter == DS2423_COUNTER_A) {
        _countA = count;
        _errorA = error;
    } else if (counter == DS2423_COUNTER_B) {
        _countB = count;
        _errorB = error;
    }
}

