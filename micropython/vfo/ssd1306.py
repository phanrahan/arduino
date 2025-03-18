# MicroPython SSD1306 OLED driver, I2C and SPI interfaces
# Based on https://github.com/stlehmann/micropython-ssd1306/blob/master/ssd1306.py
# Modifications based on https://github.com/nickpmulder/ssd1306big (a new, larger font was drawn using framebuffer lines by Nick Mulder)
# Known to work on ESP32 and RP2040

from micropython import const
import framebuf


# register definitions
SET_CONTRAST = const(0x81)
SET_ENTIRE_ON = const(0xA4)
SET_NORM_INV = const(0xA6)
SET_DISP = const(0xAE)
SET_MEM_ADDR = const(0x20)
SET_COL_ADDR = const(0x21)
SET_PAGE_ADDR = const(0x22)
SET_DISP_START_LINE = const(0x40)
SET_SEG_REMAP = const(0xA0)
SET_MUX_RATIO = const(0xA8)
SET_COM_OUT_DIR = const(0xC0)
SET_DISP_OFFSET = const(0xD3)
SET_COM_PIN_CFG = const(0xDA)
SET_DISP_CLK_DIV = const(0xD5)
SET_PRECHARGE = const(0xD9)
SET_VCOM_DESEL = const(0xDB)
SET_CHARGE_PUMP = const(0x8D)

# Subclassing FrameBuffer provides support for graphics primitives
# http://docs.micropython.org/en/latest/pyboard/library/framebuf.html
class SSD1306(framebuf.FrameBuffer):
    def __init__(self, width, height, external_vcc):
        self.width = width
        self.height = height
        self.external_vcc = external_vcc
        self.pages = self.height // 8
        self.buffer = bytearray(self.pages * self.width)
        super().__init__(self.buffer, self.width, self.height, framebuf.MONO_VLSB)
        self.init_display()

    def init_display(self):
        for cmd in (
            SET_DISP | 0x00,  # off
            # address setting
            SET_MEM_ADDR,
            0x00,  # horizontal
            # resolution and layout
            SET_DISP_START_LINE | 0x00,
            SET_SEG_REMAP | 0x01,  # column addr 127 mapped to SEG0
            SET_MUX_RATIO,
            self.height - 1,
            SET_COM_OUT_DIR | 0x08,  # scan from COM[N] to COM0
            SET_DISP_OFFSET,
            0x00,
            SET_COM_PIN_CFG,
            0x02 if self.width > 2 * self.height else 0x12,
            # timing and driving scheme
            SET_DISP_CLK_DIV,
            0x80,
            SET_PRECHARGE,
            0x22 if self.external_vcc else 0xF1,
            SET_VCOM_DESEL,
            0x30,  # 0.83*Vcc
            # display
            SET_CONTRAST,
            0xFF,  # maximum
            SET_ENTIRE_ON,  # output follows RAM contents
            SET_NORM_INV,  # not inverted
            # charge pump
            SET_CHARGE_PUMP,
            0x10 if self.external_vcc else 0x14,
            SET_DISP | 0x01,
        ):  # on
            self.write_cmd(cmd)
        self.fill(0)
        self.show()

    def poweroff(self):
        self.write_cmd(SET_DISP | 0x00)

    def poweron(self):
        self.write_cmd(SET_DISP | 0x01)

    def contrast(self, contrast):
        self.write_cmd(SET_CONTRAST)
        self.write_cmd(contrast)

    def invert(self, invert):
        self.write_cmd(SET_NORM_INV | (invert & 1))

    def show(self):
        x0 = 0
        x1 = self.width - 1
        if self.width == 64:
            # displays with width of 64 pixels are shifted by 32
            x0 += 32
            x1 += 32
        self.write_cmd(SET_COL_ADDR)
        self.write_cmd(x0)
        self.write_cmd(x1)
        self.write_cmd(SET_PAGE_ADDR)
        self.write_cmd(0)
        self.write_cmd(self.pages - 1)
        self.write_data(self.buffer)

    def clear(self):
        self.fill(0)

    def wrap(self, text, x, y, font_size=2, fill=1):
        # display.wrap("Hello World",2,8,2,1)

        char_function_map = {
            'A': A, 'a': A,
            'B': B, 'b': B,
            'C': C, 'c': C,
            'D': D, 'd': D,
            'E': E, 'e': E,
            'F': F, 'f': F,
            'G': G, 'g': G,
            'H': H, 'h': H,
            'I': I, 'i': I,
            'J': J, 'j': J,
            'K': K, 'k': K,
            'L': L, 'l': L,
            'M': M, 'm': M,
            'N': N, 'n': N,
            'O': O, 'o': O,
            'P': P, 'p': P,
            'Q': Q, 'q': Q,
            'R': R, 'r': R,
            'S': S, 's': S,
            'T': T, 't': T,
            'U': U, 'u': U,
            'V': V, 'v': V,
            'W': W, 'w': W,
            'X': X, 'x': X,
            'Y': Y, 'y': Y,
            'Z': Z, 'z': Z,
            '0': zero,
            '1': one,
            '2': two,
            '3': three,
            '4': four,
            '5': five,
            '6': six,
            '7': seven,
            '8': eight,
            '9': nine,
            '.': period,
            '!': exclam,
            '?': question,
            '/': slash,
            ':': colon,
            ',': comma,
            '&': amp,
            '+': plus,
            '-': minus,
            '=': equal,
        }

        # Loop through each character in the text and call the corresponding function
        for i in range(len(text)):
            char = text[i]
            if char in char_function_map:
                char_function_map[char](x, y, self, font_size, fill)
            x = x + (font_size * 5)

    def bold_wrap(self, text, x, y, font_size=2, fill=1):
        for i in range(font_size):
            self.wrap(text, x+i, y, font_size, fill)
            self.wrap(text, x, y+i, font_size, fill)

    def overlap_wrap(self, text, x, y, font_size=2, fill=1, overlap=2):
            self.wrap(text, x, y, font_size, fill)
            if overlap >= 2:
                self.wrap(text, x+1, y+1, font_size, fill)
            if overlap >= 1:
                self.wrap(text, x+2, y+2, font_size, fill)

    def bold_text(self, text, x, y, fill=1):
        self.text(text, x + 1, y, fill)
        self.text(text, x, y, fill)

    
