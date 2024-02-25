#include <Wire.h>
#include "MAX30105.h"

MAX30105 particleSensor;

double avered = 0;
double aveir = 0;
double sumirrms = 0;
double sumredrms = 0;
int i = 0;
int Num = 100;

double ESpO2 = 95.0;
double FSpO2 = 0.7;
double frate = 0.95;

#define TIMETOBOOT 3000
#define SAMPLING 5
#define FINGER_ON 30000
#define MINIMUM_SPO2 80.0

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");

  while (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 was not found. Please check wiring/power at MAX30102 board.");
  }

  byte ledBrightness = 0x7F;
  byte sampleAverage = 4;
  byte ledMode = 2;
  int sampleRate = 200;
  int pulseWidth = 411;
  int adcRange = 16384;

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
  particleSensor.enableDIETEMPRDY();
}

void loop() {
  uint32_t ir, red;
  double SpO2 = 0;

  particleSensor.check();
  while (particleSensor.available()) {
    red = particleSensor.getFIFORed();
    ir = particleSensor.getFIFOIR();

    i++;
    double fred = (double)red;
    double fir = (double)ir;
    avered = avered * frate + (double)red * (1.0 - frate);
    aveir = aveir * frate + (double)ir * (1.0 - frate);
    sumredrms += (fred - avered) * (fred - avered);
    sumirrms += (fir - aveir) * (fir - aveir);

    if ((i % SAMPLING) == 0) {
      if (millis() > TIMETOBOOT) {
        if (ir < FINGER_ON) {
          ESpO2 = MINIMUM_SPO2; // Finger detached
        } else {
          double R = (sqrt(sumredrms) / avered) / (sqrt(sumirrms) / aveir);
          SpO2 = -23.3 * (R - 0.4) + 100;
          ESpO2 = FSpO2 * ESpO2 + (1.0 - FSpO2) * SpO2;
        }
        Serial.print("Oxygen % = ");
        Serial.println(ESpO2);
        delay(100);
        Serial.print("Oxygen % = ");
        Serial.println(ESpO2);
        delay(100);
      }
    }

    if ((i % Num) == 0) {
      sumredrms = 0.0;
      sumirrms = 0.0;
      i = 0;
      Serial.println("Initializing...");
      delay(100);
      Serial.println("Initializing...");
      delay(100);
      break;
    }

    particleSensor.nextSample();
  }
}