arduino-cli config init

arduino-cli sketch new blink

arduino-cli compile --fbqn arduino:avr:nano blink

arduino-cli upload --port /dev/cu.usbserial-10  --fqbn arduino:avr:nano blink



