import sys

s = ''
for line in sys.stdin.readlines():
    for char in line:
        ch = ord(char)
        if ch == 0200: continue
        if ch == 0240: continue
        if ch == 0343: continue
        if ch == 0302: continue
        s += char

print s
