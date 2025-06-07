#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <vector>

#include "file.h"

std::vector<HighScore> readHighScores() {
  File file = SD.open("/high-scores.txt");
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

std::string readFile(const char* path) {
  File file = SD.open(path);
  std::string contents = "";
  while(file.available()){
    contents += file.read();
  }
  file.close();

  return contents;
}

// void writeFile(fs::FS &fs, const char * path, const char * message){
//   Serial.printf("Writing file: %s\n", path);

//   File file = fs.open(path, FILE_WRITE);
//   if(!file){
//     Serial.println("Failed to open file for writing");
//     return;
//   }
//   if(file.print(message)){
//     Serial.println("File written");
//   } else {
//     Serial.println("Write failed");
//   }
//   file.close();
// }

void init(){
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