#include <Arduino.h>
#include "Game_Audio.h"
#include "sounds.h"
#include "file.h"

Game_Audio_Class GameAudio(26,0);
Game_Audio_Wav_Class pmWav(pacmanDeath);
Game_Audio_Wav_Class *wav = NULL;

void playWav(const char* path, unsigned char* buff) {
  delay(100);
  Serial.println("about to read");
  delay(100);
  readWav(path, buff);
  Serial.println("about to create");
  delay(100);

  wav = new Game_Audio_Wav_Class(buff);

  Serial.println("about to play, for:");
  Serial.println(wav->getDuration());
  Serial.println("sample rate: ");
  Serial.println(wav->getSampleRate());
  delay(100);

  GameAudio.PlayWav(wav, true, 1.0);
}

void playDeath() {
  GameAudio.PlayWav(&pmWav, false, 1.0);
}
