#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <vector>

#include "file.h"

const char* HIGH_SCORE_PATH = "/high-scores.txt";
std::vector<HighScore> readHighScores() {
  File file = SD.open(HIGH_SCORE_PATH);
  std::vector<HighScore> highScores;
  if(!file){
    Serial.println("Failed to open file for reading");
  }

  std::string name = "";
  std::string score = "";
  bool isName = true;
  while(file.available()){
    char c = static_cast<char>(file.read());
    if(c == ',') isName = false;
    else if(c == '\n') {
      highScores.push_back({ name, std::stol(score) });
      name = "", score = "";
      isName = true;
    } else if(isName) name += c;
    else score += c;
  }
  file.close();

  return highScores;
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

bool isHighScore(long score) {
  auto scores = readHighScores();
  Serial.println("highscore?");
  Serial.println(scores.back().score < score);
  return scores.back().score < score;
}

void writeHighScore(std::string name, long score) {
  auto scores = readHighScores();
  int index = -1;
  for(int i=0 ; i < scores.size() ; i++)
    if(scores[i].score < score) index = i;

  scores.insert(scores.begin() + index, HighScore{ name, score });

  if(scores.size() > 3) scores.pop_back();

  File file = SD.open(HIGH_SCORE_PATH, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  char line [100];
  for(int i=0 ; i < scores.size() ; i++) {
    sprintf (line, "%s,%d", scores[i].name, scores[i].score);
    file.println(line);
  }
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