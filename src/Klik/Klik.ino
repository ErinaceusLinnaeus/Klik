/* (C)2020 Benji Schmid

    Klik is a Metronom that will be part of my recording rack.
    Presumably it will take up 1 HE and half the rack's length.

    Parts:
      Arduino Uno/Due
      2.8" TFT Touchscreen
*/

#include <stdint.h>
#include <TouchScreen.h>

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin

//If the touchscreen's zero-point doesn't start at 0
#define  XCORRECTION -120
#define  YCORRECTION -150

//How wide(pixel) the buttons are
#define  XDIVIDE 170
#define  YDIVIDE 240

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
  //Later replaced by stuff loaded from the SD card, maybe
  currBand.bandname = "lister";
  currBand.song[0].songname = "Sofdu";
  currBand.song[0].tempo = 78;
  currBand.song[1].songname = "Solo";
  currBand.song[1].tempo = 112;

  //etc.


}

void loop() {
  
  // a point object holds x y and z coordinates
  TSPoint point = ts.getPoint();
  TSPoint field;

//Divides the display into an array:
// (4/2) (3/2) (2/2) (1/2) (0/2)
// (4/1) (3/1) (2/1) (1/1) (0/2)
// (4/0) (3/0) (2/0) (1/0) (0/0)
  field.x = ((point.x + XCORRECTION) / XDIVIDE);
  field.y = ((point.y + YCORRECTION) / YDIVIDE);
  field.z = point.z;

  MonitorOutField(field);
//  MonitorOutCoordinate(point);
  delay(333);

}

//Information for error correction
void MonitorOutField(TSPoint point) {
  
  if (point.z > ts.pressureThreshhold) {
    Serial.print("Field (x/y): (");
    Serial.print(point.x);
    Serial.print("/");
    Serial.print(point.y);
    Serial.println(")");
  }
}

//Information for error correction
void MonitorOutCoordinate(TSPoint point) {
  
  if (point.z > ts.pressureThreshhold) {
    Serial.print("Coordinate (x/y): (");
    Serial.print(point.x + XCORRECTION);
    Serial.print("/");
    Serial.print(point.y + YCORRECTION);
    Serial.println(")");
  }
}
