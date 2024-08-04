import time
from sds011 import SDS011

sensor = SDS011("/dev/cu.usbserial-143340", use_query_mode=True)
sensor.sleep(sleep=False)  # Turn on fan and diode
time.sleep(15)  # Allow time for the sensor to measure properly
pm25, pm10 = sensor.query()
print("pm25", pm25)
print("pm10", pm10)
sensor.sleep();
