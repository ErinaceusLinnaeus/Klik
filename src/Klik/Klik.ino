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
#define BLACK     0x0000
#define DARKGREY  0x7BEF
#define LIGHTGREY 0xBDF7
#define WHITE     0xFFFF
#define BROWN     0x79E0
#define RED       0xF800
#define GREEN     0x07E0
#define BLUE      0x001F
#define CYAN      0x07FF
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define ORANGE    0xFBE0

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin

//If the touchscreen's zero-point doesn't start at 0
#define TOUCHXCORRECTION -120
#define TOUCHYCORRECTION -150

//How wide(pixel) the buttons are
#define TOUCHXDIVIDE 240
#define TOUCHYDIVIDE 240

//How wide(pixel) the buttons are
#define TFTXDIVIDE 3
#define TFTYDIVIDE 4

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
typedef enum {previous, next, play, stop,
            normalMode, editMode, loadMode,
            NIX} command;
int matrix[mode(NumOfModes)][TFTYDIVIDE][TFTXDIVIDE];

//Keep the current mode and status in mind
mode currentMode = normal;
command currentStatus = stop;

  
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
  
//filling the array
  matrix[0][0][0] = next;
  matrix[0][0][1] = play;
  matrix[0][0][2] = play;
  matrix[0][1][0] = stop;
  matrix[0][1][1] = play;
  matrix[0][1][2] = play;
  matrix[0][2][0] = previous;
  matrix[0][2][1] = play;
  matrix[0][2][2] = play;
  matrix[0][3][0] = loadMode;
  matrix[0][3][1] = editMode;
  matrix[0][3][2] = normalMode;
  
  matrix[1][0][0] = NIX;
  matrix[1][0][1] = NIX;
  matrix[1][0][2] = NIX;
  matrix[1][1][0] = NIX;
  matrix[1][1][1] = NIX;
  matrix[1][1][2] = NIX;
  matrix[1][2][0] = NIX;
  matrix[1][2][1] = NIX;
  matrix[1][2][2] = NIX;
  matrix[1][3][0] = loadMode;
  matrix[1][3][1] = editMode;
  matrix[1][3][2] = normalMode;
  
  matrix[2][0][0] = NIX;
  matrix[2][0][1] = NIX;
  matrix[2][0][2] = NIX;
  matrix[2][1][0] = NIX;
  matrix[2][1][1] = NIX;
  matrix[2][1][2] = NIX;
  matrix[2][2][0] = NIX;
  matrix[2][2][1] = NIX;
  matrix[2][2][2] = NIX;
  matrix[2][3][0] = loadMode;
  matrix[2][3][1] = editMode;
  matrix[2][3][2] = normalMode;
  
  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(3);
  tft.fillScreen(BLACK);


  drawButtons();
  
//Grid for debug and development reasons
//  drawGrid();
  
  band currBand;
  //Later replaced by stuff loaded from the SD card, maybe
  strcpy(currBand.bandname, "Lister");
  strcpy(currBand.song[0].songname, "Sofdu");
  currBand.song[0].tempo = 78;
  strcpy(currBand.song[1].songname, "Solo");
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

