#include <Arduino.h>
#include "AudioTools.h"
#include "AudioTools/Disk/FileLoop.h"
#include "AudioTools/AudioLibs/AudioESP32ULP.h"
#include "SD.h"

WAVDecoder wav;
WAVDecoder effectWav;
AudioESP32ULP out;

EncodedAudioOutput decoder(&out, &wav);  // Decoding stream
EncodedAudioOutput effectDecoder(&out, &effectWav);  // Decoding stream
AudioInfo info(8000, 1, 16);
File soundFile;
FileLoop loopingFile;
OutputMixer<int16_t> mixer(out, 1); // if you set this to 2 (as I think you should) audio plays too fast
StreamCopy copier(mixer, loopingFile);
StreamCopy effectCopier(mixer, soundFile);

void initAudio() {
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);
  // AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

  auto config = out.defaultConfig();
  config.copyFrom(info);

  out.setMonoDAC(ULP_DAC2);
  out.begin(config);

  mixer.begin();
}

void playWav(const char* path) {
  loopingFile.setFile(SD.open(path));
  loopingFile.begin();

  decoder.begin();
}

void playDeath() {
  soundFile = SD.open("/death.wav");
  effectDecoder.begin();
}

void playAudio() {
  copier.copy();
  effectCopier.copy();
}