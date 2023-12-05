/*
Kirsti Härö
20.10.2023

In this program I am using Adafruit Metro M4 with VL53L1X sensor to measure distance.
Distance is calculated as an average from 10 consequtive measurements.

*/


#include <Arduino.h>
#include "Adafruit_VL53L1X.h"

#define IRQ_PIN 12
#define XSHUT_PIN 11

Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);
int my_array[10];
int average;

void setup() {
  my_array[9] = -1; //for making sure there are 10 measurements in array
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println(F("Adafruit VL53L1X sensor demo"));

  Wire.begin();
  if (! vl53.begin(0x52, &Wire)) {
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    while (1)       delay(10);
  }
  Serial.println(F("VL53L1X sensor OK!"));

  Serial.print(F("Sensor ID: 0x"));
  Serial.println(vl53.sensorID(), HEX);

  if (! vl53.startRanging()) {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(vl53.vl_status);
    while (1)       delay(10);
  }
  Serial.println(F("Ranging started"));

  // Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms!
  vl53.setTimingBudget(50);
  Serial.print(F("Timing budget (ms): "));
  Serial.println(vl53.getTimingBudget());

}

void loop() {
  int16_t distance;
  int i;

  if (vl53.dataReady()) {
    // measurements to our array
    for (i = 0; i < 10; i++) {
      distance = vl53.distance();
      my_array[i] = distance;
      delay(100);

      if (distance == -1) {
        // something went wrong!
        Serial.print(F("Couldn't get distance: "));
        Serial.println(vl53.vl_status);
        return;
      }

      //making sure we already have 10 measurements in array
      if (my_array[9] >= 0) {
        int sum = 0;
        //calculating sum and average of the measurements
        for (int j = 0; j < 10; j++) {
          sum += my_array[j];
        }
        average = sum / 10;
        Serial.println(average);
        delay(10);
      }

    }

    // after i has reached end of an array, we will re-start writing at position 0
    if (i == 9) {
      i = 0;
    }

    // data is read out, time for another reading!
    vl53.clearInterrupt();
  }
}
