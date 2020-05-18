/* ============================================
  I2Cdev device library code is placed under the MIT license
  Copyright (c) 2011 Jeff Rowberg

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
  ===============================================
*/

#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

const long interval = 1000;    // record for 3s

MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

//bool blinkState = false;

void setup() {

  Wire.begin();
  Serial.begin(9600);

  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  // 
  Serial.println("Press 'R' to record gesture for 3 seconds.");
  Serial.println("Press 'U' to unlock the LED.");

  // configure Arduino LED for output
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  
  if (Serial.available() > 0) {
    
    switch (Serial.read()) {
        case 'R':{
          // start recoding for 3s
          unsigned long startTime = millis();
          unsigned long d = 0;
          Serial.println(startTime);
          
          while (d < interval) {
            accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
            Serial.print("a/g:\t");
            Serial.print(ax); Serial.print("\t");
            Serial.print(ay); Serial.print("\t");
            Serial.print(az); Serial.print("\t");
            Serial.print(gx); Serial.print("\t");
            Serial.print(gy); Serial.print("\t");
            Serial.println(gz);
            
            d = millis() - startTime;
          }
          break;
        }

        case 'U': {
          Serial.println("Hello!");
          break;
        }
    }
    
  }

  // blink LED to indicate activity
//  blinkState = !blinkState;
//  digitalWrite(LED_BUILTIN, blinkState);
}
