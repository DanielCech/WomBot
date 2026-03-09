
import time
import board
import busio
from adafruit_clue import clue

i2c = busio.I2C(board.P0, board.P1)
pins = {'P0': clue._p0, 'P1': clue._p1, 'P2': clue._p2, 'P8': clue._p8, 'P12': clue._p12, 'P13': clue._p13, 'P14': clue._p14, 'P15': clue._p15}

class PCA9685(object):

    def __init__(self, i2c, address=0x40):
        self.address = address
        self.i2c = i2c
        self.i2c.writeto(self.address, bytearray([0x00, 0x00]))
        self.i2c.writeto(self.address, bytearray([0x01, 0x04]))
        self.i2c.writeto(self.address, bytearray([0x00, 0x01]))
        time.sleep(0.005)
        self.i2c.writeto(self.address, bytearray([0x00]))
        mode1 = self.i2c.readfrom(self.address, 1)
        mode1 = int.from_bytes(mode1, "little")
        mode1 = mode1 & ~0x10  # wake up (reset sleep)
        self.i2c.writeto(self.address, bytearray([0x00, mode1]))
        time.sleep(0.005)

    def set_pwm_freq(self, freq_hz):
        prescaleval = 25000000.0  # 25MHz
        prescaleval /= 4096.0  # 12-bit
        prescaleval /= float(freq_hz)
        prescaleval -= 1
        prescale = int(math.floor(prescaleval + 0.5))
        self.i2c.writeto(self.address, bytearray([0x00]))
        oldmode = self.i2c.readfrom(self.address, 1)
        oldmode = int.from_bytes(oldmode, "little")
        newmode = (oldmode & 0x7F) | 0x10  # sleep
        self.i2c.writeto(self.address, bytearray([0x00, newmode]))  # go to sleep
        self.i2c.writeto(self.address, bytearray([0xFE, prescale]))
        self.i2c.writeto(self.address, bytearray([0x00, oldmode]))
        time.sleep(0.005)
        self.i2c.writeto(self.address, bytearray([0x00, oldmode | 0x80]))

    def set_pwm(self, channel, on, off):
        if on is None or off is None:
            self.i2c.writeto(self.address, bytearray([0x06 + 4 * channel]))
            distance = self.i2c.readfrom(self.address, 4)
            return int.from_bytes(distance, "little")
        self.i2c.writeto(self.address, bytearray([0x06 + 4 * channel, on & 0xFF]))
        self.i2c.writeto(self.address, bytearray([0x07 + 4 * channel, on >> 8]))
        self.i2c.writeto(self.address, bytearray([0x08 + 4 * channel, off & 0xFF]))
        self.i2c.writeto(self.address, bytearray([0x09 + 4 * channel, off >> 8]))

    def set_all_pwm(self, on, off):
        self.i2c.writeto(self.address, bytearray([0xFA, on & 0xFF]))
        self.i2c.writeto(self.address, bytearray([0xFB, on >> 8]))
        self.i2c.writeto(self.address, bytearray([0xFC, off & 0xFF]))
        self.i2c.writeto(self.address, bytearray([0xFD, off >> 8]))

pwm = PCA9685(i2c)
pwm.set_pwm_freq(50)

def servo(index, degree):   # index:1~8,degree:0~180
    degree = (degree * 10 + 600) * 4096 // 20000
    pwm.set_pwm(index, 0, degree)

def stepper(index, sense_of_rotation):  # index:1~2,sense_of_rotation:0~1
    if index == 2:
        if sense_of_rotation == 0:
            pwm.set_pwm(0, 2047, 4095)
            pwm.set_pwm(2, 1, 2047)
            pwm.set_pwm(1, 1023, 3071)
            pwm.set_pwm(3, 3071, 1023)
        elif sense_of_rotation == 1:
            pwm.set_pwm(3, 2047, 4095)
            pwm.set_pwm(1, 1, 2047)
            pwm.set_pwm(2, 1023, 3071)
            pwm.set_pwm(0, 3071, 1023)
    elif index == 1:
        if sense_of_rotation == 0:
            pwm.set_pwm(4, 2047, 4095)
            pwm.set_pwm(6, 1, 2047)
            pwm.set_pwm(5, 1023, 3071)
            pwm.set_pwm(7, 3071, 1023)
        elif sense_of_rotation == 1:
            pwm.set_pwm(7, 2047, 4095)
            pwm.set_pwm(5, 1, 2047)
            pwm.set_pwm(6, 1023, 3071)
            pwm.set_pwm(4, 3071, 1023)

def stepper_degree(index, degree):  # index:1 ~ 2,degree:-360 ~ 360
    stepper(index, degree > 0)
    degree = abs(degree)
    time.sleep(10240 * degree / 360)
    for i in range(0, 16):
        pwm.set_pwm(i, 0, 0)

def motor_run(index, speed):  # index:1~4,speed:-250~250
    speed = speed * 16
    if speed >= 4096:
        speed = 4095
    if speed <= -4096:
        speed = -4095
    if index > 4 or index <= 0:
        return
    if index == 1:
        index = 3
    elif index == 2:
        index = 4
    elif index == 3:
        index = 1
    elif index == 4:
        index = 2
    pp = (index - 1) * 2
    pn = (index - 1) * 2 + 1
    if speed >= 0:
        pwm.set_pwm(pp, 0, speed)
        pwm.set_pwm(pn, 0, 0)
    else:
        pwm.set_pwm(pp, 0, 0)
        pwm.set_pwm(pn, 0, -speed)

def stop_pwm_more(start, end):  # start:1~16,end>=start
    for i in range(start-1, end-1):
        pwm.set_pwm(i, 0, 0)

def stop_car():
    stop_pwm_more(1, 9)



servo(1, 90)