from sds011 import SDS011

sensor = SDS011("/dev/cu.usbserial-143340", use_query_mode=True)
print(sensor.query())  # Gets (pm25, pm10)
