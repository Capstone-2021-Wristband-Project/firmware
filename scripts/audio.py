import serial
import wavio
import numpy as np


with open("scripts/audio.txt", "r") as textfile:
    samps = [int(line) for line in textfile.read().split()]
    print(max(samps))
    print(min(samps))
    data = np.asarray(samps)

    wavio.write("scripts/audio.wav", data, 8000, sampwidth=1, scale="none")

# str = ""
# with serial.Serial('/dev/cu.usbserial-12201', 921600, timeout=1) as ser:
#     ser.write(b"audio\n")
#     str = ser.read(12000)
#     samps = [int(val) for val in str.split()[2:]]
#     data = np.asarray(samps)

#     wavio.write("scripts/audio.wav", data, 8000, sampwidth=2)
#     print(samps)
