/* (C)2021 Benji
 *  Version 1.0

    Klik is a metronom
    Parts:
      Arduino Due
      Stereo Trigger Pad (Piezo, Rimswitch)

      
    Stereo Pad: Piezo & Rimswitch
      3V between Ring & Sleve
      Piezo at Tip (cinch: black)
      Rimswitch at ring (cinch: red)

      Sleve: 0V
      Ring: analog pullup A1
      Tip: analog pullup A0

    Line-Out an PWM 5

*/

//~ 120.000 cycles/second
// -> At 10.000 -> 12 c/s
// -> 720 c/min
// -> 180 bpm at 2 times resolution
#define CYCLE 10000

//How big/long the Array for the timer is
#define TIMER_ARRAY_SIZE 8

//Where the TriggerPad is connected to
int pinPiezo = A0;
int pinRimSwitch = A1;

//Where the Outputjack is connected to
int pinLineOut = 5;

//An Array where the trigger inputs during a cycle a stored
int piezo[CYCLE];
int rim[CYCLE];

//Here we store the default state values of the connected Trigger Pad
int global_min_piezo = 1023;
int global_min_rim = 1023;

//A temporary storage for millis()
//so we can calculate the difference between two hits, therefor the new bpm
unsigned long lastMillis = 0;
//Timer Array, let´s us calculate the average tempo
unsigned int timer[TIMER_ARRAY_SIZE];
unsigned char posInTimerArray = 0;
//Default tempo of 750 ms = 90 bpm
unsigned int tempo = 750;

unsigned long lastBeep = 0;

//Boolean to turn the metronom on/off
bool metronomActive = false;

//Play the very simple 500Hz metronom beep
void playBeep() {

  analogWrite(pinLineOut, 1023);
  delay(1);
  analogWrite(pinLineOut, 0);
  delay(1);
  analogWrite(pinLineOut, 512);
  delay(1);
  analogWrite(pinLineOut, 0);
  delay(1);
  analogWrite(pinLineOut, 265);
  delay(1);
  analogWrite(pinLineOut, 0);

  //Just for debuging
//  Serial.println("BEEP");
}

void setup() {

  //Usual initializing stuff
  Serial.begin(9600);

  //The pins
  pinMode(pinPiezo, INPUT_PULLUP);
  pinMode(pinRimSwitch, INPUT_PULLUP);
  
  //Finding the default state of the Trigger Pad
  //And doing it 20 times to get a more accurate result
  //Takes about 2 seconds
  for(int metatick = 0; metatick < 20; metatick++){
    for(int tick = 0; tick < CYCLE; tick++){
      
      piezo[tick] = analogRead(pinPiezo);
      rim[tick] = analogRead(pinRimSwitch);
      
      if (piezo[tick] < global_min_piezo)
        global_min_piezo = piezo[tick];
        
      if (rim[tick] < global_min_rim)
        global_min_rim = rim[tick];
    }
  }
  
  //Just to give the rim some wiggle space, it is very sensitiv by default
  global_min_rim -= 250;

  //Output some info
  Serial.println("--------------------------------------");
  Serial.println("Default-State for this Trigger Pad is:");
  Serial.print("Piezo: ");
  Serial.println(global_min_piezo);
  Serial.print("Rim  : ");
  Serial.println(global_min_rim);
  Serial.println("--------------------------------------");
}

void loop() {

  //Re-setting last cycles minimal messured values
  short min_piezo = 1023;
  short min_rim = 1023;

  //Find this cycles "most" minimal input
  for(int tick = 0; tick < CYCLE; tick++){
    piezo[tick] = analogRead(pinPiezo);
    rim[tick] = analogRead(pinRimSwitch);
    
    if (piezo[tick] < min_piezo)
      min_piezo = piezo[tick];
    
    if (rim[tick] < min_rim)
      min_rim = rim[tick];
  }

  //Turn the metronom on/off
  if (min_rim < global_min_rim){
    
    metronomActive = !metronomActive;
    /*
    Serial.print("Signal at Rim: ");
    Serial.println(min_rim);
    Serial.print("Metronom is: ");
    Serial.println(metronomActive);
    */
  }
  //Only do this when the metronom is turned on
  if (metronomActive && min_piezo < global_min_piezo){

    //Get the difference between two triggers
    unsigned long tempMillis = millis();
    unsigned long tempTimer = tempMillis - lastMillis;

    //If the new tempo would be 30 bpm, let's assume that this is a new bpm messurement
    if (tempTimer > 2000){
      posInTimerArray = 0;
      for (int i=0; i<TIMER_ARRAY_SIZE; i++){
        timer[i] = 0;
      }
    }

    //Fill the messured time into the array...
    timer[posInTimerArray] = tempTimer;
    //...and store the last messurement
    lastMillis = tempMillis;

    //Go to the next step in the array or the first
    posInTimerArray++;
    if (posInTimerArray == TIMER_ARRAY_SIZE)
      posInTimerArray = 0;

    /*
    Serial.println("-----------");
    Serial.println("Tempos: ");
    for (int i=0; i<TIMER_ARRAY_SIZE; i++){
      Serial.print("timer[");
      Serial.print(i);
      Serial.print("] = ");
      Serial.println(timer[i]);
    }
    */

    //Calculate the average of the stored tempos
    //But count the zeros and...
    unsigned short zeroInTimer = 0;
    for (int i=1; i<TIMER_ARRAY_SIZE; i++){
      tempTimer += timer [i];
      if (timer[i] == 0)
        zeroInTimer++;
    }
    //...ignore those zeros
    tempTimer /= (TIMER_ARRAY_SIZE - zeroInTimer);
    tempo = tempTimer;
    
    /*
    Serial.print("New tempTimer: ");
    Serial.print(tempTimer);
    Serial.println(" ms");
    Serial.print("New tempo is: ");
    Serial.print(60000/tempo);
    Serial.println(" bpm");
    */
  }

  //If the actual time overtakes the lastBeep and the tempo...
  unsigned long tempMillis = millis();
  if (metronomActive && (lastBeep + tempo < tempMillis)){
    //...it's time to play another beep
    playBeep();
    lastBeep = tempMillis;
//    Serial.println("BEEEEEEP");
  }
  
}