//Draws the symbols/buttons
void drawButtons() {
  
  int w = tft.width();
  int h = tft.height();
  
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  
  for (int x=0; x<TFTXDIVIDE; x++) {
    for (int y=0; y<TFTYDIVIDE; y++) {
      
      switch (matrix[currentMode][TFTYDIVIDE-y-1][TFTXDIVIDE-x-1])
      {
        case previous:
          tft.fillCircle((y*(w/TFTYDIVIDE)+40), (x*(h/TFTXDIVIDE)+40), 36, LIGHTGREY);
          tft.fillRoundRect((y*(w/TFTYDIVIDE)+22), (x*(h/TFTXDIVIDE)+22), 12, 36, 2, BLACK);
          tft.fillTriangle((y*(w/TFTYDIVIDE)+58), (x*(h/TFTXDIVIDE)+23),
                           (y*(w/TFTYDIVIDE)+34), (x*(h/TFTXDIVIDE)+39),
                           (y*(w/TFTYDIVIDE)+58), (x*(h/TFTXDIVIDE)+57), BLACK);
          break;
        case next:
          tft.fillCircle((y*(w/TFTYDIVIDE)+40), (x*(h/TFTXDIVIDE)+40), 36, LIGHTGREY);
          tft.fillRoundRect((y*(w/TFTYDIVIDE)+46), (x*(h/TFTXDIVIDE)+22), 12, 36, 2, BLACK);
          tft.fillTriangle((y*(w/TFTYDIVIDE)+22), (x*(h/TFTXDIVIDE)+23),
                           (y*(w/TFTYDIVIDE)+46), (x*(h/TFTXDIVIDE)+39),
                           (y*(w/TFTYDIVIDE)+22), (x*(h/TFTXDIVIDE)+57), BLACK);
          break;
        case play:
          break;
        case stop:
          if (currentStatus == stop)
            tft.fillCircle((y*(w/TFTYDIVIDE)+40), (x*(h/TFTXDIVIDE)+40), 36, YELLOW);
          else
            tft.fillCircle((y*(w/TFTYDIVIDE)+40), (x*(h/TFTXDIVIDE)+40), 36, LIGHTGREY);
          tft.fillRoundRect((y*(w/TFTYDIVIDE)+22), (x*(h/TFTXDIVIDE)+22), 36, 36, 2, BLACK);
          break;
        case normalMode:
          if (currentMode == normal)
            tft.fillRoundRect((y*(w/TFTYDIVIDE)+10), (x*(h/TFTXDIVIDE)+10), 60, 60, 2, YELLOW);
          else
            tft.fillRoundRect((y*(w/TFTYDIVIDE)+10), (x*(h/TFTXDIVIDE)+10), 60, 60, 2, LIGHTGREY);
          tft.setCursor(y*(w/TFTYDIVIDE)+18,x*(h/TFTXDIVIDE)+30);
          tft.println("PLAY");
          break;
        case editMode:
          if (currentMode == edit)
            tft.fillRoundRect((y*(w/TFTYDIVIDE)+10), (x*(h/TFTXDIVIDE)+10), 60, 60, 2, YELLOW);
          else
            tft.fillRoundRect((y*(w/TFTYDIVIDE)+10), (x*(h/TFTXDIVIDE)+10), 60, 60, 2, LIGHTGREY);
          tft.setCursor(y*(w/TFTYDIVIDE)+18,x*(h/TFTXDIVIDE)+30);
          tft.println("EDIT");
          break;
        case loadMode:
          if (currentMode == load)
            tft.fillRoundRect((y*(w/TFTYDIVIDE)+10), (x*(h/TFTXDIVIDE)+10), 60, 60, 2, YELLOW);
          else
            tft.fillRoundRect((y*(w/TFTYDIVIDE)+10), (x*(h/TFTXDIVIDE)+10), 60, 60, 2, LIGHTGREY);
          tft.setCursor(y*(w/TFTYDIVIDE)+18,x*(h/TFTXDIVIDE)+30);
          tft.println("LOAD");
          break;
        case NIX:
          break;
      }
          Serial.print("y*(w/TFTYDIVIDE)+2 : ");
          Serial.print(y);
          Serial.print("*(");
          Serial.print(w);
          Serial.print("/");
          Serial.print(TFTYDIVIDE);
          Serial.print(")+2 = ");
          Serial.println(y*(w/TFTYDIVIDE)+2);
          
          Serial.print("x*(h/TFTXDIVIDE)+2 : ");
          Serial.print(x);
          Serial.print("*(");
          Serial.print(h);
          Serial.print("/");
          Serial.print(TFTXDIVIDE);
          Serial.print(")+2 = ");
          Serial.println(x*(h/TFTXDIVIDE)+2);
    }
  }
}

//Draws the grid on the tft, so we know where the fields of touchscreen are
//Also draw what command is assigned to what field 
void drawGrid() {
  
  int w = tft.width();
  int h = tft.height();
  
  tft.setTextColor(DARKGREY);
  tft.setTextSize(1);
  
  for (int y=0; y<=w; y+=(w/TFTYDIVIDE))
    tft.drawFastHLine(0, y, w, LIGHTGREY);
  tft.drawFastHLine(0, h-1, w, LIGHTGREY);
    
  for (int x=0; x<=h; x+=(h/TFTXDIVIDE))
    tft.drawFastVLine(x, 0, h, LIGHTGREY);
  tft.drawFastVLine(w-1, 0, h, LIGHTGREY);

  for (int x=0; x<TFTXDIVIDE; x++) {
    for (int y=0; y<TFTYDIVIDE; y++) {
      
      tft.setCursor(y*(w/TFTYDIVIDE)+2,x*(h/TFTXDIVIDE)+2);
      switch (matrix[currentMode][TFTYDIVIDE-y-1][TFTXDIVIDE-x-1])
      {
        case previous:
          tft.print("previous");
          break;
        case next:
          tft.print("next");
          break;
        case play:
          tft.print("play");
          break;
        case stop:
          tft.print("stop");
          break;
        case normalMode:
          tft.print("normalMode");
          break;
        case editMode:
          tft.print("editMode");
          break;
        case loadMode:
          tft.print("loadMode");
          break;
        case NIX:
          tft.print("NIX");
          break;
      }
    }
  } 
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
      case play:
        Serial.println("play");
        break;
      case stop:
        Serial.println("stop");
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
