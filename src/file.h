#pragma once

const int MAX_HIGHSCORES = 3;
const int NAME_MAX_LEN = 4;

struct HighScore {
    char name[NAME_MAX_LEN];
    long score;
};

struct ScoreList {
    HighScore scores[MAX_HIGHSCORES];
};

bool isHighScore(long score);
ScoreList readHighScores();
void writeHighScore(char* name, long score);

void readFile(const char* path, char*);
void initSd();