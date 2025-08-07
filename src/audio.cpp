#include <Arduino.h>
#include "AudioTools.h"
#include "AudioTools/AudioLibs/AudioESP32ULP.h"
#include "SD.h"

AudioESP32ULP out;
AudioInfo info(8000, 1, 16);

File soundFile;
File songFile;

WAVDecoder wav;
WAVDecoder effectWav;

InputMixer<int16_t> mixer;

// EncodedAudioOutput decoder(&out, &wav);  // Decoding stream
// EncodedAudioOutput effectDecoder(&out, &effectWav);  // Decoding stream

// OutputMixer<int16_t> mixer(out, 2); // if you set this to 2 (as I think you should) audio plays too fast

StreamCopy copier(out, mixer);
// StreamCopy effectCopier(mixer, volume);


// PoppingSoundRemover<int16_t> converter(1, true, true);
// VolumeStream volume(soundFile);

void playSong(const char* path) {
  // Serial.printf("playing song %s\n", path);

  if(songFile) songFile.close();
  // songFile = SD.open(path);
  songFile = SD.open("/sounds/silence.wav");
}

void playSound(const char* path) {
  // if(!effectCopier.isActive()) return; // only one sound at a time

  // volume.setVolume(0);
  // if(soundFile) soundFile.close();
  // soundFile = SD.open(path);
  soundFile = SD.open("/sounds/silence.wav");
  // volume.setVolume(100);
}

void initAudio() {
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);
  // AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

  auto config = out.defaultConfig();
  config.copyFrom(info);

  out.setMonoDAC(ULP_DAC2);
  out.begin(config);

  mixer.add(wav);
  mixer.add(effectWav);
  mixer.begin();

  // always need to play something so that the steams are in sync
  playSound("/sounds/silence.wav");
  playSong("/sounds/silence.wav");

  // copier.setMinCopySize(1);

  // decoder.begin();
  // effectDecoder.begin();
}

void playAudio() {
  // if(!copier.available()) songFile.seek(0);
  // if(!effectCopier.available()) playSound("/sounds/silence.wav");

  copier.copy();
  // effectCopier.copy();
}

void stopSong() {
  playSong("/sounds/silence.wav");
}