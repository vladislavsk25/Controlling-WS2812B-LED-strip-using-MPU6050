/**
-----------------------------------------------
Realistic water physics with MPU6050 + WS2812B
-----------------------------------------------
*
* @file       waves.ino
* @author     Vladislav Skachkov
* @date       2025-11-22
* @version    1.0
* @brief      Tilt-controlled luminous water simulation
* 
* Features:
*   • Dual-core physics (100 Hz) + visuals (~22 FPS)
*   • Perfectly tuned water bounce
*   • Live ASCII visualization in Serial Monitor
*   • Demo mode with beautiful sine wave when no sensor
*
*/

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>

// ------------------- Configuration -------------------

#define LED_PIN         11
#define NUM_LEDS        30        
#define BRIGHTNESS      120

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800); // GRB - GREEN/RED/BLUE | 800 kHz signal (standard speed for almost all WS2812B strips)
Adafruit_MPU6050 mpu;

// ------------------- Shared volatile variables -------------------

volatile float waterPos = NUM_LEDS / 2.0f;
volatile float waterVel = 0.0f;
volatile bool mpuFound = false;

// --------------------Physics Tuning-------------------------------

/** The factor of sensitivity (generally speaking).
* IF we DECREASE ---> Much more tilt is needed to get the same movement
* IF we INCREASE ---> Water reacts faster and stronger to small tilts(becomes more sensitive) */
const float TILT_SCALE = 0.018f;     
/* Natural "resistance" factor/ depending on the density of the liquid can be changed
* For example, something like syrop would be around 0.1 and vise versa**/
const float GRAVITY    = 0.06f;  
/** How much speed is lost every time the water hits an obstacle 
* For clarity, something like a bouncy ball is around 0.9 */
const float DAMPING    = 0.68f;      

// ------------------- Core 0: setup() & loop() – Sensor + Physics -------------------

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); // Pauses Core 0 for 10 ms after each physics update

  Wire.setSDA(4);
  Wire.setSCL(5);

  if (mpu.begin()) {
    mpuFound = true;
    mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    Serial.println("MPU6050 found and initialized!");
  } else {
    mpuFound = false;
    Serial.println("No MPU6050 - running in demo mode with simulated tilt");
  }
}

void loop() {
  float tilt = 0.0f;

  if (mpuFound) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Roll = rotation around X axis → left/right tilt
    // Compute true roll angle using gravity vector
    float accelX = a.acceleration.x;
    float accelZ = a.acceleration.z;
    tilt = atan2(accelX, accelZ) * 180.0 / PI;  // Roll in degrees
  } else {
    // Beautiful demo wave when no sensor
    tilt = 65.0f * sin(millis() * 0.0012f);
  }

  // ----- Physics engine -----

  float accel = -tilt * TILT_SCALE;
  waterVel += accel;
  waterVel *= (1.0f - GRAVITY);  // Air/water friction

  waterPos += waterVel;

  // Bounce with realistic damping

  if (waterPos <= 0) {
    waterPos = 0;
    waterVel = -waterVel * DAMPING;
  }
  if (waterPos >= NUM_LEDS - 1) {
    waterPos = NUM_LEDS - 1;
    waterVel = -waterVel * DAMPING;
  }
  delay(10);  // ~100 Hz physics update
}

// ------------------- Core 1: setup1() & loop1() – LEDs + ASCII Visualization -------------------

void setup1() {
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.clear();
  strip.show();
}

void drawWaterASCII() {
  String line = "";
  float pos = waterPos;

  for (int i = 0; i < NUM_LEDS; i++) {
    float dist = abs(i - pos);
    if (dist < 0.8f)      line += "██";
    else if (dist < 2.0f) line += "▓▓";
    else if (dist < 4.0f) line += "▒▒";
    else if (dist < 7.0f) line += "≈≈";
    else if (dist < 11.0f)line += "~~";
    else if (dist < 16.0f)line += "..";
    else                  line += "__";
  }

  Serial.printf("\n%s  Pos:%.2f  Vel:%.3f", line.c_str(), pos, waterVel);
}

void updateLEDs() {
  strip.clear();
  float pos = waterPos;

  for (int i = 0; i < NUM_LEDS; i++) {
    float dist = abs(i - pos);
    uint8_t intensity = 0; // how bright the current LED (pixel number i) should be

    if (dist < 1.0f)      intensity = 255;
    else if (dist < 3.0f) intensity = 200 - (dist-1)*60;
    else if (dist < 7.0f) intensity = 100 - dist*12;
    else if (dist < 12.0f)intensity = max(30 - (int)(dist*3), 0);
    else                  intensity = 0;

    // Shimmer Effect

    if (random(100) < 15 && dist < 6) intensity = min(255, intensity + 90);

    if (intensity > 10) {
      // Blue water glow: R=0, G=80+intensity/3, B=255 (shifted for more blue)
      strip.setPixelColor(i, strip.Color(0, 80 + intensity/3, 255));
    }
  }
  strip.show();
}

void loop1() {
  updateLEDs();
  drawWaterASCII();
  delay(45);  // ~22 FPS visual update (1000ms/45 = 22.22) 
}
