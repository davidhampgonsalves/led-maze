#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include "file.h"

const char* HIGH_SCORE_PATH = "/high-scores.txt";
int MAX_CHARS_PER_LINE = 20;

char hsName1[NAME_MAX_LEN];
char hsName2[NAME_MAX_LEN];
char hsName3[NAME_MAX_LEN];
long highScore1;
long highScore2;
long highScore3;

void readHighScores() {
  File file = SD.open(HIGH_SCORE_PATH);

  int len = file.readBytesUntil(',', hsName1, NAME_MAX_LEN);
  hsName1[len] = '\0';
  highScore1 = file.parseInt();
  file.read();

  len = file.readBytesUntil(',', hsName2, NAME_MAX_LEN);
  hsName2[len] = '\0';
  highScore2 = file.parseInt();
  file.read();

  len = file.readBytesUntil(',', hsName3, NAME_MAX_LEN);
  hsName3[len] = '\0';
  highScore3 = file.parseInt();

  file.close();
}

bool isHighScore(long score) {
  return score > highScore3;
}

void writeHighScore(char* name, long score) {
    char buffer[20];

    File file = SD.open("highscores.txt");
    if(score > highScore1) {
      sprintf(buffer, "%s,%ld", name, score);
      file.println(buffer);
      sprintf(buffer, "%s,%ld", hsName1, highScore1);
      file.println(buffer);
      sprintf(buffer, "%s,%ld", hsName2, highScore2);
      file.println(buffer);
    } else if(score > highScore2) {
      sprintf(buffer, "%s,%ld", hsName1, highScore1);
      file.println(buffer);
      sprintf(buffer, "%s,%ld", name, score);
      file.println(buffer);
      sprintf(buffer, "%s,%ld", hsName2, highScore2);
      file.println(buffer);
    } else {
      sprintf(buffer, "%s,%ld", hsName1, highScore1);
      file.println(buffer);
      sprintf(buffer, "%s,%ld", hsName2, highScore2);
      file.println(buffer);
      sprintf(buffer, "%s,%ld", name, score);
      file.println(buffer);
    }

    file.close();

    readHighScores();
}

void readFile(const char* path, char* out) {
  File file = SD.open(path);
  int i=0;
  for( ; file.available() ; i++){
    out[i] = file.read();
  }
  out[i] = '\0';
  file.close();
}

void initSd() {
  if(!SD.begin(5)) Serial.println("Card Mount Failed");
}