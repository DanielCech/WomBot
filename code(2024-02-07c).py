# from adafruit_clue import clue
# import board
# import busio
# import adafruit_pca9685
# from adafruit_servokit import ServoKit
# import adafruit_motor.servo

# i2c = busio.I2C(board.SCL, board.SDA)
# pca = adafruit_pca9685.PCA9685(i2c)
# kit = ServoKit(channels=16)

# pca.frequency = 50


# servo = adafruit_motor.servo.Servo(servo_channel)

# kit.servo[0].angle = 180

# clue_data = clue.simple_text_display(title="WomBot 1.0", title_scale=2)

# while True:
#     clue_data[0].text = "Acceleration: {:.2f} {:.2f} {:.2f}".format(*clue.acceleration)
#     clue_data[1].text = "Gyro: {:.2f} {:.2f} {:.2f}".format(*clue.gyro)
#     clue_data[2].text = "Magnetic: {:.3f} {:.3f} {:.3f}".format(*clue.magnetic)
#     clue_data.show()


import time

from board import SCL, SDA
import busio

# Import the PCA9685 module. Available in the bundle and here:
#   https://github.com/adafruit/Adafruit_CircuitPython_PCA9685
from adafruit_motor import servo
from adafruit_pca9685 import PCA9685

i2c = busio.I2C(SCL, SDA)

# Create a simple PCA9685 class instance.
pca = PCA9685(i2c)
# You can optionally provide a finer tuned reference clock speed to improve the accuracy of the
# timing pulses. This calibration will be specific to each board and its environment. See the
# calibration.py example in the PCA9685 driver.
# pca = PCA9685(i2c, reference_clock_speed=25630710)
pca.frequency = 50

# To get the full range of the servo you will likely need to adjust the min_pulse and max_pulse to
# match the stall points of the servo.
# This is an example for the Sub-micro servo: https://www.adafruit.com/product/2201
# servo7 = servo.Servo(pca.channels[7], min_pulse=580, max_pulse=2350)
# This is an example for the Micro Servo - High Powered, High Torque Metal Gear:
#   https://www.adafruit.com/product/2307
# servo7 = servo.Servo(pca.channels[7], min_pulse=500, max_pulse=2600)
# This is an example for the Standard servo - TowerPro SG-5010 - 5010:
#   https://www.adafruit.com/product/155
# servo7 = servo.Servo(pca.channels[7], min_pulse=400, max_pulse=2400)
# This is an example for the Analog Feedback Servo: https://www.adafruit.com/product/1404
# servo7 = servo.Servo(pca.channels[7], min_pulse=600, max_pulse=2500)
# This is an example for the Micro servo - TowerPro SG-92R: https://www.adafruit.com/product/169
# servo7 = servo.Servo(pca.channels[7], min_pulse=500, max_pulse=2400)

# The pulse range is 750 - 2250 by default. This range typically gives 135 degrees of
# range, but the default is to use 180 degrees. You can specify the expected range if you wish:
# servo7 = servo.Servo(pca.channels[7], actuation_range=135)
servo7 = servo.Servo(pca.channels[7])

# We sleep in the loops to give the servo time to move into position.
for i in range(180):
    servo7.angle = i
    time.sleep(0.03)
for i in range(180):
    servo7.angle = 180 - i
    time.sleep(0.03)

# You can also specify the movement fractionally.
fraction = 0.0
while fraction < 1.0:
    servo7.fraction = fraction
    fraction += 0.01
    time.sleep(0.03)

pca.deinit()


# class PCA9685(object):

#     def __init__(self, i2c, address = 0x40):
#         self.address = address
#         i2c.write(self.address, bytearray([0x00, 0x00]))
#         i2c.write(self.address, bytearray([0x01, 0x04]))
#         i2c.write(self.address, bytearray([0x00, 0x01]))
#         sleep(5)
#         i2c.write(self.address, bytearray([0x00]))
#         mode1 = i2c.read(self.address, 1)
#         mode1 = ustruct.unpack('<H', mode1)[0]
#         mode1 = mode1 & ~0x10  # wake up (reset sleep)
#         i2c.write(self.address, bytearray([0x00, mode1]))
#         sleep(5)

#     def set_pwm_freq(self, freq_hz):
#         prescaleval = 25000000.0  # 25MHz
#         prescaleval /= 4096.0  # 12-bit
#         prescaleval /= float(freq_hz)
#         prescaleval -= 1
#         prescale = int(math.floor(prescaleval + 0.5))
#         i2c.write(self.address, bytearray([0x00]))
#         oldmode = i2c.read(self.address, 1)
#         oldmode = ustruct.unpack('<H', oldmode)[0]
#         newmode = (oldmode & 0x7F) | 0x10  # sleep
#         i2c.write(self.address, bytearray([0x00, newmode]))  # go to sleep
#         i2c.write(self.address, bytearray([0xFE, prescale]))
#         i2c.write(self.address, bytearray([0x00, oldmode]))
#         sleep(5)
#         i2c.write(self.address, bytearray([0x00, oldmode | 0x80]))

#     def set_pwm(self, channel, on, off):
#         if on is None or off is None:
#             i2c.write(self.address, bytearray([0x06 + 4 * channel]))
#             distance = i2c.read(self.address, 4)
#             return ustruct.unpack('<HH', distance)
#         i2c.write(self.address, bytearray([0x06 + 4 * channel, on & 0xFF]))
#         i2c.write(self.address, bytearray([0x07 + 4 * channel, on >> 8]))
#         i2c.write(self.address, bytearray([0x08 + 4 * channel, off & 0xFF]))
#         i2c.write(self.address, bytearray([0x09 + 4 * channel, off >> 8]))

#     def set_all_pwm(self, on, off):
#         i2c.write(self.address, bytearray([0xFA, on & 0xFF]))
#         i2c.write(self.address, bytearray([0xFB, on >> 8]))
#         i2c.write(self.address, bytearray([0xFC, off & 0xFF]))
#         i2c.write(self.address, bytearray([0xFD, off >> 8]))

# from microbit import sleep, pin0, pin1, pin2, pin8, pin12, pin13, pin14, pin15

# pins = {'P0': pin0, 'P1': pin1, 'P2': pin2, 'P8': pin8, 'P12': pin12, 'P13': pin13, 'P14': pin14, 'P15': pin15}

# def get_UTdistance(trig, echo):
# 	pins[echo].write_digital(0)
# 	utime.sleep_us(2)
# 	pins[trig].write_digital(1)
# 	utime.sleep_us(15)
# 	pins[trig].write_digital(0)
# 	while(pins[echo].read_digital() == 0):
# 		pass
# 	time_start = utime.ticks_us()
# 	while pins[echo].read_digital():
# 		pass
# 	distance = ((utime.ticks_us() - time_start) / 10000) * 340 / 2
# 	distance = [distance, 300][distance > 300]
# 	return distance


# led = digitalio.DigitalInOut(board.LED)
# led.direction = digitalio.Direction.OUTPUT

# while True:
#     print("Hello, CircuitPython!")
#     led.value = True
#     time.sleep(1)
#     led.value = False
#     time.sleep(1)
