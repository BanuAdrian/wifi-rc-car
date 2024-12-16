# Wi-Fi Remote Controlled Car

## Introduction

This project represents a four-wheeled car controlled via Wi-Fi, implementing various functions such as a horn, acceleration sound, obstacle avoidance, headlights, taillights, and more.

The idea is rooted in my childhood passion for toy cars, with my favorites being those that could be controlled remotely. I often imagined the moment when I would get my driver’s license and be able to drive in real life. However, I would feel sad whenever the remote control broke, but it gave me the opportunity to take the cars apart using a screwdriver and admire the components inside, without necessarily understanding them.

I believe this project is valuable because it offers a great way to turn a childhood passion into a tangible creation that blends nostalgia with innovation. For others, it could serve as an inspiration to bring their ideas to life. Additionally, since this project is essentially a toy, it has the potential to bring excitement and joy for those who interact with it, no matter their age.

## Overview

### Block diagram

![diagram (1)](https://github.com/user-attachments/assets/4f947def-dae7-4112-96e2-62c6a26fe44a)

### Car Features

- It can be controlled via Wi-Fi, either on a phone or a laptop. 
- It uses 4 TT Gear Motors connected to the L298N Driver Module, which is powered by 2x Li-Ion 18650 batteries. The driver uses 6 pins of the ESP32, 2 of which need to be connected to PWM pins (EN1 and EN2 pins, which control the speed of the motors). As a note, the first 4 pins of the board from the left side (34, 35, 36, 39) are input only pins and can't be configured as output, so I used the pins starting from the next available one (32). 
- Due to the 5V regulator of the module, it can output 5V, which is used not only for the ESP32 board, but also for other modules. 
- Its steering works similarly to the tank steering system (i.e. accelerating one side while the opposite is standing still or reversing). 
- It uses the GP2Y0A21YK0F Infrared Distance Sensor for obstacle avoidance (i.e. it will reverse when it detects a potential collision), which is connected to ADC channel 1 (pin 35). As a note, ADC channel 2 is used by the Wi-Fi driver of the ESP32, so those pins can't be used for this purpose.
- It has a speaker attached to it, so it can play different sounds (horn, acceleration etc.). In order to do so, the microcontroller is connected to a Mini Audio Amplifier based on PAM8403 using DAC1 (pin 25) and, in order to store sounds files, also communicates with a MicroSD module using SPI (pins 5 - CS, 18 - SCK, 19 - MISO, 23 - MOSI).
- It has taillights and headlights represented by two pairs of LEDs. Only two pins are needed (pins 16, 17) since the respective lights will turn on and off simultaneously.



## Hardware Design

### Bill of Material

|#   | Name   | Quantity | Datasheet   |
|:---:|---|:---:|:---:|
|1   |  [ESP32 DevKit V1](https://www.sigmanortec.ro/placa-dezvoltare-esp32-cu-wifi-si-bluetooth) | 1  |  [link](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf)  |
| 2  |  [Dual H-Bridge Motor Driver L298N](https://www.sigmanortec.ro/Punte-H-Dubla-L298N-p125423236) | 1  | [link](https://www.st.com/resource/en/datasheet/l298.pdf)  |
| 3  |  [TT Gear Motor with Wheels [1:48 Gear Ratio]](https://www.sigmanortec.ro/Kit-Motor-reductor-Roata-plastic-cu-cauciuc-p134585625) | 4   |  N/A  |
| 4  |  [Li-Ion 18650 Battery, 3.7V, 2200mAh](https://www.dedeman.ro/ro/acumulator-li-ion-well-18650-3-7v-2200-mah/p/1050265) | 2   |    N/A |
| 5  |  [Li-Ion 18650 Battery Case with On/Off Switch](https://www.sigmanortec.ro/Suport-baterie-18650-2S-cu-capac-si-intrerupator-p192040353) | 1   |    N/A |
| 6  |  GP2Y0A21YK0F Infrared Distance Sensor | 1   | [link](https://global.sharp/products/device/lineup/data/pdf/datasheet/gp2y0a21yk_e.pdf)  | 
| 7  |  [MicroSD Module](https://www.sigmanortec.ro/Modul-MicroSD-p126079625) | 1      | N/A|
| 8  |  Mini Audio Amplifier PAM8403 | 1  |  [link](https://www.mouser.com/datasheet/2/115/PAM8403-247318.pdf?srsltid=AfmBOoqdyrw5H8aEVmHnLDsAMdP0bwbvcuJrRvT6vCjeOBP84tMt8KNv) |
| 9  |  2.5W, 4 Ohm Speaker | 1  | N/A |
| 10  |  Red LED | 2   | N/A |
| 11  |  Clear Blue LED | 2  | N/A |
| 12  |  220 Ohm Resistor | 4  | N/A |

### Circuit diagram

![electric-scheme_bb](https://github.com/user-attachments/assets/dc22b993-3e45-496c-b994-65c141c7eba9)


## Software Design

I will be using PlatformIO IDE for VSCode.

## Results

## Conclusions

## Logs

- 24.11.2024 - ordered the components
- 4.12.2024 - soldered wires on the motors terminals & made 2x splitters for 5V and GND connections from a stacking header
- 8.12.2024 - tested the motors with a demo code
- 9.12.2024 - started to design the 3D model of the car's chassis
- 10.12.2024 - tested the IR distance sensor using a multimeter to make sure its output voltage stays below 3V3 and then with a demo code
- 11.12.2024 - 3D printed the chassis of the car & soldered the wires of the IR distance sensor so they can be used with jumpers
- 12.12.2024 - 3D printed the mounting brackets for the motors & made 2x Y female jumpers & soldered the LEDs with resistors and jumpers
- 13.12.2024 - tested the LEDs, MicroSD, Speaker with demo codes
- 15.12.2024 - bought M3 screws and nuts then mounted the motors on the chassis and verified the car's look by attaching its body:  <br><br><img src="https://github.com/user-attachments/assets/495706b2-3ac9-4ed4-bee5-4a270597b945" width=350 height=500>

## Bibliography/Resources
