#!/usr/bin/env python3
#
# upy.py: raw REPL tool
#    udp.py port ls
#    udp.py port run file.py
#       runs in RAM
#    udp.py port put file.py
#       use single quotes for triple quote strings
#    udp.py port put file.py code.py
#       to rename file; code.py to run at boot
#    udp.py port get file.py
#    udp.py port rm file.py
#
# Neil Gershenfeld 6/23/23
#
# This work may be reproduced, modified, distributed,
# performed, and displayed for any purpose, but must
# acknowledge this project. Copyright is retained and
# must be preserved. The work is provided as is; no
# warranty is provided, and users accept all liability.
#

char_delay = 0.001
cmd_delay = 0.1
import sys,serial,time
if (len(sys.argv) < 3):
   print("upy.py port command [arguments]")
   sys.exit()
s = serial.Serial(sys.argv[1])
s.write(b'\x04') # ^d reset
s.write(b'\r\n\x03\r\n') # ^c break
s.write(b'\x01') # ^a enter raw mode
while (s.in_waiting > 0): # clear buffer
   s.read()
if (sys.argv[2] == 'run'):
   file = open(sys.argv[3],'r')
   str = file.read()
   file.close()
   for i in str:
      s.write(bytes(i,'ascii'))
      time.sleep(char_delay)
   s.write(b'\x04') # ^d exit raw mode and execute
   while (True): # show output
      sys.stdout.write(s.read().decode('utf-8'))
elif (sys.argv[2] == 'ls'):
   s.write(b'import os\n')
   s.write(b'files = os.listdir()\n')
   s.write(b'print("")\n')
   s.write(b'for i in files:\n')
   s.write(b'   print(i)\n')
   s.write(b'\x04') # ^d exit raw mode and execute
   time.sleep(cmd_delay)
   while (s.in_waiting > 0): # show output
      sys.stdout.write(s.read().decode('utf-8'))
   print('')
elif (sys.argv[2] == 'put'):
   file = open(sys.argv[3],'r')
   str = file.read()
   file.close()
   if (len(sys.argv) == 4):
      s.write(bytes(f'file = open("{sys.argv[3]}","w")\n','ascii'))
   elif (len(sys.argv) == 5):
      s.write(bytes(f'file = open("{sys.argv[4]}","w")\n','ascii'))
   s.write(bytes(f'file.write("""','ascii'))
   for i in range(len(str)):
      c = str[i]
      s.write(bytes(f'{c}','ascii'))
      if (c == '\\'):
         s.write(b'\\')
      time.sleep(char_delay)
   s.write(bytes(f'""")\n','ascii'))
   s.write(b'file.close()\n')
   s.write(b'\x04') # ^d exit raw mode and execute
   time.sleep(cmd_delay)
   while (s.in_waiting > 0): # show output and quit
      sys.stdout.write(s.read().decode('utf-8'))
   print('')
elif (sys.argv[2] == 'get'):
   s.write(bytes(f'file = open("{sys.argv[3]}","r")\n','ascii'))
   s.write(b'while (True):\n')
   s.write(b'   ret = file.read(1)\n')
   s.write(b'   if (ret != ""):\n')
   s.write(b'      print(ret,end="")\n')
   s.write(b'   else:\n')
   s.write(b'      break\n')
   s.write(b'file.close()\n')
   s.write(b'\x04') # ^d exit raw mode and execute
   time.sleep(cmd_delay)
   while (True):
      sys.stdout.write(s.read().decode('ascii'))
elif (sys.argv[2] == 'rm'):
   s.write(b'import os\n')
   s.write(bytes(f'os.remove("{sys.argv[3]}")\n','ascii'))
   s.write(b'\x04') # ^d exit raw mode and execute
   time.sleep(cmd_delay)
   while (s.in_waiting > 0): # show output and quit
      sys.stdout.write(s.read().decode('utf-8'))
