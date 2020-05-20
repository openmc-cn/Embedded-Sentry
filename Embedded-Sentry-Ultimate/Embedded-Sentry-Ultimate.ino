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

const int motionDuration = 1000;           // record and unlock for 1s
const int interval = motionDuration/10;    // retrieve data every 0.1s (so get 9 sets of data; each set has 6 data)

MPU6050 accelgyro;

int ax, ay, az, gx, gy, gz;
int r_data[9][6];   // record data
int u_data[9][6];   // upload data

void setup() {

  Wire.begin();
  Serial.begin(9600);

  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  // instruction
  Serial.println("Send 'R' to record gesture.");
  Serial.println("Send 'U' to unlock the LED.");

  // configure Arduino LED for output
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  
  if (Serial.available() > 0) {
    
    switch (Serial.read()) {
      case 'R':{  // recoding
        Serial.println("Recording...");
        retrieveData(r_data);
        break; 
      }

      case 'U': {  // unlocking
        Serial.println("Unlocking...");
        retrieveData(u_data);
        check();
        break;
      }
    }
    
  }
  
}

void printOut() {
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.print("a/g:\t");
  Serial.print(ax); Serial.print("\t");
  Serial.print(ay); Serial.print("\t");
  Serial.print(az); Serial.print("\t");
  Serial.print(gx); Serial.print("\t");
  Serial.print(gy); Serial.print("\t");
  Serial.println(gz);
}

void printData(int a[][6]) {

  Serial.println("Printing...");
  for (int i=0; i<9; i++) {
    for (int j=0; j<6; j++) {
      Serial.print(a[i][j]); Serial.print("\t");
      if (j==5) {
        Serial.print("\n");
      }
    }
  }
  Serial.println("Done.");
  
}

void retrieveData(int a[][6]) {

  unsigned long startTime = millis();
  int duration = 0;
  unsigned long retrieveTime = startTime + interval;
  int i = 0;

  // record or upload for motionDuration
  while (duration < motionDuration) {
    
//    printOut();

    // retrieve data
    if (millis() > retrieveTime ) {
      accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      addToArray(a[i], ax, ay, az, gx, gy, gz);
      retrieveTime += interval;
      i++;
    }
    
    duration = millis() - startTime;
  }

  Serial.println("Done.");

//  printData(a);
  
}

void addToArray(int a[], int ax, int ay, int az, int gx, int gy, int gz) {

    a[0] = ax;
    a[1] = ay;
    a[2] = az;
    a[3] = gx;
    a[4] = gy;
    a[5] = gz;
  
}

void check() {

//  Serial.println(getDistance(r_data, u_data));  
  if (getDistance(r_data, u_data) < 1000) {
    success();
  }
  else {
    fail();
  } 
  
}

long getDistance(int a[][6], int b[][6]) {

  long d[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  long distance = 0;
  for (int i=0; i<9; i++) {  
    for (int j=0; j<6; j++) {
      d[i] += abs(a[i][j]-b[i][j]); 
//      Serial.print("|");Serial.print(a[i][j]);Serial.print("-");Serial.print(b[i][j]);Serial.print("|");Serial.print("=");Serial.println(abs(a[i][j]-b[i][j]));
//      Serial.println(d[i]);   
    }  
    d[i] = round(sqrt(d[i]));
//    Serial.println(d[i]);
  }
  
  for (int k=0; k<9; k++) {
    distance += d[k];
  }
  
  return distance;
  
}

void success() {

  // if unlocked
  Serial.println("Unlock Successfully!");
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on for 5s
  delay(5000);
  digitalWrite(LED_BUILTIN, LOW);
  
}

void fail() {

  // if failed to unlock
  Serial.println("Fail to Unlock! Please Try Again!");
  
}
