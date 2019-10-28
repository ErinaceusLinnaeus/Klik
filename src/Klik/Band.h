/*  (C)2019 Benji Schmid
 *   
 *  Band.h
 *  Definition of classes to create a list of songs
 */

#ifndef BAND_H
#define BAND_H

#include <stdint.h>

class Song {
  
  private:
    //The Data
    char songname[31];
    uint32_t tempo;

  public:
    //The Nodes
    Song* nextSong;
    
    //Constructors
    Song();

    //Deconstructor
    ~Song();

    //Getting Data
    char* getSongName();
    uint32_t getTempo();

    //Manipulating Data
    void setSongName(Song *song, char songname[31]);
    void setTempo(Song *song, uint32_t tempo);
};

class Band {

  private:
    //The Data
    char bandname[31];
    
    //The Songs
    Song* firstSong;
    Song* currSong;

  public:
    //Constructor
    Band();

    //Manage Songs
    void newSong(char songname[31], uint32_t tempo);
    void delSong();
    
    //Getting Data
    char* getBandName();
    Song* getFirstSong();
    Song* getCurrSong();
    
    //Manipulating Data
    void saveBand();
    void setBandName(char bandname[31]);

    //Navigating the Songs
    void goToNextSong();
    void goToTheTop();
};

#endif
