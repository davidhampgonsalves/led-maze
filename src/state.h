const int TITLE = 0;
const int GAME_START = 1;
const int PLAYING = 2;
const int HIGHSCORE = 3;
const int DEMO = 4;

static int state = TITLE;

void updateState(int s) { state = s; };