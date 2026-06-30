//Very much inspired by https://www.dfrobot.com/blog-1462.html by DFRobot Feb 26 2020
//Additions made by Just Baselmans https://www.youtube.com/justbaselmansYT Jan 23 2023

#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define MIDDLE_PIN 6
#define RIGHT_PIN 8

// Initialize software serial on pins 10 and 11
SoftwareSerial mySoftwareSerial(12, 10);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;

int folder = 1;
int song = 1;

// each unit here corresponds to 100ms
int doubleclickBuffer = 0;
int rightholdBuffer = 0;

bool leftInput = false;
bool middleInput = false;
bool rightInput = false;

bool playing = false;
bool songMode = true; // if true, selecting songs. if false, selecting folders

void setup() {
  //setting up pins
  pinMode(MIDDLE_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);

  // Serial communication with the module
  mySoftwareSerial.begin(9600);

  // Initialize Arduino serial
  Serial.begin(115200);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini"));
  Serial.println(F("Initializing DFPlayer module ... Wait!"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Not initialized:"));
    Serial.println(F("1. Check the DFPlayer Mini connections"));
    Serial.println(F("2. Insert an SD card"));
    while (true) {
      delay(100);
    }
  }

  Serial.println();
  Serial.println(F("DFPlayer Mini module initialized!"));

  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(15);
  myDFPlayer.EQ(0);

  changeMusic();
}

void changeMusic() {
  myDFPlayer.playFolder(folder, song);
  playing = true;
}

void loop() {

  middleInput = !digitalRead(MIDDLE_PIN);
  rightInput = !digitalRead(RIGHT_PIN);

  // ---------------- MIDDLE BUTTON ----------------

  if (middleInput) {

    if (doubleclickBuffer > 0) {
      Serial.println("FUCKIN CHANGE MODE");
      songMode = !songMode;
      doubleclickBuffer = 0;

      while (!digitalRead(MIDDLE_PIN)) {
        delay(50);
      }

      return;
    }

    if (playing) {
      playing = false;
      myDFPlayer.pause();
    } else {
      playing = true;
      myDFPlayer.start();
    }

    while (!digitalRead(MIDDLE_PIN)) {
      delay(50);
    }

    doubleclickBuffer = 7;
  }

  // ---------------- RIGHT BUTTON ----------------

  if (rightInput) {

    bool holdTriggered = false;
    rightholdBuffer = 0;

    while (!digitalRead(RIGHT_PIN)) {

      delay(100);
      rightholdBuffer++;

      if (!holdTriggered && rightholdBuffer >= 10) {

        holdTriggered = true;

        Serial.println("PREVIOUS MUSIC");

        if (songMode) {
          song = max(song - 1, 1);
        } else {
          folder = max(folder - 1, 1);
          song = 1;
        }

        changeMusic();
      }
    }

    if (!holdTriggered) {

      Serial.println("NEXT MUSIC");

      if (songMode) {
        song = min(song + 1, 99);
      } else {
        folder = min(folder + 1, 99);
        song = 1;
      }

      changeMusic();
    }
  }

  // ---------------- BUFFERS ----------------

  doubleclickBuffer = max(doubleclickBuffer - 1, 0);

  Serial.print(digitalRead(MIDDLE_PIN));
  Serial.print(digitalRead(RIGHT_PIN));
  Serial.println(rightholdBuffer);

  delay(100);
}