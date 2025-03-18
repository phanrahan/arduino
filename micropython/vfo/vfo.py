import rp2
from machine import Pin

@rp2.asm_pio(set_init=rp2.PIO.OUT_LOW)
def vfo():
    wrap_target()
    set(pins, 1)
    set(pins, 0)
    wrap()

sm = rp2.StateMachine(0, blink, freq=1000000, set_base=Pin(1))
sm.active(1)
