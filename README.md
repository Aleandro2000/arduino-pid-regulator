# Arduino PID Controller

A simple **PID (Proportional-Integral-Derivative)** controller on Arduino. It reads a sensor and adjusts an actuator to maintain a desired setpoint.

## Components
- Arduino (Uno, Mega, Nano)
- Sensor (e.g., temperature, position)
- Actuator (e.g., motor, heater)
- Wires, breadboard

## Flutter Application
Alongside the Arduino sketch, a **Flutter mobile app** was developed:
- Connects to the Arduino via **Bluetooth (HC-05/HC-06)**
- Displays live sensor readings: **Temperature, Humidity, Motor Speed**
- Plots data in real-time using charts
- Provides a button to toggle the **LED on/off** remotely