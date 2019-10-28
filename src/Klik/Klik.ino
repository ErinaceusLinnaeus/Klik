/* (C)2019 Benji Schmid
 *  
 *  Klik is a Metronom that will be part of my recording rack.
 *  Presumably it will be take up 1 HE and half the rack's length.
 *  
 *  Parts:
 *    Arduino Due
 *    2.8" TFT Touchscreen
 */
 
#include <stdint.h>
#include "TouchScreen.h"

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

struct songlist
{
   char* songname;
   int tempo;
};
typedef struct songlist songlist;

struct band
{
   char* bandname;
   songlist song[32]; 
};
typedef struct band band;

void setup() {
  Serial.begin(9600);

  band currBand;
  //PSEUDOCODE, later replaced by stuff loaded from the SD card
  currBand.bandname = "lister";
  currBand.song[0].songname = "Sofdu";
  currBand.song[0].tempo = 78;
  currBand.song[1].songname = "Solo";
  currBand.song[1].tempo = 112;

  //etc.
  

}

void loop() {
  // a point object holds x y and z coordinates
  TSPoint p = ts.getPoint();
  
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > ts.pressureThreshhold) {
     Serial.print("X = "); Serial.print(p.x);
     Serial.print("\tY = "); Serial.print(p.y);
     Serial.print("\tPressure = "); Serial.println(p.z);
  }

  delay(100);

}
