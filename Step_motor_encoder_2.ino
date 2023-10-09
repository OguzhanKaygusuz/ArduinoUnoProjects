#include <stdlib.h>
#include <stdio.h>

// Define pins
#define CS_H  digitalWrite(SS, HIGH)
#define CS_L  digitalWrite(SS, LOW)
#define CLK_H  digitalWrite(SCK, HIGH)
#define CLK_L  digitalWrite(SCK, LOW)
#define driverPUL 7    // PUL- pin
#define driverDIR 6    // DIR- pin

#define driverENA 8    // ENA- pin 
// PUL+ DIR+ ENA+ connects to 5V of Arduino

int pulse = 400;            // Pulse Delay period
bool dir = HIGH;            // Set Direction
double angle = 0;
double data_diff; 
double newdata = 0;
String direction = "";
double angle_calc = 0.351906; // (360/1023) this number turns the 10-bit value into angle(0-360)


int SPI_RW()
{
  long int i = 0;
  long int wdata = 0;
  CS_H;
  delayMicroseconds(15); // Delay using microseconds
  CS_L;
  delayMicroseconds(15); // Delay using microseconds
  for (i = 0; i < 9; i++) 
  {
    CLK_H;
    delayMicroseconds(15);
    if (digitalRead(MISO) > 0)
      wdata |= 1;
    wdata <<= 1;
    CLK_L;
    delayMicroseconds(15);
  }
  return wdata;  // Returns the absolute position value of the encoder
}

void setup() {
  pinMode(SS, OUTPUT);       // Set the Slave Select pin as an output
  pinMode(SCK, OUTPUT);      // Set the Serial Clock pin as an output
  pinMode(MISO, INPUT);      // Set the Master In Slave Out pin as an input
  Serial.begin(9600);      // Initialize the serial communication with a baud rate of 9600
  pinMode(driverPUL, OUTPUT);
  pinMode(driverDIR, OUTPUT);
  pinMode(driverENA, OUTPUT);
  digitalWrite(driverDIR, dir);
  digitalWrite(driverENA, LOW);
  
}

void loop() {
  int wdata = SPI_RW();      // Read the data value from the rotary encoder
  
  double angle = wdata * angle_calc;
  int newpulse = pulse;
  bool newdir = dir;

  if (abs(newdata-wdata)>2) {
    data_diff = wdata - newdata ;
    Serial.print("angle: ");
    Serial.print(angle);
    Serial.print(" data difference: ");
    Serial.println(data_diff);
    
   while (Serial.available() > 0) {
    int c = Serial.read();        //Read command
    switch (c) {
      case 'e':                     //ENABLE DRIVER
        digitalWrite(driverENA, HIGH);
        break;
      case 'd':                     //DISABLE DRIVER
        digitalWrite(driverENA, LOW);
        break;
      case 'r':                     // Reset the position
        angle = 0;
        data_diff = 0;
        wdata = 0;
        newdata = 0;
        break;
      }
  }
  if (data_diff > 0) {
    newdir = HIGH;     // CW
  } else {
    newdir = LOW;      // CCW
  }
    
    digitalWrite(driverDIR, newdir);
    newdata = wdata;
    
    int step = abs(data_diff) * (200.0*(14) / 1023.0);  // calculates the exact position of the stepper motor
    for (int i = 0; i < step * 2 ; i++) {
      digitalWrite(driverPUL, HIGH);
      delayMicroseconds(pulse);
      digitalWrite(driverPUL, LOW);
      delayMicroseconds(pulse);
    }
  }
  }
