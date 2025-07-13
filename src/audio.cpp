#include <Arduino.h>
#include "AudioTools.h"
#include "AudioTools/Disk/FileLoop.h"
#include "AudioTools/AudioLibs/AudioESP32ULP.h"
#include "SD.h"

WAVDecoder wav;
// AnalogAudioStream out;
AudioESP32ULP out;
EncodedAudioOutput decoder(&out, &wav);  // Decoding stream
AudioInfo info(8000, 1, 16);
File audioFile;
FileLoop loopingFile;
StreamCopy copier(decoder, loopingFile);

SineWaveGenerator<int16_t> sineWave1(8000);
GeneratedSoundStream<int16_t> sound1(sineWave1);

void initAudio() {
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);
  // AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

  auto config = out.defaultConfig();
  config.copyFrom(info);
  // config.channels = 1;

  out.setMonoDAC(ULP_DAC2);
  out.begin(config);
}

void playWav(const char* path) {
  loopingFile.setFile(SD.open(path));
  loopingFile.begin();

  decoder.begin();
}

// void playDeath() { }



// AudioInfo info(8000, 1, 16);
// SineWaveGenerator<int16_t> sineWave(32000);                // subclass of SoundGenerator with max amplitude of 32000
// GeneratedSoundStream<int16_t> sound(sineWave);             // Stream generated from sine wave
// // AnalogAudioStream out;
// AudioESP32ULP out;
// StreamCopy copier(out, sound);                             // copies sound into i2s

// void initAudio() {
//   AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

//   auto config = out.defaultConfig();
//   config.copyFrom(info);
//   config.channels = 1;

//   out.setMonoDAC(ULP_DAC2);
//   out.begin(config);

//   // Setup sine wave
//   sineWave.begin(info, N_B4);
// }
// void playWav(const char* path) { }

void playAudio() { copier.copy(); }