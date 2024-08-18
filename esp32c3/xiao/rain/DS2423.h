/*
 *   DS2423.h
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

#ifndef DS2423_h
#define DS2423_h

#include <Arduino.h>
#include <OneWire.h>

#define DS2423_READ_MEMORY_COMMAND 0xa5
#define DS2423_PAGE_ONE 0xc0
#define DS2423_PAGE_TWO 0xe0
#define DS2423_COUNTER_A 0x01
#define DS2423_COUNTER_B 0x02

class DS2423 {
    public:
        DS2423(OneWire *ow, uint8_t *address);
        void begin(uint8_t counter=DS2423_COUNTER_A);
        void update();
        uint32_t getCount(uint8_t counter=DS2423_COUNTER_A);
        unsigned long getTimestamp();
        boolean isError();
    private:
        OneWire *_ow;
        uint8_t *_address;
        uint8_t _counter;
        unsigned long _timestamp;
        boolean _errorA;
        boolean _errorB;
        uint32_t _countA;
        uint32_t _countB;
        void readCounter(uint8_t counter);
};

#endif
