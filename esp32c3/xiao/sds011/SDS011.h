// SDS011 dust sensor PM2.5 and PM10
//
// Base on library by R. Zschiegner 

#ifndef __SDS011_H
#define __SDS011_H

#include "Arduino.h"

class SDS011 {
	public:
		SDS011(void);
		void begin(HardwareSerial* serial);
		void begin(HardwareSerial* serial, int8_t pin_rx, int8_t pin_tx);
		int read(float *p25, float *p10);
		void sleep();
		void wakeup();
		void continuous_mode();
		void write19(const uint8_t *command);
		void read10();
		void read10(uint8_t *response);
	private:
		Stream *sds_data;
};

#endif
