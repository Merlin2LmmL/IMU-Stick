# IMU-Stick
# Hardware
## Use Cases
When thinking abiout a new Project I remembered my Studica Robot, that i assembled and wrote software for. I was currently on the way of developing SLAM for the Robot using a LiDAR Sensor to allow for automomos sloving of tasks in the Robo Cup. I remembered, that pseudo IMU data can be generated using the cmd_vel ROS2 topic but those are often very unreliable due to drifting. That lead to the idea of buying a IMU Sensor. But what's better than buying someting? Correct! Building it yourself! So I began to think about what form factor I wanted the sensor to be. I figured that there is no real benefit of thinking about where to place the Sensor, since the motion is everywhere the same inside the robot so i chose the smallest possible form factor: Plugging the Sensor right into the Raspberry Pi 5, thats powering the Robot.

## Components
### Chip
For a chip I chose the all mighty rp2040 since its very easy to use and very popular. I obviously couldn't use an entire microcontroller like the rp zero or the XIOA rp2040, because they all already have a USB Port, which is useless for me and there is no microcontroller with a USB A Plug.

### USB Interface
Since the Sensor is going to be plugged into a Raspberry Pi 5, the only real option is USB A, because all the IO ports are occupied by the HAT that powers the Motorcontroller. USB A would also allow for universal use with basically any device. I concluded to USB 2.0 since 3. would be ovekill and only elevate the price of the PCB.

### IMU Senor
I decided to use the ICM-45686 in Single Interface Mode (SPI Interface to Host) since it is fits here really well and allows me to easily communicate with the Sensor over SPI and make programming the firmware later easier.

### Flash Memory
Since we're using a bare chip here, we obviously need flash memory to store the firmware onto it. There is not really much to say on why i settled for the W25Q128JVS because there are no real differences in Flash Memories.

## Pictures
### Schematic: 
<img width="1608" height="1110" alt="image" src="https://github.com/user-attachments/assets/d1088152-8eb0-44e9-9693-d19b12f7da93" />

### PCB
<img width="1305" height="539" alt="image" src="https://github.com/user-attachments/assets/35a09891-5cc4-47d8-98d2-4c583176b211" />

### Case
<img width="1655" height="1097" alt="Screenshot 2026-01-05 172002" src="https://github.com/user-attachments/assets/61cf0380-802f-4761-89aa-21fb4930cae5" />

Also Note, that I purposely added a clutch powered way to open and close the Case. I plan on maybe gluing the top and bottom part of the case together, if they end up not having enough clutching power, because there is no real need in opening the case.
<img width="1734" height="1006" alt="Screenshot 2026-01-05 172024" src="https://github.com/user-attachments/assets/9b9dc81a-b5f8-4551-a07c-345b354e23bf" />

# Software
## Firmware
I wrote some Firmware for the Stick to send the motion data to the device its plugged in. Acess the firmware here: "firmware/imu_usb.c"

## Software use
Now to read the Sensor data, there are mltiple options. One is to immediately recieve the data via the terminal by executing this bash command:
```bash
screen /dev/ttyACM0 115200
```
or
```bash
cat /dev/ttyACM0
```
Another way would be to use a simple python script to read out the data like so:
```python
import serial

ser = serial.Serial('/dev/ttyACM0', 115200)

while True:
    line = ser.readline().decode().strip()
    ax, ay, az, gx, gy, gz = map(int, line.split(','))
    print(ax, ay, az, gx, gy, gz)
```
And using that same principle you can also publish the data onto a ROS2 topich, which is the way that I'm going to use it.

# BOM.csv
| Designator | Footprint | Quantity | Value | LCSC Part # |
|------------|-----------|----------|-------|-------------|
| C10, C9 | 0603 | 2 | 10uF | |
| C11, C13, C14, C15, C16, C17, C18, C19, C20, C21, C22 | 0603 | 11 | 0.1uF | |
| C12, C23 | 0603 | 2 | 1uF | |
| C2, C3 | 0603 | 2 | 100nF | |
| C7, C8 | 0603 | 2 | 15pF | |
| J1 | USB_A_CNCTech_1001-011-01101_Horizontal | 1 | USB_A | |
| R11, R12 | 0603 | 2 | 1kR | |
| R13 | 0603 | 1 | 10KR | |
| R19, R9 | 0603 | 2 | 27R | |
| SW1 | SW_Push_SPST_NO_Alps_SKRK | 1 | SW_Push | |
| U1 | QFN-56-1EP_7x7mm_P0.4mm_EP3.2x3.2mm | 1 | RP2040 | |
| U2 | LGA-14_L3.0-W2.5-P0.50-TL | 1 | ICM-45686 | C22459454 |
| U6 | SOT-223-3_TabPin2 | 1 | NCP1117-3.3_SOT223 | |
| U7 | SOIC-8_5.3x5.3mm_P1.27mm | 1 | W25Q128JVS | |
| Y1 | Crystal_SMD_SeikoEpson_TSX3225-4Pin_3.2x2.5mm | 1 | 12 MHz | |
