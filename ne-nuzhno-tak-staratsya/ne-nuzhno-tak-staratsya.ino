#include <FastLED.h>

#define DATA_PIN 8
#define TRIG_PIN 10
#define ECHO_PIN 11
#define NUM_LEDS 54

CRGB leds[NUM_LEDS];

bool hasBlackedOut = false;
int color_state = 0;
int pattern_state = 0;
int offset = 0;

float duration, dist;

double recordedDists[200];
int recordedDistLength = 0;
float deltaDist;

int hasHandCount = 0;
int noHandCount = 0;
double h1 = 0;
double h2 = 0;

int hue = 0;
int saturation = 0;
int brightness = 0;

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(9600);
}

float toCm(float duration) {
  return (duration/2)/29.1; // Divide by 29.1 or multiply by 0.0343
}

float toInches(float duration) {
  return (duration/2)/74; // Divide by 74 or multiply by 0.0135
}

void loop() {
  // the sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
 
  // read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(ECHO_PIN, INPUT);
  duration = pulseIn(ECHO_PIN, HIGH);
  dist = toCm(duration);     
  
  if(hasHandCount < 15) { // start detecting hands
    if(dist < 15) {
      hasHandCount += 1;
    } else {
      hasHandCount = 0;
    }
  } else if(hasHandCount == 15) { // has detected hands for a while
    if(dist < 15 && recordedDistLength <= 200) {
      recordedDists[recordedDistLength] = dist;
      if(recordedDistLength > 0) {
        deltaDist = abs(recordedDists[recordedDistLength] - recordedDists[recordedDistLength-1]);
        if(deltaDist > 0.25) {
          recordedDists[recordedDistLength] = dist;
          recordedDistLength += 1;
        }
      } else {
        recordedDists[recordedDistLength] = dist;
        recordedDistLength += 1;
      }
    } else if(recordedDistLength > 0) { // dectect hands leaving
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
          // Serial.println("left!");
          if(pattern_state == 0) {
            color_state = 0;
          }
          pattern_state = (pattern_state+1)%4;
          hasBlackedOut = false;
        } else {
          // Serial.println("right!");
          color_state = (color_state+1)%7;
        }
        hasHandCount = 0;
        noHandCount = 0;
        recordedDistLength = 0;
        h1 = 0;
        h2 = 0;
      }
    } 
  }

  switch(color_state) {
    case 0: // red
      hue = 95;
      saturation = 255;
      brightness = 200;
      break;
    case 1: // yellow
      hue = 75;
      saturation = 255;
      brightness = 200;
      break;
    case 2: // green
      hue = 0;
      saturation = 255;
      brightness = 200;
      break;
    case 3: // cyan
      hue = 230;
      saturation = 255;
      brightness = 200;
      break;
    case 4: // blue
      hue = 175;
      saturation = 255;
      brightness = 200;
      break;
    case 5: // purple
      hue = 150;
      saturation = 255;
      brightness = 200;
      break;
    case 6: // rainbow
      hue = (hue+4)%255;;
      saturation = 255;
      brightness = 200;
      break;
  }

  switch(pattern_state) {
    case 0: // off
      if(hasBlackedOut == false) {
        hasBlackedOut = true;
        for(int i=0; i<NUM_LEDS; i++) {
          leds[i] = CRGB::Black;
        }
        FastLED.show();
      }
      break;
    case 1: // clockwise
      for(int i=0; i<NUM_LEDS; i++) {
        if(255-(i*5) > 0) {
          brightness = 255-(i*5);
        } else {
          brightness = 0;
        }
        leds[(offset-i+NUM_LEDS)%NUM_LEDS] = CHSV(hue, saturation, brightness);        
      }
      FastLED.show();
      break;
    case 2: // counter-clockwise
      for(int i=0; i<NUM_LEDS; i++) {
        if(255-(i*5) > 0) {
          brightness = 255-(i*5);
        } else {
          brightness = 0;
        }
        leds[(offset+i)%NUM_LEDS] = CHSV(hue, saturation, brightness);
      }
      FastLED.show();
      break;
    case 3: // blink
      for(int i=0; i<5; i++) {
        leds[int(random(0, NUM_LEDS))] = CHSV(hue, saturation, brightness);
      }
      FastLED.show();
      for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
      }
      break;   
  }

  if(pattern_state == 1) {
    offset = (offset+1)%NUM_LEDS;
  } else if(pattern_state == 2) {
    offset = (offset-1+NUM_LEDS)%NUM_LEDS;
  }
  delay(10);
}
