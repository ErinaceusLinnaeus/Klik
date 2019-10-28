/* (C)2019 Benjamin Schmid  
 *  
 *  Band.cpp
 */

#include "Arduino.h"
#include "Band.h"

Band::Band() {

  this->setBandName("newBand");

  this->firstSong = new Song();
  this->currSong = firstSong;

  saveBand();
}

void Band::newSong(char songname[31], uint32_t tempo) {

  Song* temp;
  temp = new Song();

  temp->setSongName(temp, songname);
  temp->setTempo(temp, tempo);
  
  temp->nextSong = currSong->nextSong;
  this->currSong->nextSong = temp;

  this->goToNextSong();
  delete temp;
}

void Band::delSong() {

  Song* temp;
  temp = this->firstSong;
  
  while(temp->nextSong != this->currSong){
    temp = temp->nextSong;
  }
  
  temp->nextSong = this->currSong->nextSong;

  temp = this->currSong;
  this->currSong = this->currSong->nextSong;

  delete temp;
}

char* Band::getBandName(){
  
  return this->bandname;
}

Song* Band::getFirstSong(){

  return this->firstSong;
}

Song* Band::getCurrSong(){

  return this->currSong;
}

//Save to SD Card
void Band::saveBand(){

}

void Band::setBandName(char bandname[31]){
  
  strcpy(this->bandname, bandname);
}

void Band::goToNextSong(){
  
  //It should loop back to first song after the last
  //...nee to test that...
  
  this->currSong = this->currSong->nextSong;
}

void Band::goToTheTop(){

  this->currSong = this->firstSong;
}

//BIS HIER BISHER
//////////////////////////////////////////

/*



//Default Constructor creates a song with 110 bpm
Song::Song() {

  //Defining default values for the first song
  strcpy(this->songname, "A new song");
  this->tempo = 110;
}


void AutoRocketNode::setAction(AutoRocketNode *node, command action){

  this->action = action;
}


void AutoRocketNode::setValue(AutoRocketNode *node, uint32_t value){

  this->value = value;
}


void AutoRocketNode::setInformation(AutoRocketNode *node, char information[31]) {
  
  strcpy(this->information, information);
//  saveScript();
}


command AutoRocketNode::getAction() {
  
  return this->action;
}

uint32_t AutoRocketNode::getValue() {
  
  return this->value;
}

char* AutoRocketNode::getInformation(){

  return this->information;
}



*/
