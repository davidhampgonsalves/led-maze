#include <Arduino.h>
#include "AudioTools.h"
#include "AudioTools/AudioLibs/AudioESP32ULP.h"
#include "SD.h"

WAVDecoder wav;
WAVDecoder effectWav;
AudioESP32ULP out;

EncodedAudioOutput decoder(&out, &wav);  // Decoding stream
EncodedAudioOutput effectDecoder(&out, &effectWav);  // Decoding stream
AudioInfo info(8000, 1, 16);
File soundFile;
File songFile;
OutputMixer<int16_t> mixer(out, 2); // if you set this to 2 (as I think you should) audio plays too fast
StreamCopy copier(mixer, songFile);
StreamCopy effectCopier(mixer, soundFile);

void playSong(const char* path) {
  Serial.printf("playing song %s\n", path);

  if(songFile != NULL) songFile.close();
  songFile = SD.open(path);
}

void playSound(const char* path) {
  if(!effectCopier.isActive()) return; // only one sound at a time

  if(soundFile != NULL) soundFile.close();
  soundFile = SD.open(path);
}

void initAudio() {
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);
  // AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

  auto config = out.defaultConfig();
  config.copyFrom(info);

  out.setMonoDAC(ULP_DAC2);
  out.begin(config);

  mixer.begin();

  // always need to play something so that the steams are in sync
  playSound("/sounds/silence.wav");
  playSong("/sounds/silence.wav");

  decoder.begin();
  effectDecoder.begin();
}

void playAudio() {
  if(!copier.available()) songFile.seek(0);
  if(!effectCopier.available()) playSound("/sounds/silence.wav");

  copier.copy();
  effectCopier.copy();
}

void stopSong() {
  playSong("/sounds/silence.wav");
}