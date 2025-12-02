# Virtual Water Tube – Realistic Tilt-Controlled Water Simulation   
**Author:** Vladislav Skachkov  
**Date:** November 2025  

### The “Virtual Waves” project  
A stunningly realistic glowing water simulation running on a **Raspberry Pi Pico** using an **MPU6050** accelerometer and a **WS2812B (NeoPixel) LED strip**.
When you tilt the board(sensor) left or right, the glowing “water” flows, bounces off the ends with perfect damping, and slowly settles.

[Link to the demo version of a project](https://www.youtube.com/watch?v=vVL3egFfj9s) 

### Features
- Dual-core design (100 Hz physics on Core 0, ~22 FPS visuals on Core 1)  
- Perfectly tuned physics: sensitivity, viscosity, wall bounce  
- Live ASCII art visualization in Serial Monitor  
- Beautiful automatic demo mode (sine wave) when no sensor is connected  
- Random shimmer effect for extra realism  

### Hardware 
- Raspberry Pi Pico (or Pico W)  
- Adafruit MPU6050 (or any MPU6050 breakout)  
- WS2812B LED strip (any length, 30 LEDs used in the current system)  
- External 5V power supply

### Wiring 

/Raspberry Pi Pico
	-3.3V(OUT) (pin 36)
	-GND (pin 38)

/MPU6050 
	-VCC → 3.3 V
	-GND → GND
	-SDA → GP4
	-SCL → GP5

/WS2812B strip
	-VCC  → 3.3V (1-2 A)
	-GND → Pico GND
	-DIN → 330Ω resistor → Pico GP11