class SSD1306_I2C(SSD1306):
    def __init__(self, width, height, i2c, addr=0x3C, external_vcc=False):
        self.i2c = i2c
        self.addr = addr
        self.temp = bytearray(2)
        self.write_list = [b"\x40", None]  # Co=0, D/C#=1
        super().__init__(width, height, external_vcc)

    def write_cmd(self, cmd):
        self.temp[0] = 0x80  # Co=1, D/C#=0
        self.temp[1] = cmd
        self.i2c.writeto(self.addr, self.temp)

    def write_data(self, buf):
        self.write_list[1] = buf
        self.i2c.writevto(self.addr, self.write_list)


class SSD1306_SPI(SSD1306):
    def __init__(self, width, height, spi, dc, res, cs, external_vcc=False):
        self.rate = 10 * 1024 * 1024
        dc.init(dc.OUT, value=0)
        res.init(res.OUT, value=0)
        cs.init(cs.OUT, value=1)
        self.spi = spi
        self.dc = dc
        self.res = res
        self.cs = cs
        import time

        self.res(1)
        time.sleep_ms(1)
        self.res(0)
        time.sleep_ms(10)
        self.res(1)
        super().__init__(width, height, external_vcc)

    def write_cmd(self, cmd):
        self.spi.init(baudrate=self.rate, polarity=0, phase=0)
        self.cs(1)
        self.dc(0)
        self.cs(0)
        self.spi.write(bytearray([cmd]))
        self.cs(1)

    def write_data(self, buf):
        self.spi.init(baudrate=self.rate, polarity=0, phase=0)
        self.cs(1)
        self.dc(1)
        self.cs(0)
        self.spi.write(buf)
        self.cs(1)

# The Alphabets

def A(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 5),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 5),int(y + font_size * 1),int(x + font_size * 10),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 3),int(y + font_size * 11),int(x + font_size * 8),int(y + font_size * 11),fill)

    
    
def B(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 6),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 6),int(y + font_size * 1),int(x + font_size * 8),int(y + font_size * 3),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 3),int(x + font_size * 8),int(y + font_size * 4),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 4),int(x + font_size * 6),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 5),int(y + font_size * 7),int(x + font_size * 1),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 6),int(y + font_size * 7),int(x + font_size * 9),int(y + font_size * 10),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 10),int(x + font_size * 9),int(y + font_size * 12),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 12),int(x + font_size * 6),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 6),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 15),fill)
        
    
def C(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 10),int(y + font_size * 2),int(x + font_size * 9),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 1),int(x + font_size * 4),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 1),int(x + font_size * 2),int(y + font_size * 3),fill)
    oled.line(int(x + font_size * 2),int(y + font_size * 3),int(x + font_size * 1),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 7),int(x + font_size * 1),int(y + font_size * 12),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 12),int(x + font_size * 4),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 15),int(x + font_size * 8),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 15),int(x + font_size * 10),int(y + font_size * 13),fill)
    
    
