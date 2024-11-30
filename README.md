# Wi-Fi Remote Controlled Car

## Introduction

This project represents a four-wheeled car controlled via Wi-Fi, implementing various functions such as a horn, acceleration sound, obstacle avoidance, headlights, taillights, and more.

The idea is rooted in my childhood passion for toy cars, with my favorites being those that could be controlled remotely. I often imagined the moment when I would get my driver’s license and be able to drive in real life. However, I would feel sad whenever the remote control broke, but it gave me the opportunity to take the cars apart using a screwdriver and admire the components inside, without necessarily understanding them.

I believe this project is valuable because it offers a great way to turn a childhood passion into a tangible creation that blends nostalgia with innovation. For others, it could serve as an inspiration to bring their ideas to life. Additionally, since this project is essentially a toy, it has the potential to bring excitement and joy for those who interact with it, no matter their age.

## Overview

## Hardware Design

#### Components list (could be modified): 
- 1x ESP32 DevKit V1
- 1x Dual H-Bridge Motor Driver L298N
- 4x TT Gear Motors with Wheels (1:48 Gear Ratio)
- 2x Li-Ion 18650 Batteries, 3.7V, 2200mAh (powering the L298N driver and the ESP32 board)
- 1x Li-Ion 18650 Battery Case with On/Off Switch
- 1x HC-SR04 Ultrasonic Sensor/GP2Y0A21YK0F Infrared Distance Sensor (obstacle avoidance)
- 1x MicroSD Module (storing sounds files)
- 1x Passive Buzzer (3.3V) (playing horn & acceleration sounds)
- 4x LEDs (2x White - headlights, 2x Red - taillights)
- 4x Resistors (330 Ohm)
- Jumper Wires

## Software Design

I will be using PlatformIO IDE for VSCode.

## Results

## Conclusions

## Logs

## Bibliography/Resources
