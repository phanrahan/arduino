// SDS011 dust sensor PM2.5 and PM10

#include "SDS011.h"

static constexpr uint8_t QUERY_CMD[19] = {
	0xAA, 0xB4, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0xFF,	0xFF, 0x02, 0xAB
};
static constexpr uint8_t SLEEP_CMD[19] = {
	0xAA, 0xB4, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0xFF,	0xFF, 0x05, 0xAB
};
static constexpr uint8_t WAKEUP_CMD[19] = {
	0xAA, 0xB4, 0x06, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x06, 0xAB
};
static constexpr uint8_t VERSION_CMD[] PROGMEM = {
	0xAA, 0xB4, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x05, 0xAB
};

static constexpr uint8_t GET_MODE_CMD[] PROGMEM = {
	0xAA, 0xB4, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xAB
};

static constexpr uint8_t SET_QUERY_MODE_CMD[] PROGMEM = {
	0xAA, 0xB4, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x02, 0xAB
};

static constexpr uint8_t CONTINUOUS_MODE_CMD[] PROGMEM = {
	0xAA, 0xB4, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x07, 0xAB
};

SDS011::SDS011(void) {

}

// --------------------------------------------------------
// SDS011:read
// --------------------------------------------------------
int SDS011::read(float *p25, float *p10) {
	int pm10_serial = 0;
	int pm25_serial = 0;
  uint8_t response[10];

  write19(QUERY_CMD);
  int ok = read10(response);
  if( ok ) {
    pm25_serial = response[2] + (response[3] << 8);
    pm10_serial = response[4] + (response[5] << 8);

    *p10 = (float)pm10_serial/10.0;
    *p25 = (float)pm25_serial/10.0;
  }
	return ok; // need to perform checksum and other error checking
}

// --------------------------------------------------------
// SDS011:sleep
//
// --------------------------------------------------------
void SDS011::sleep() {
    write19(SLEEP_CMD);
    read10();
}

// --------------------------------------------------------
// SDS011:wakeup
// --------------------------------------------------------
void SDS011::wakeup() {
    write19(WAKEUP_CMD);
    read10();
}

// --------------------------------------------------------
// SDS011:get_mode (active or query)
// --------------------------------------------------------
int SDS011::get_mode() {
    uint8_t response[10];
    write19(GET_MODE_CMD);
    read10(response);
    return response[4];
}

// --------------------------------------------------------
// SDS011:set_query_mode 
// --------------------------------------------------------
void SDS011::set_query_mode() {
    write19(SET_QUERY_MODE_CMD);
    read10();
}


// --------------------------------------------------------
// SDS011:continuous_mode
// --------------------------------------------------------
void SDS011::continuous_mode() {
    write19(CONTINUOUS_MODE_CMD);
    read10();
}

void SDS011::begin(HardwareSerial* serial) {
	serial->begin(9600);
	sds_data = serial;
}

void SDS011::begin(HardwareSerial* serial, int8_t rxPin, int8_t txPin) {
	serial->begin(9600, SERIAL_8N1, rxPin, txPin);
	sds_data = serial;
    while( sds_data->available() )
        sds_data->read();
}

void SDS011::write19(const uint8_t *command) {
	for (int i = 0; i < 19; i++) {
		sds_data->write(command[i]);
	}
	sds_data->flush();
}

int SDS011::read10() {
    uint8_t response[10];
    return read10(response);
}

int SDS011::read10(uint8_t *response) {
	for (int i = 0; i < 10; i++) {
		response[i] = sds_data->read();
	}
    uint8_t checksum = crc(response);
    Serial.printf("checksum %2x %2x\n", response[8], checksum);
    return response[8] == checksum;
}

uint8_t SDS011::crc(uint8_t *response)
{
    uint8_t checksum = 0;
    for (int i = 2; i < 8; i++ ) {
        checksum += response[i];
    }
    return checksum;
}