def D(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 6),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 6),int(y + font_size * 1),int(x + font_size * 9),int(y + font_size * 3),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 3),int(x + font_size * 9),int(y + font_size * 12),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 12),int(x + font_size * 6),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 6),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 15),fill)
    
    
def E(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 9),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 7),int(x + font_size * 7),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 9),int(y + font_size * 15),fill)
    
    
def F(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 9),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 7),int(x + font_size * 6),int(y + font_size * 7),fill)
    
    
def G(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 9),int(y + font_size * 2),int(x + font_size * 8),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 1),int(x + font_size * 4),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 1),int(x + font_size * 2),int(y + font_size * 3),fill)
    oled.line(int(x + font_size * 2),int(y + font_size * 3),int(x + font_size * 1),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 7),int(x + font_size * 1),int(y + font_size * 12),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 12),int(x + font_size * 4),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 15),int(x + font_size * 8),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 15),int(x + font_size * 10),int(y + font_size * 13),fill)
    oled.line(int(x + font_size * 10),int(y + font_size * 13),int(x + font_size * 10),int(y + font_size * 9),fill)
    oled.line(int(x + font_size * 10),int(y + font_size * 9),int(x + font_size * 6),int(y + font_size * 9),fill)    
    

def H(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 7),int(x + font_size * 9),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 15),int(x + font_size * 9),int(y + font_size * 1),fill)
    

def I(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 9),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 9),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 5),int(y + font_size * 15),int(x + font_size * 5),int(y + font_size * 1),fill)
    

def J(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 9),int(y + font_size * 1),int(x + font_size * 9),int(y + font_size * 10),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 10),int(x + font_size * 7),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 15),int(x + font_size * 3),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 3),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 10),fill)
    
    
def K(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 9),int(x + font_size * 8),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 7),int(x + font_size * 9),int(y + font_size * 15),fill)
    

def L(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 9),int(y + font_size * 15),fill)
    
    
def M(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 5),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 1),int(x + font_size * 5),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 15),int(x + font_size * 9),int(y + font_size * 1),fill)
    

def N(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 9),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 15),int(x + font_size * 9),int(y + font_size * 1),fill)
    

def O(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 10),int(y + font_size * 5),int(x + font_size * 8),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 1),int(x + font_size * 4),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 1),int(x + font_size * 2),int(y + font_size * 3),fill)
    oled.line(int(x + font_size * 2),int(y + font_size * 3),int(x + font_size * 1),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 7),int(x + font_size * 1),int(y + font_size * 12),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 12),int(x + font_size * 4),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 15),int(x + font_size * 7),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 15),int(x + font_size * 10),int(y + font_size * 12),fill)
    oled.line(int(x + font_size * 10),int(y + font_size * 12),int(x + font_size * 10),int(y + font_size * 5),fill)
    


def P(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 7),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 1),int(x + font_size * 9),int(y + font_size * 4),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 4),int(x + font_size * 9),int(y + font_size * 6),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 6), (x + font_size * 6),int(y + font_size * 9),fill)
    oled.line(int(x + font_size * 5),int(y + font_size * 9),int(x + font_size * 1),int(y + font_size * 9),fill)
     

def Q(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 10),int(y + font_size * 5),int(x + font_size * 8),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 1),int(x + font_size * 4),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 1),int(x + font_size * 2),int(y + font_size * 3),fill)
    oled.line(int(x + font_size * 2),int(y + font_size * 3),int(x + font_size * 1),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 7),int(x + font_size * 1),int(y + font_size * 12),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 12),int(x + font_size * 4),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 15),int(x + font_size * 7),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 15),int(x + font_size * 10),int(y + font_size * 12),fill)
    oled.line(int(x + font_size * 10),int(y + font_size * 12),int(x + font_size * 10),int(y + font_size * 5),fill)
    oled.line(int(x + font_size * 6),int(y + font_size * 10),int(x + font_size * 10),int(y + font_size * 15),fill)
    

