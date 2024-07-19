import network
import urequests
import utime as time

# Network settings
wifi_ssid = "Wifihill"
wifi_password = "Wifihill"

def scan_and_connect():
    station = network.WLAN(network.STA_IF)
    station.active(True)
    station.config(dhcp_hostname='esp32c3')

    print("Scanning for WiFi networks, please wait...")
    for ssid, bssid, channel, RSSI, authmode, hidden in station.scan():
        print("* {:s}".format(ssid))
        print("   - Channel: {}".format(channel))
        print("   - RSSI: {}".format(RSSI))
        print("   - BSSID: {:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}".format(*bssid))
        print()

    while not station.isconnected():
        print("Connecting...")
        station.connect(wifi_ssid, wifi_password)
        time.sleep(10)

    print("Connected!")
    print("My IP Address:", station.ifconfig()[0])


# Execute the functions
scan_and_connect()
