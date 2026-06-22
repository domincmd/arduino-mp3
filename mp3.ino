//Very much inspired by https://www.dfrobot.com/blog-1462.html by DFRobot Feb 26 2020
//Additions made by Just Baselmans https://www.youtube.com/justbaselmansYT Jan 23 2023

#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define LEFT_PIN 2
#define MIDDLE_PIN 3
#define RIGHT_PIN 4

// Initialize software serial on pins 10 and 11
SoftwareSerial mySoftwareSerial(10, 11);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;

int folder = 1;
int song = 1;


//each unit here corresponds to 100ms
int doubleclickBuffer = 0;

bool leftInput = false;
bool middleInput = false;
bool rightInput = false;

bool playing = false;
bool songMode = true; //if true, selecting songs. if false, selecting folders

void setup() {
  //setting up pins
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);

  // Serial communication with the module
  mySoftwareSerial.begin(9600);
  // Initialize Arduino serial
  Serial.begin(115200);
  // Check if the module is responding and if the SD card is found
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini"));
  Serial.println(F("Initializing DFPlayer module ... Wait!"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Not initialized:"));
    Serial.println(F("1. Check the DFPlayer Mini connections"));
    Serial.println(F("2. Insert an SD card"));
    while (true)
      ;
  }
  Serial.println();
  Serial.println(F("DFPlayer Mini module initialized!"));
  // Initial settings
  myDFPlayer.setTimeOut(500);  // Serial timeout 500ms
  myDFPlayer.volume(15);        // Volume 5
  myDFPlayer.EQ(0);            // Normal equalization
  //  myDFPlayer.EQ(DFPLAYER_EQ_POP);
  //  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
  //  myDFPlayer.EQ(DFPLAYER_EQ_JAZZ);
  //  myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);
  //  myDFPlayer.EQ(DFPLAYER_EQ_BASS);

  changeMusic();
}

void changeMusic() {
  myDFPlayer.playFolder(folder, song);
  playing = true;
}

void loop() {
  leftInput = !digitalRead(LEFT_PIN);
  middleInput = !digitalRead(MIDDLE_PIN);
  rightInput = !digitalRead(RIGHT_PIN);

  if (middleInput) {
    if (doubleclickBuffer > 0) { //this only repeats once
      Serial.println("FUCKIN CHANGE MODE");
      songMode = !songMode;
      doubleclickBuffer = 0;
      return;
    }
    if (playing) {
      playing = false;
      myDFPlayer.pause();
    }else{
      playing = true;
      myDFPlayer.start();
    }
    while (!digitalRead(MIDDLE_PIN)) { //this needs constant updating
      delay(50); //wait
    }
    doubleclickBuffer = 7;
  }

  if (leftInput) {
    if (songMode) {
      song = max(song-1, 1);
    }else{
      folder = max(folder-1, 1);
      song = 1;
    }
    changeMusic();

    while (!digitalRead(LEFT_PIN)) { //constant updating
      delay(50);
    }
  }

  if (rightInput) {
    if (songMode) {
      song = min(song+1, 99);
    }else{
      folder = min(folder+1, 99);
      song = 1;
    }
    changeMusic();

    while (!digitalRead(RIGHT_PIN)) { //constant updating
      delay(50);
    }
  }


  doubleclickBuffer = max(doubleclickBuffer-1, 0);
  Serial.println(doubleclickBuffer);
  
  delay(100);
}
