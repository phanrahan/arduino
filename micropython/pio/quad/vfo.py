import rp2
from machine import Pin

@rp2.asm_pio(set_init=(rp2.PIO.OUT_LOW, rp2.PIO.OUT_LOW))
def vfo():
    set(pins, 0)
    set(pins, 1)
    set(pins, 3)
    set(pins, 2)

sm = rp2.StateMachine(0, vfo, freq=10000000, set_base=Pin(0))
sm.active(1)
