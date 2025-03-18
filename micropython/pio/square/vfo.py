import rp2
from machine import Pin

@rp2.asm_pio(set_init=rp2.PIO.OUT_LOW)
def vfo():
    set(pins, 1)
    set(pins, 0)

sm = rp2.StateMachine(0, vfo, freq=2500000, set_base=Pin(0))
sm.active(1)
