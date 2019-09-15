# This is your main script.


print("Hello, world!")

import machine
import framebuf
import st7735
import time

#spi = machine.SPI(2, baudrate=2000000)
spi = machine.SPI(2)
#spi.init()
repl_button = machine.Pin(15, machine.Pin.OUT)
display = st7735.ST7735R(spi, machine.Pin(27, machine.Pin.OUT), machine.Pin(14, machine.Pin.OUT), machine.Pin(33, machine.Pin.OUT), 160, 80)
display.init()
display.fill(0xffffff)

print("EOF")
time.sleep(1000*1000)
print("EOF")

"""
import math
>>> 
>>> 
>>> 
>>> import machine
>>> 
>>> spi

>>> i
>>> dir()
['machine', 'gc', '__name__', 'bdev', 'uos', 'math', 'st7735', 'spi']
>>> display = st7735.ST7735(spi, machine.Pin(27),machine.Pin(14),machine.Pin(33), 160,80)
>>> dir(display)
['__class__', '__init__', '__module__', '__qualname__', '__dict__', 'cs', 'fill', 'hline', 'init', 'pixel', 'reset', 'vline', 'width', 'spi', '_COLUMN_SET', '_PAGE_SET', '_RAM_WRITE', '_RAM_READ', '_INIT', '_ENCODE_PIXEL', '_ENCODE_POS', 'dc', 'rst', 'height', '_write', '_DECODE_PIXEL', '_block', '_encode_pos', '_read', '_encode_pixel', '_decode_pixel', 'fill_rectangle', 'blit_buffer']
>>> display.fill(255)
[0;31mE (483950) spi_master: check_trans_valid(1113): trans tx_buffer should be NULL and SPI_TRANS_USE_TXDATA should be cleared to skip MOSI phase.[0m
>>> display.init()
>>> 
>>> display.fill(255)
[0;31mE (640570) spi_master: check_trans_valid(1113): trans tx_buffer should be NULL and SPI_TRANS_USE_TXDATA should be cleared to skip MOSI phase.[0m
>>> spi.init()
>>> display.init()
>>> display.fill(255) 
[0;31mE (1268240) spi_master: check_trans_valid(1113): trans tx_buffer should be NULL and SPI_TRANS_USE_TXDATA should be cleared to skip MOSI phase.[0m
>>> display.fill(255) 
[0;31mE (1274680) spi_master: check_trans_valid(1113): trans tx_buffer should be NULL and SPI_TRANS_USE_TXDATA should be cleared to skip MOSI phase.[0m
>>> display.fill(255) 
[0;31mE (1276110) spi_master: check_trans_valid(1113): trans tx_buffer should be NULL and SPI_TRANS_USE_TXDATA should be cleared to skip MOSI phase.[0m
>>> 
"""