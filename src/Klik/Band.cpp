/* (C)2019 Benjamin Schmid  
 *  
 *  Band.cpp
 */

//#include "Arduino.h"
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

//Save to SD Card
void Band::saveBand() {

}

char* Band::getBandName(){
  
  return this->bandname;
}


//BIS HIER BISHER
//////////////////////////////////////////

/*


AutoRocketNode* AutoRocketList::getFirstNode(){
  
  return this->firstNode;
}

AutoRocketNode* AutoRocketList::getLastNode(){
  
  return this->lastNode;
}

AutoRocketNode* AutoRocketList::getCurrNode(){
  
  return this->currNode;
}



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

char* AutoRocketList::getFilename(){

  return this->filename;
}


void AutoRocketList::setFilename(char filename[31]) {
  
  strcpy(this->filename, filename);
//  saveScript();
}


void AutoRocketList::setDescription(char description[255]) {
  
  strcpy(this->description, description);
//  saveScript();
}


void AutoRocketList::goToNextNode(){

  //Don't try to jump over theExitNode
  if (this->currNode->nextNode != NULL)
    this->currNode = this->currNode->nextNode;
   else
    //We reached the end of the list
    NULL;
}


void AutoRocketList::goToPrevNode(){

  this->currNode = this->currNode->prevNode;
}

void AutoRocketList::goToTheTop(){

  this->currNode = this->firstNode;
}

*/
