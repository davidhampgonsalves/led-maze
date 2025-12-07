#pragma once

const int MAX_HIGHSCORES = 3;
const int NAME_MAX_LEN = 4;

extern char hsName1[NAME_MAX_LEN];
extern char hsName2[NAME_MAX_LEN];
extern char hsName3[NAME_MAX_LEN];
extern long highScore1;
extern long highScore2;
extern long highScore3;

bool isHighScore(long score);
void readHighScores();
void writeHighScore(char* name, long score);

void readFile(const char* path, char*);
void initSd();