def R(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 7),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 1),int(x + font_size * 9),int(y + font_size * 4),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 4),int(x + font_size * 9),int(y + font_size * 6),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 6), (x + font_size * 6),int(y + font_size * 9),fill)
    oled.line(int(x + font_size * 5),int(y + font_size * 9),int(x + font_size * 1),int(y + font_size * 9),fill)
    oled.line(int(x + font_size * 5),int(y + font_size * 9),int(x + font_size * 9),int(y + font_size * 15),fill)
    
    
def S(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 9),int(y + font_size * 2),int(x + font_size * 7),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 1),int(x + font_size * 3),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 3),int(y + font_size * 1),int(x + font_size * 2),int(y + font_size * 2),fill)
    oled.line(int(x + font_size * 3),int(y + font_size * 1),int(x + font_size * 2),int(y + font_size * 2),fill)    
    oled.line(int(x + font_size * 2),int(y + font_size * 2),int(x + font_size * 1),int(y + font_size * 5),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 5),int(x + font_size * 5),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 5),int(y + font_size * 7),int(x + font_size * 9),int(y + font_size * 8),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 8),int(x + font_size * 10),int(y + font_size * 11),fill)
    oled.line(int(x + font_size * 10),int(y + font_size * 11),int(x + font_size * 10),int(y + font_size * 13),fill)
    oled.line(int(x + font_size * 10),int(y + font_size * 13),int(x + font_size * 7),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 15),int(x + font_size * 4),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 13),fill)
    #oled.line(int(x + font_size * 10),int(y + font_size * 13),int(x + font_size * 7),int(y + font_size * 15),fill)
    

def T(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 5),int(y + font_size * 15),int(x + font_size * 5),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 9),int(y + font_size * 1),fill)
    
    
def U(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 1),int(y + font_size * 13),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 13),int(x + font_size * 3),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 3),int(y + font_size * 15),int(x + font_size * 7),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 15),int(x + font_size * 9),int(y + font_size * 13),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 13),int(x + font_size * 9),int(y + font_size * 1),fill)
    

def V(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 5),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 5),int(y + font_size * 15),int(x + font_size * 9),int(y + font_size * 1),fill)
    

def W(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 3),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 3),int(y + font_size * 15),int(x + font_size * 5),int(y + font_size * 8),fill)
    oled.line(int(x + font_size * 5),int(y + font_size * 8),int(x + font_size * 8),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 15),int(x + font_size * 10),int(y + font_size * 1),fill)
    

def X(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 9),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 1),int(x + font_size * 1),int(y + font_size * 15),fill)
    

def Y(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 5),int(y + font_size * 15),int(x + font_size * 5),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 5),int(y + font_size * 7),int(x + font_size * 1),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 5),int(y + font_size * 7),int(x + font_size * 10),int(y + font_size * 1),fill)
    

def Z(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 9),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 9),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 9),int(y + font_size * 15),fill)
    

# The symbols

def period(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 14),int(x + font_size * 2),int(y + font_size * 14),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 2),int(y + font_size * 15),fill)
    

def exclam(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 14),int(x + font_size * 1),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 1),int(y + font_size * 10),fill)
    

def plus(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 5),int(y + font_size * 5),int(x + font_size * 5),int(y + font_size * 11),fill)
    oled.line(int(x + font_size * 2),int(y + font_size * 8),int(x + font_size * 8),int(y + font_size * 8),fill)
    
    
def minus(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 2),int(y + font_size * 8),int(x + font_size * 8),int(y + font_size * 8),fill)
    
    
def equal(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 2),int(y + font_size * 6),int(x + font_size * 8),int(y + font_size * 6),fill)
    oled.line(int(x + font_size * 2),int(y + font_size * 9),int(x + font_size * 8),int(y + font_size * 9),fill)
    

def comma(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 13),int(x + font_size * 1),int(y + font_size * 14),fill)
    oled.line(int(x + font_size * 2),int(y + font_size * 13),int(x + font_size * 2),int(y + font_size * 17),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 17),int(x + font_size * 2),int(y + font_size * 17),fill)
    

def colon(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 14),int(x + font_size * 2),int(y + font_size * 14),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 2),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 6),int(x + font_size * 2),int(y + font_size * 6),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 5),int(x + font_size * 2),int(y + font_size * 5),fill)
    


