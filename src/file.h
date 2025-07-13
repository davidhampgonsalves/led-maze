#pragma once

#include <string>
#include <vector>

const int HIGH_SCORE_COUNT = 5;

struct HighScore {
  std::string name;
  long score;
};

void writeHighScore(std::string name, long score);
std::vector<HighScore> readHighScores();
std::string readFile(const char* path);
void readWav(const char* path, unsigned char*);
bool isHighScore(long score);