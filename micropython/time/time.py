from machine import Pin, SoftI2C
import ssd1306
from time import sleep
import time
import network
import urequests
import ujson

# ESP32 Pin assignment
# i2c = SoftI2C(scl=Pin(22), sda=Pin(21))

# ESP8266 Pin assignment
# ESP32c3 Pin assignment
i2c = SoftI2C(scl=Pin(7), sda=Pin(6))  # Adjust the Pin numbers based on your connections

oled_width = 64
oled_height = 32
oled = ssd1306.SSD1306_I2C(oled_width, oled_height, i2c)

station = network.WLAN(network.STA_IF)
station.active(True)
station.config(dhcp_hostname='esp32c3')

# Network settings
wifi_ssid = "Wifihill"
wifi_password = "Wifihill"
url = "http://worldtimeapi.org/api/timezone/America/New_York"

print("Scanning for WiFi networks, please wait...")
authmodes = ['Open', 'WEP', 'WPA-PSK' 'WPA2-PSK4', 'WPA/WPA2-PSK']
for (ssid, bssid, channel, RSSI, authmode, hidden) in station.scan():
    print("* {:s}".format(ssid))
    print("   - Channel: {}".format(channel))
    print("   - RSSI: {}".format(RSSI))
    print("   - BSSID: {:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}".format(*bssid))
    print()

# Continually try to connect to WiFi access point
while not station.isconnected():
    # Try to connect to WiFi access point
    print("Connecting...")
    station.connect(wifi_ssid, wifi_password)
    time.sleep(10)

# Display connection details
print("Connected!")
print("My IP Address:", station.ifconfig()[0])


while True:
    # Perform HTTP GET request on a non-SSL web
    response = urequests.get(url)
    # Check if the request was successful
    if response.status_code == 200:
        # Parse the JSON response
        data = ujson.loads(response.text)
        # Extract the "datetime" field for New York
        ny_datetime = data["datetime"]
        # Split the date and time components
        date_part, time_part = ny_datetime.split("T")
        # Get only the first two decimal places of the time
        time_part = time_part[:8]
        # Get the timezone
        timezone = data["timezone"]
        
        # Clear the OLED display
        oled.fill(0)
        
        # Display the New York date and time on separate lines
        oled.text("NY ", 0, 0)
        oled.text(date_part, 0, 8)
        oled.text("NY ", 0, 16)
        oled.text(time_part, 0, 24)
        #oled.text("Timezone:", 0, 40)
        #oled.text(timezone, 0, 50)
        # Update the display
        oled.show()
    else:
        oled.text("Failed to get the time for New York!")
        # Update the display
        oled.show()