def slash(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 9),int(y + font_size * 1),int(x + font_size * 1),int(y + font_size * 15),fill)
    
    
def question(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 5),int(y + font_size * 14),int(x + font_size * 6),int(y + font_size * 14),fill)
    oled.line(int(x + font_size * 5),int(y + font_size * 15),int(x + font_size * 6),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 5),int(y + font_size * 10),int(x + font_size * 5),int(y + font_size * 8),fill)
    oled.line(int(x + font_size * 5),int(y + font_size * 8),int(x + font_size * 8),int(y + font_size * 6),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 6),int(x + font_size * 9),int(y + font_size * 2),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 1),int(x + font_size * 4),int(y + font_size * 1),fill)

    


def amp(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    #&
    oled.line(int(x + font_size * 4),int(y + font_size * 7),int(x + font_size * 2),int(y + font_size * 5),fill)
    oled.line(int(x + font_size * 2),int(y + font_size * 5),int(x + font_size * 2),int(y + font_size * 3),fill)
    oled.line(int(x + font_size * 2),int(y + font_size * 3),int(x + font_size * 3),int(y + font_size * 2),fill)
    oled.line(int(x + font_size * 3),int(y + font_size * 2),int(x + font_size * 4),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 1),int(x + font_size * 6),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 6),int(y + font_size * 1),int(x + font_size * 7),int(y + font_size * 2),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 2),int(x + font_size * 8),int(y + font_size * 3),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 3),int(x + font_size * 8),int(y + font_size * 4),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 4),int(x + font_size * 6),int(y + font_size * 6),fill)
    oled.line(int(x + font_size * 6),int(y + font_size * 6),int(x + font_size * 1),int(y + font_size * 10),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 10),int(x + font_size * 1),int(y + font_size * 13),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 13),int(x + font_size * 3),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 3),int(y + font_size * 15),int(x + font_size * 6),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 6),int(y + font_size * 15),int(x + font_size * 9),int(y + font_size * 9),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 8),int(x + font_size * 10),int(y + font_size * 15),fill)
    
    

# The numbers

def zero(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 10),int(y + font_size * 5),int(x + font_size * 8),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 1),int(x + font_size * 4),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 1),int(x + font_size * 2),int(y + font_size * 3),fill)
    oled.line(int(x + font_size * 2),int(y + font_size * 3),int(x + font_size * 1),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 7),int(x + font_size * 1),int(y + font_size * 12),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 12),int(x + font_size * 4),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 15),int(x + font_size * 7),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 15),int(x + font_size * 10),int(y + font_size * 12),fill)
    oled.line(int(x + font_size * 10),int(y + font_size * 12),int(x + font_size * 10),int(y + font_size * 5),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 4),int(x + font_size * 2),int(y + font_size * 12),fill)
    

def one(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 5),int(y + font_size * 15),int(x + font_size * 5),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 5),int(y + font_size * 1),int(x + font_size * 2),int(y + font_size * 3),fill)
    

def two(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 3),int(x + font_size * 2),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 2),int(y + font_size * 1),int(x + font_size * 7),int(y + font_size * 1),fill)    
    oled.line(int(x + font_size * 7),int(y + font_size * 1),int(x + font_size * 9),int(y + font_size * 3),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 3),int(x + font_size * 9),int(y + font_size * 6),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 6),int(x + font_size * 2),int(y + font_size * 13),fill)
    oled.line(int(x + font_size * 2),int(y + font_size * 13),int(x + font_size * 1),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 15),int(x + font_size * 10),int(y + font_size * 15),fill)
    
    

def three(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 3),int(x + font_size * 2),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 2),int(y + font_size * 1),int(x + font_size * 7),int(y + font_size * 1),fill)    
    oled.line(int(x + font_size * 7),int(y + font_size * 1),int(x + font_size * 9),int(y + font_size * 3),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 3),int(x + font_size * 9),int(y + font_size * 5),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 5),int(x + font_size * 7),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 7),int(x + font_size * 4),int(y + font_size * 7),fill)    
    oled.line(int(x + font_size * 7),int(y + font_size * 8),int(x + font_size * 9),int(y + font_size * 9),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 9),int(x + font_size * 9),int(y + font_size * 12),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 12),int(x + font_size * 7),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 15),int(x + font_size * 3),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 3),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 13),fill)    

    

def four(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 8),int(y + font_size * 1),int(x + font_size * 8),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 1),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 7),int(x + font_size * 9),int(y + font_size * 7),fill)
    

