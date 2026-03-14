# ESP32 Robot Radar System

![License](https://img.shields.io/badge/license-MIT-blue)
![Platform](https://img.shields.io/badge/platform-ESP32-green)
![Framework](https://img.shields.io/badge/framework-Arduino-red)
![Language](https://img.shields.io/badge/language-C++-orange)
![Sensor](https://img.shields.io/badge/sensor-HC--SR04-yellow)
![Actuator](https://img.shields.io/badge/actuator-Micro%20Servo-lightgrey)
![Networking](https://img.shields.io/badge/network-ESP32%20WiFi-blue)
![Data](https://img.shields.io/badge/data-JSON-lightgrey)
![Frontend](https://img.shields.io/badge/frontend-HTML5-orange)
![Style](https://img.shields.io/badge/style-CSS3-blue)
![Script](https://img.shields.io/badge/script-JavaScript-yellow)
![Graphics](https://img.shields.io/badge/graphics-Canvas%20API-purple)

An ESP32-powered robotic radar system that performs real-time environmental scanning, object detection, and live radar visualization through a web dashboard.


---

## Overview

This project implements a radar-style robotic scanning system using an ultrasonic sensor mounted on a servo motor.

The system continuously scans the environment from **0° to 180°**, detects nearby objects, and provides multi-layer feedback through LEDs, buzzer alerts, and a live radar dashboard.

When an object enters the detection range, the robot validates the signal and locks onto the target, similar to real radar tracking systems.

---

## Demo

![Radar Demo-Auto Mode](Robot_Radar_in_Operation.gif)
![Radar Demo - Manual Mode](Robot_Radar_in_Operation%20(1).gif)



https://www.youtube.com/watch?v=6M3X1xsha7I

https://www.youtube.com/watch?v=Nw1-PEBm7XM&feature=youtu.be

## Digital Twin (3D Model)

https://scaniverse.com/scan/qmovmeqbyeedrgni

---

## Hardware Components

- ESP32 microcontroller
- HC-SR04 ultrasonic sensor
- Micro servo motor
- LED proximity ladder
- Piezo buzzer
- Analog joystick
- Push button

---

## Hardware Setup

![Hardware Setup](Robot_Radar-img.png)
![Hardware Setup](Robot_Radar-img-2.png)

---

## Tech Stack

### Embedded

- C++
- Arduino Framework
- PWM Servo Control
- State Machine Control
- Non-blocking timing (`millis()`)

### Networking

- ESP32 WiFi SoftAP
- Embedded HTTP Server
- JSON telemetry API

### Web Interface

- HTML
- CSS
- JavaScript
- Canvas API

---

## System Architecture

Input Layer
- Ultrasonic Sensor → Distance measurement
- Joystick → Manual control
- Button → Mode switching

Processing Layer
- ESP32
- Signal validation
- Control logic
- Servo control
- LED mapping
- Buzzer control

Output Layer
- Servo scanning
- LED proximity display
- Buzzer alerts
- Radar web dashboard

---

## Features

- 0°–180° radar sweep
- Autonomous scanning mode
- Manual joystick control
- Object locking mechanism
- Distance-based LED ladder
- Parking-sensor style buzzer alerts
- Real-time radar dashboard
- Live telemetry streaming

---

## Control Logic

The robot operates using a layered decision architecture that processes sensor signals, validates detections, and controls actuation in real time.

The system follows a state-based control flow:

SCAN → DETECT → LOCK → RELEASE

SCAN  
The servo continuously sweeps the ultrasonic sensor from 0°–180°.

DETECT  
Distance measurements are validated to filter noise and false echoes.

LOCK  
If an object remains within the detection threshold for a specified time window, the system locks onto the target and freezes the servo.

RELEASE  
If the object moves outside the release threshold, scanning resumes.

---

## Radar Dashboard

![Radar Dashboard](Robot_Radar_dashboard-3.png)
![Radar Dashboard](Robot_Radar_dashboard-2.png)
![Radar Dashboard](Robot_Radar_dashboard-1.png)


The ESP32 hosts a web interface that visualizes the radar sweep in real time.

Features include:

- animated radar sweep
- echo persistence visualization
- lock indication
- distance telemetry
- synchronized LED ladder

---

## Circuit Diagram

![Circuit Diagram](Robot_Radar-img-3.png)

---


## Future Improvements

- LiDAR-based sensing
- ROS integration
- SLAM-based mapping
- autonomous navigation
- machine learning object detection

---

## Documentation

A detailed explanation of the system architecture, control logic, and implementation methodology is available in the full project report: ![Project Report](docs/Robot-radar-esp32.pdf)

---

## Acknowledgements

Developed as part of the Robotics Interest Group induction project.

## Author

Adithya A  
Electronics & Communication Engineering  
NIT Calicut
India

## License

MIT License
