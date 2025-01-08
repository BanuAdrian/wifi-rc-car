# Wi-Fi Remote Controlled Car

## Introduction

This project represents a four-wheeled car controlled via Wi-Fi, implementing various functions such as a horn, acceleration sound, obstacle avoidance, headlights, taillights, and more.

The idea is rooted in my childhood passion for toy cars, with my favorites being those that could be controlled remotely. I often imagined the moment when I would get my driver’s license and be able to drive in real life. However, I would feel sad whenever the remote control broke, but it gave me the opportunity to take the cars apart using a screwdriver and admire the components inside, without necessarily understanding them.

I believe this project is valuable because it offers a great way to turn a childhood passion into a tangible creation that blends nostalgia with innovation. For others, it could serve as an inspiration to bring their ideas to life. Additionally, since this project is essentially a toy, it has the potential to bring excitement and joy for those who interact with it, no matter their age.

## Overview

### Block diagram

![block-diagram](https://github.com/BanuAdrian/wifi-rc-car/blob/main/hardware/block-diagram.png)

### Car Features

- It can be controlled via Wi-Fi, either on a phone or a laptop.
- It is powered by an USB Power Bank which provides 5V 2100 mAh. 
- It uses 4 TT Gear Motors connected to the L298N Driver Module, which is powered by 2x Li-Ion 18650 batteries. The driver uses 6 pins of the ESP32, 2 of which need to be connected to PWM pins (EN1 and EN2 pins, which control the speed of the motors). As a note, the first 4 pins of the board from the left side (34, 35, 36, 39) are input only pins and can't be configured as output, so I used the pins starting from the next available one (32). 
- Due to the 5V regulator of the L298N Driver Module, it can output 5V, which is used for different modules (e.g. the Mini Audio Amplifier PAM8403 or the Infrared Distance Sensor). 
- Its steering works similarly to the tank steering system (i.e. accelerating one side while the opposite is standing still or reversing). 
- It uses the GP2Y0A21YK0F Infrared Distance Sensor for obstacle avoidance (i.e. it will reverse when it detects a potential collision), which is connected to ADC channel 1 (pin 35). As a note, ADC channel 2 is used by the Wi-Fi driver of the ESP32, so those pins can't be used for this purpose.
- It has a speaker attached to it, so it can play different sounds (horn, acceleration, reversing). In order to do so, the microcontroller is connected to a Mini Audio Amplifier based on PAM8403 using DAC1 (pin 25) and, in order to store sounds files, also communicates with a MicroSD module using SPI (pins 5 - CS, 18 - SCK, 19 - MISO, 23 - MOSI). As a note, when audio is being played, connecting any device to DAC2 (pin 26) causes interference, which either disrupts proper audio playback or module functionality. Thus, I won't use it.
- It has taillights and headlights represented by two pairs of LEDs. Only two pins are needed (pins 16, 17) since the respective lights will turn on and off simultaneously.

## Hardware Design

### Bill of Material

|#   | Name   | Quantity | Datasheet   |
|:---:|---|:---:|:---:|
|1   |  [ESP32 DevKit V1](https://www.sigmanortec.ro/placa-dezvoltare-esp32-cu-wifi-si-bluetooth) | 1  |  [link](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf)  |
| 2  |  USB Power Bank 5V 2100mAh | 1  | N/A  |
| 3  |  [Dual H-Bridge Motor Driver L298N](https://www.sigmanortec.ro/Punte-H-Dubla-L298N-p125423236) | 1  | [link](https://www.st.com/resource/en/datasheet/l298.pdf)  |
| 4  |  [TT Gear Motor with Wheels [1:48 Gear Ratio]](https://www.sigmanortec.ro/Kit-Motor-reductor-Roata-plastic-cu-cauciuc-p134585625) | 4   |  N/A  |
| 5  |  [Li-Ion 18650 Battery, 3.7V, 2200mAh](https://www.dedeman.ro/ro/acumulator-li-ion-well-18650-3-7v-2200-mah/p/1050265) | 2   |    N/A |
| 6  |  [Li-Ion 18650 Battery Case with On/Off Switch](https://www.sigmanortec.ro/Suport-baterie-18650-2S-cu-capac-si-intrerupator-p192040353) | 1   |    N/A |
| 7  |  GP2Y0A21YK0F Infrared Distance Sensor | 1   | [link](https://global.sharp/products/device/lineup/data/pdf/datasheet/gp2y0a21yk_e.pdf)  | 
| 8  |  [MicroSD Module](https://www.sigmanortec.ro/Modul-MicroSD-p126079625) | 1      | N/A|
| 9  |  Mini Audio Amplifier PAM8403 | 1  |  [link](https://www.mouser.com/datasheet/2/115/PAM8403-247318.pdf?srsltid=AfmBOoqdyrw5H8aEVmHnLDsAMdP0bwbvcuJrRvT6vCjeOBP84tMt8KNv) |
| 10  |  2.5W, 4 Ohm Speaker | 1  | N/A |
| 11  |  Red LED | 2   | N/A |
| 12  |  Clear Blue LED | 2  | N/A |
| 13  |  220 Ohm Resistor | 4  | N/A |

### Circuit diagram

![electric-scheme](https://github.com/BanuAdrian/wifi-rc-car/blob/main/hardware/electric-scheme.png)


## Software Design

The code is developed using the PlatformIO extension in Visual Studio Code.

### Software Components:
- WebSocket protocol for real-time communication with the user.
- Vehicle control through a web interface.
- Obstacle avoidance algorithm using IR sensor data.
- Synchronized audio playback based on the vehicle's actions.

### Libraries Used

I have used the following libraries:

- WiFi.h: Enables the ESP32 to establish a Wi-Fi Access Point for remote control of the RC car through a web interface.

- AsyncTCP.h: Provides non-blocking TCP communication, ensuring smooth WebSocket and HTTP connections without interfering with other processes.

- ESPAsyncWebServer.h: Sets up an asynchronous web server to serve the RC car's web interface and handle real-time commands via WebSockets.

- SD.h: Interfaces with the SD card to read audio files, enabling the storage and playback of sound effects.

- SPI.h: Facilitates communication with the SD card using the SPI protocol, essential for reading audio files.

- AudioGeneratorWAV.h: Decodes and plays WAV audio files stored on the SD card.

- AudioOutputI2S.h: Transmits audio data via I2S to the Mini Audio Amplifier PAM8403 for sound playback.

- AudioFileSourceSD.h: Reads audio files from the SD card for seamless playback together with AudioGeneratorWAV and AudioOutputI2S libraries.

### Implemented Functions

- setup(): Initializes all hardware components, including motors, lights, audio, and the Wi-Fi access point. It also sets up the WebSocket server and root URL handling.

- loop(): Handles periodic tasks:
    - Plays sounds based on the car’s state (e.g., acceleration, reversing).
    - Detects and avoids obstacles using an IR sensor.

- initWebSocket(): Initializes the WebSocket server and associates it with event handlers for client communication.

- handleWebSocketMessage(void *arg, uint8_t *data, size_t len): Processes commands received via WebSocket, such as movement, feature activation, and toggling specific features.

- onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len): Handles WebSocket connection events, such as client connections, disconnections, and incoming data.

- handleRootRequests(): Serves the HTML page for the car's control interface at the root URL (/).

- setMotorsDirection(uint8_t motors, uint8_t direction): Controls the direction of the left or right motors based on the given command:
    - Move forward
    - Move backward
    - Stop

- setMotorsSpeed(uint8_t speedValue): Sets the speed of the motors using PWM (Pulse Width Modulation).

- moveWheels(uint8_t direction): Handles the car’s movement in the specified direction:
    - Forward
    - Backward
    - Left
    - Right
    - Stop

- activateFeature(uint8_t feature): Activates specific features such as the car horn.

- toggleFeature(uint8_t feature): Toggles features such as:
    - Obstacle avoidance
    - Headlights

- initSDAudio(): Initializes the SD card module and I2S audio output for playing WAV files.

- initLights(): Configures the GPIO pins controlling the headlights and taillights.

- checkAndPlaySound(const char* filePath, bool condition): Plays a specific audio file based on the current state (e.g., acceleration, honking, reversing).

- handleSounds(): Manages sound playback logic for acceleration, horn, and reversing sounds.

- detectAndAvoidObstacles(): Monitors the distance to obstacles using an IR sensor and triggers obstacle avoidance actions if necessary.

- setupMotors(): Initializes motor control pins and sets default states.

### Usage of Microcontroller Functionalities

In this project, the use of functionalities studied in the lab, such as GPIO, SPI, PWM, and ADC, is essential for the implementation and full operation of the remote-controlled car:

- GPIOs (General Purpose Input/Output) are used to control the basic components of the car and interact with various peripherals:
    - Motor control: GPIOs are configured as outputs to control the direction of the wheels through the H-Bridge driver.
        - Example: Pins LEFT_MOTORS_IN1, LEFT_MOTORS_IN2, RIGHT_MOTORS_IN3, RIGHT_MOTORS_IN4.
    - Light control: GPIO pins are used to turn the headlights and taillights on and off.
        - Example: Pins HEADLIGHTS and TAILLIGHTS.
    - IR Sensor: A GPIO configured as an input reads the analog signal generated by the IR sensor to detect obstacles.

- SPI (Serial Peripheral Interface) is used to communicate with the SD card module, which stores the audio files needed for the car's sounds:
    - Reading audio files: The ESP32 uses SPI to access .wav files stored on the SD card. These files are played using the speaker.
        -   Pins used: SD_SCLK (clock), SD_MOSI (data out), SD_MISO (data in), SD_CS (chip select).

- PWM (Pulse Width Modulation) is used to control the speed of the motors by adjusting the voltage applied to them:
    - Wheel speed control: The analogWrite function generates a PWM signal that controls the current sent to the motors, allowing the speed of the car to be adjusted.
        - Example: Pins LEFT_MOTORS_EN and RIGHT_MOTORS_EN.

- ADC (Analog-to-Digital Converter) is used to read the analog value from the IR sensor: 
    - Obstacle detection: The IR sensor returns an analog signal proportional to the distance to an obstacle. The ADC converts this signal into a digital value, which is then used to calculate the distance in centimeters.
        - Example: Pin IR_SENSOR connected to an ESP32 analog pin.

### Interaction Between Functionalities

Each functionality is interconnected and contributes to the overall behavior of the vehicle:

-   Motor Control (GPIO + PWM):
    - The vehicle's direction is managed using GPIO signals sent to the H-Bridge.
    - Speed is adjusted using PWM signals applied to the motor enable pins.

- Audio Playback (SPI + DAC):
    - Audio files are stored on the MicroSD card and read using the SPI protocol.
    - Sounds are played through the digital-to-analog converter (DAC1, pin 25) connected to the audio amplifier, based on vehicle events (e.g., accelerating, reversing, honking).

- Obstacle Avoidance (ADC):
    - The IR sensor measures the distance to obstacles and sends an analog signal to the microcontroller.
    - The analog signal is converted to distance using a calibrated formula.
    - If an obstacle is too close, the vehicle automatically changes direction (stopping the motors and reversing).

- Wi-Fi Control (WebSocket):
    - The user connects to the ESP32 via a Wi-Fi Access Point.
    - Commands sent from the web interface are processed via WebSocket to activate functionalities such as direction, speed, headlights, or horn.
    - Debugging information is printed to the serial monitor.


### Validation of Functionality Integration:

The project was tested as a complete logical workflow, where each functionality was verified in its context:
- The user connects to the vehicle’s Wi-Fi network.
- Commands are sent from the web interface to control movement and other actions.
- The vehicle automatically avoids obstacles when detected.
- Sounds and lights are activated based on the vehicle’s state (e.g., accelerating, braking, reversing).
- The vehicle’s overall behavior was assessed in a real-world environment.

### Sensor Calibration

The calibration of the SHARP GP2Y0A21 IR sensor involved the following steps:

- Reading the Sensor: The sensor’s output is read by the ESP32, and this raw analog value is then converted into a distance value using a mathematical formula. The sensor’s voltage range is first mapped to a volts value using the following formula:

```
float volts = (analogValue * 3.3) / 4095.00;
```
- Converting to Distance: The voltage is converted into distance in centimeters using a pre-calculated distance formula based on the sensor's distance measuring characteristics, which can be found in the [datasheet](https://global.sharp/products/device/lineup/data/pdf/datasheet/gp2y0a21yk_e.pdf) (page 5). According to the tutorial on [makerguides.com](https://www.makerguides.com/sharp-gp2y0a21yk0f-ir-distance-sensor-arduino-tutorial/), this can be achieved using tools like MS Excel, resulting in the following formula for distances greater than 10 cm:


```
int cmDistance = 29.988 * pow(volts, -1.173);
```

- Testing: The sensor was tested by checking if it gave correct distance readings at different points to ensure it worked well for obstacle detection.


## Results

### Working Car Demo

Here is a video showing how the car works:
- Users can connect to the Wi-Fi Access Point created by the ESP32 and control the car through the web interface at 192.168.4.1 by using the designated buttons, which can move it in any direction, as well as activate the horn, toggle the obstacle avoidance feature, and switch the headlights on or off.

[![demo-wifi-rc-car](https://img.youtube.com/vi/ml5nQtXt7Yc/0.jpg)](https://www.youtube.com/watch?v=ml5nQtXt7Yc)

### Car Photos

Car Front             |  Car Back
:-------------------------:|:-------------------------:
![](https://github.com/BanuAdrian/wifi-rc-car/blob/main/images/front-of-the-car.jpg)  |  ![](https://github.com/BanuAdrian/wifi-rc-car/blob/main/images/back-of-the-car.jpg)

Car Interior            |  Car Interior |  Car Interior
:-------------------------:|:-------------------------:|:-------------------------:
![](https://github.com/BanuAdrian/wifi-rc-car/blob/main/images/interior-chassis.jpg)  |  ![](https://github.com/BanuAdrian/wifi-rc-car/blob/main/images/interior-close.jpg) |  ![](https://github.com/BanuAdrian/wifi-rc-car/blob/main/images/interior-overview.jpg)

### Web Interface

<img src="https://github.com/BanuAdrian/wifi-rc-car/blob/main/images/web-interface.jpg" width=35% height=35%>

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
- 15.12.2024 - bought M3 screws and nuts then mounted the motors on the chassis and verified the car's look by attaching its body
- 03.01.2025 - started to develop the Wi-Fi functionality of the car, including the web page 
- 04.01.2025 - drilled holes for headlights, taillights, IR sensor and speaker wires (also mounted this in a tire from one of the wheels which the car had) & connected all the components according to the electric scheme (also made sure the cables weren't sticking out of the chassis)
- 04.01.2025 - 07.01.2025 - tested the functionality of all components while developing the code and made adjustments as needed or fixed various issues that arose (e.g. dealing with broken wires or, more critically, rethinking how the ESP32 is powered because the L298N's 5V output, which was previously used, dropped to as low as 4.5V when the motors started, causing the ESP32 to restart and rendering it unusable).
- 07.01.2025 - finished the software milestone

## Bibliography/Resources


- https://lastminuteengineers.com/esp32-pinout-reference/
- https://www.makerguides.com/esp32-and-gp2y0a21yk0f-distance-sensor/
- https://randomnerdtutorials.com/esp32-websocket-server-arduino/
- https://www.thingiverse.com/thing:5611259 (motor mount 3D model)
