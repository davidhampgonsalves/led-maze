#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include "file.h"

const char* HIGH_SCORE_PATH = "/high-scores.txt";
int MAX_CHARS_PER_LINE = 20;

void sortScores(ScoreList& list) {
    for (int i = 0; i < MAX_HIGHSCORES - 1; i++) {
        for (int j = 0; j < MAX_HIGHSCORES - i - 1; j++) {
            if (list.scores[j].score < list.scores[j + 1].score) {
                HighScore temp = list.scores[j];
                list.scores[j] = list.scores[j + 1];
                list.scores[j + 1] = temp;
            }
        }
    }
}


ScoreList readHighScores() {
    ScoreList list;
    int scoreCount = 0;

    File file = SD.open(HIGH_SCORE_PATH);
    char lineBuffer[MAX_CHARS_PER_LINE];
    Serial.println("Read scores");
    while (file.available() && scoreCount < MAX_HIGHSCORES) {
        int bytesRead = file.readBytesUntil('\n', lineBuffer, MAX_CHARS_PER_LINE - 1);
        lineBuffer[bytesRead] = '\0';

        char* name_token = strtok(lineBuffer, ":");
        char* score_token = strtok(NULL, ":");

        strncpy(list.scores[scoreCount].name, name_token, NAME_MAX_LEN - 1);
        list.scores[scoreCount].name[NAME_MAX_LEN - 1] = '\0';
        list.scores[scoreCount].score = atol(score_token);

        Serial.printf("  %d %s %ld\n", scoreCount, list.scores[scoreCount].name, list.scores[scoreCount].score);

        scoreCount++;
    }
    file.close();

    return list;
}

bool isHighScore(long score) {
  return score > readHighScores().scores[MAX_HIGHSCORES - 1].score;
}

void writeHighScore(char* name, long score) {
    ScoreList list = readHighScores();

    long lowestScore = list.scores[MAX_HIGHSCORES - 1].score;

    strncpy(list.scores[MAX_HIGHSCORES - 1].name, name, NAME_MAX_LEN - 1);
    list.scores[MAX_HIGHSCORES - 1].name[NAME_MAX_LEN - 1] = '\0';
    for (int j = 0; list.scores[MAX_HIGHSCORES - 1].name[j] != '\0'; j++) {
      list.scores[MAX_HIGHSCORES - 1].name[j] = tolower(list.scores[MAX_HIGHSCORES - 1].name[j]);
    }
    list.scores[MAX_HIGHSCORES - 1].score = score;

    sortScores(list);

    File file = SD.open(HIGH_SCORE_PATH, FILE_WRITE);
    if (file) {
        for (int i = 0; i < MAX_HIGHSCORES; i++) {
            file.print(list.scores[i].name);
            file.print(':');
            file.println(list.scores[i].score);
        }
        file.close();
    }
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
  if(!SD.begin(5)){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}