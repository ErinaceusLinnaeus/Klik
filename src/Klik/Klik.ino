/* (C)2020 Benji Schmid

    Klik is a Metronom that will be part of my recording rack.
    Presumably it will take up 1 HE and half the rack's length.

    Parts:
      Arduino Uno
      2.8" TFT Touchscreen
*/

#include <TouchScreen.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin

//If the touchscreen's zero-point doesn't start at 0
#define TOUCHXCORRECTION -120
#define TOUCHYCORRECTION -150

//How wide(pixel) the buttons are
#define TOUCHXDIVIDE 170
#define TOUCHYDIVIDE 240

//How wide(pixel) the buttons are
#define TFTXDIVIDE 3
#define TFTYDIVIDE 5

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 570);

//A 3D array to store what command is triggered by what field
//matrix[mode][x][y]
// !!For Now we only use the normal mode!!
typedef enum {normal, edit, load,
          NumOfModes} mode;
typedef enum {previous, next, playpause,
            normalMode, editMode, loadMode,
            NIX} command;
int matrix[mode(NumOfModes)][5][3];

//Keep the current mode in mind
mode currentMode = normal;

  
struct songlist {
  char* songname;
  int tempo;
};
typedef struct songlist songlist;

struct band {
  char* bandname;
  songlist song[32];
};
typedef struct band band;


void setup() {
  Serial.begin(9600);
  
  tft.reset();
  tft.begin(0x9341);
  
  drawGrid();
  
//filling the array
  matrix[0][0][0] = next;
  matrix[0][0][1] = next;
  matrix[0][0][2] = next;
  matrix[0][1][0] = playpause;
  matrix[0][1][1] = playpause;
  matrix[0][1][2] = playpause;
  matrix[0][2][0] = playpause;
  matrix[0][2][1] = playpause;
  matrix[0][2][2] = playpause;
  matrix[0][3][0] = previous;
  matrix[0][3][1] = previous;
  matrix[0][3][2] = previous;
  matrix[0][4][0] = loadMode;
  matrix[0][4][1] = editMode;
  matrix[0][4][2] = NIX;
  
  matrix[1][0][0] = NIX;
  matrix[1][0][1] = NIX;
  matrix[1][0][2] = NIX;
  matrix[1][1][0] = NIX;
  matrix[1][1][1] = NIX;
  matrix[1][1][2] = NIX;
  matrix[1][2][0] = NIX;
  matrix[1][2][1] = NIX;
  matrix[1][2][2] = NIX;
  matrix[1][3][0] = NIX;
  matrix[1][3][1] = NIX;
  matrix[1][3][2] = NIX;
  matrix[1][4][0] = loadMode;
  matrix[1][4][1] = NIX;
  matrix[1][4][2] = normalMode;
  
  matrix[2][0][0] = NIX;
  matrix[2][0][1] = NIX;
  matrix[2][0][2] = NIX;
  matrix[2][1][0] = NIX;
  matrix[2][1][1] = NIX;
  matrix[2][1][2] = NIX;
  matrix[2][2][0] = NIX;
  matrix[2][2][1] = NIX;
  matrix[2][2][2] = NIX;
  matrix[2][3][0] = NIX;
  matrix[2][3][1] = NIX;
  matrix[2][3][2] = NIX;
  matrix[2][4][0] = NIX;
  matrix[2][4][1] = editMode;
  matrix[2][4][2] = normalMode;
  
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
  field.x = ((point.x + TOUCHXCORRECTION) / TOUCHXDIVIDE);
  field.y = ((point.y + TOUCHYCORRECTION) / TOUCHYDIVIDE);
  field.z = point.z;

//Debuging-output
  MonitorOutCoordinate(point);
  MonitorOutField(field);
  MonitorOutCommand(field);
  delay(333);

}

//Draws the grid on the tft, so we know where the fields of touchscreen are
void drawGrid() {
  
  int w = tft.width();
  int h = tft.height();
  
  tft.fillScreen(BLACK);
  
  for (int y=0; y<h; y+=(h/TFTYDIVIDE))
    tft.drawFastHLine(0, y, w, RED);
    
  for (int x=0; x<w; x+=(w/TFTXDIVIDE))
    tft.drawFastVLine(x, 0, h, BLUE);
}

//Information for debugging
void MonitorOutField(TSPoint point) {
  
  if (point.z > ts.pressureThreshhold) {
    Serial.print("Field (x/y): (");
    Serial.print(point.x);
    Serial.print("/");
    Serial.print(point.y);
    Serial.println(")");
  }
}

//Information for debugging
void MonitorOutCoordinate(TSPoint point) {
  
  if (point.z > ts.pressureThreshhold) {
    Serial.print("Coordinate (x/y): (");
    Serial.print(point.x + TOUCHXCORRECTION);
    Serial.print("/");
    Serial.print(point.y + TOUCHYCORRECTION);
    Serial.println(")");
  }
}

//Information for debugging
void MonitorOutCommand(TSPoint point) {
  
  if (point.z > ts.pressureThreshhold) {
    Serial.print("Mode: ");
    switch (currentMode)
    {
      case normal:
        Serial.println("normal");
        break;
      case edit:
        Serial.println("edit");
        break;
      case load:
        Serial.println("load");
        break;
    }
    Serial.print("Command: (");
    Serial.print(matrix[currentMode][point.x][point.y]);
    Serial.print(") - ");
    switch (matrix[currentMode][point.x][point.y])
    {
      case previous:
        Serial.println("previous");
        break;
      case next:
        Serial.println("next");
        break;
      case playpause:
        Serial.println("playpause");
        break;
      case normalMode:
        Serial.println("normalMode");
        break;
      case editMode:
        Serial.println("editMode");
        break;
      case loadMode:
        Serial.println("loadMode");
        break;
      case NIX:
        Serial.println("NIX");
        break;
    }
    Serial.println("-------------------------");
  }
}
