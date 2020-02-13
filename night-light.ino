#include <FastLED.h>
#define NUM_LEDS 90
#define DATA_PIN 8

CRGB leds[91];

const int trigPin = 10;
const int echoPin = 11;

float duration, cm, inches;

int noHandTimes;
int number;

double recordedDists[200];
float deltaDist;

int recordedDistLength = 0;
int hasHandCount = 0;
int noHandCount = 0;
float h1 = 0;
float h2 = 0;

int dot = 0;

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Starts the serial communication

  noHandTimes = 0;
  number = 0;
}

void loop() {

  leds[dot+1] = CRGB::Blue;
  
//  Serial.println(dot);
  FastLED.show();
  // clear this led for the next time around the loop
  leds[dot] = CRGB::Black;
  
  dot += 1;
  if(dot >= NUM_LEDS) {
    dot = 0;
  }
    
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  //inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
  
  if(hasHandCount < 15) {
    if(cm < 15) {
      hasHandCount += 1;
    } else {
      hasHandCount = 0;
    }
  } else if(hasHandCount == 15) {
    if(cm < 15 && recordedDistLength <= 200) {
      recordedDists[recordedDistLength] = cm;
      if(recordedDistLength > 0) {
        deltaDist = abs(recordedDists[recordedDistLength] - recordedDists[recordedDistLength-1]);
        if(deltaDist > 0.25) {
//          Serial.print("[");
//          Serial.print(recordedDistLength);
//          Serial.print("]");
//          Serial.println(cm);
          recordedDists[recordedDistLength] = cm;
          recordedDistLength += 1;
        }
      } else {
//        Serial.print("[");
//        Serial.print(recordedDistLength);
//        Serial.print("]");
//        Serial.println(cm);
        recordedDists[recordedDistLength] = cm;
        recordedDistLength += 1;
      }
    } else if(recordedDistLength > 0) {
      noHandCount += 1;
      if (noHandCount>= 15) {
        for(int i = 0; i < recordedDistLength; i++) {
          if(i < int(recordedDistLength/2)) {
            h1 += recordedDists[i];
          } else {
            h2 += recordedDists[i];
          }
        }
        h1 = h1/int(recordedDistLength/2);
        h2 = h2/(recordedDistLength - int(recordedDistLength/2));
        if(h1 > h2) {
          Serial.println("left!");
        } else {
          Serial.println("right!");
        }
        hasHandCount = 0;
        noHandCount = 0;
        recordedDistLength = 0;
        h1 = 0;
        h2 = 0;
      }
    } 
  }
  delay(10);
}