def five(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 9),int(y + font_size * 1),int(x + font_size * 1),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 1),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 7),int(x + font_size * 1),int(y + font_size * 7),fill)    
    oled.line(int(x + font_size * 7),int(y + font_size * 8),int(x + font_size * 9),int(y + font_size * 9),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 9),int(x + font_size * 9),int(y + font_size * 12),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 12),int(x + font_size * 7),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 15),int(x + font_size * 3),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 3),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 13),fill)
    

def six(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 10),int(y + font_size * 3),int(x + font_size * 8),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 1),int(x + font_size * 4),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 1),int(x + font_size * 2),int(y + font_size * 3),fill)
    oled.line(int(x + font_size * 2),int(y + font_size * 3),int(x + font_size * 1),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 7),int(x + font_size * 1),int(y + font_size * 12),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 12),int(x + font_size * 4),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 15),int(x + font_size * 7),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 15),int(x + font_size * 10),int(y + font_size * 13),fill)
    oled.line(int(x + font_size * 10),int(y + font_size * 13),int(x + font_size * 10),int(y + font_size * 9),fill)
    oled.line(int(x + font_size * 10),int(y + font_size * 9),int(x + font_size * 8),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 7),int(x + font_size * 4),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 7),int(x + font_size * 2),int(y + font_size * 9),fill)
    
    

def seven(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 1),int(y + font_size * 1),int(x + font_size * 10),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 10),int(y + font_size * 1),int(x + font_size * 3),int(y + font_size * 15),fill)
    
    
def eight(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 4),int(y + font_size * 7),int(x + font_size * 2),int(y + font_size * 5),fill)
    oled.line(int(x + font_size * 2),int(y + font_size * 5),int(x + font_size * 2),int(y + font_size * 3),fill)
    oled.line(int(x + font_size * 2),int(y + font_size * 3),int(x + font_size * 3),int(y + font_size * 2),fill)
    oled.line(int(x + font_size * 3),int(y + font_size * 2),int(x + font_size * 4),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 1),int(x + font_size * 6),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 6),int(y + font_size * 1),int(x + font_size * 7),int(y + font_size * 2),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 2),int(x + font_size * 8),int(y + font_size * 3),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 3),int(x + font_size * 8),int(y + font_size * 5),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 5),int(x + font_size * 6),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 10),int(x + font_size * 1),int(y + font_size * 13),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 13),int(x + font_size * 3),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 3),int(y + font_size * 15),int(x + font_size * 7),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 15),int(x + font_size * 9),int(y + font_size * 13),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 13),int(x + font_size * 9),int(y + font_size * 10),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 10),int(x + font_size * 6),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 6),int(y + font_size * 7),int(x + font_size * 4),int(y + font_size * 7),fill)
    oled.line(int(x + font_size * 4),int(y + font_size * 7),int(x + font_size * 2),int(y + font_size * 9),fill)
    

def nine(x, y, oled, font_size=2, fill=1):
    font_size = font_size / 3  # divide by 3, which is the smallest visible font size


    oled.line(int(x + font_size * 10),int(y + font_size * 6),int(x + font_size * 8),int(y + font_size * 8),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 8),int(x + font_size * 3),int(y + font_size * 8),fill)
    oled.line(int(x + font_size * 3),int(y + font_size * 8),int(x + font_size * 1),int(y + font_size * 5),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 5),int(x + font_size * 1),int(y + font_size * 3),fill)
    oled.line(int(x + font_size * 1),int(y + font_size * 3),int(x + font_size * 3),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 3),int(y + font_size * 1),int(x + font_size * 8),int(y + font_size * 1),fill)
    oled.line(int(x + font_size * 8),int(y + font_size * 1),int(x + font_size * 10),int(y + font_size * 3),fill)
    oled.line(int(x + font_size * 10),int(y + font_size * 3),int(x + font_size * 10),int(y + font_size * 10),fill)
    oled.line(int(x + font_size * 10),int(y + font_size * 10),int(x + font_size * 9),int(y + font_size * 13),fill)
    oled.line(int(x + font_size * 9),int(y + font_size * 13),int(x + font_size * 7),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 7),int(y + font_size * 15),int(x + font_size * 3),int(y + font_size * 15),fill)
    oled.line(int(x + font_size * 3),int(y + font_size * 15),int(x + font_size * 1),int(y + font_size * 13),fill)
