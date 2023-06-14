#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "AudioFile.h"
#include "doctest.h"
TEST_CASE("Open Wrong File Name") {
  AudioFile aud;
  aud.MainPath = "te.wav";
  aud.OutputFilepath = "output.wav";
  CHECK_THROWS_AS(aud.reverseAudio(), std::runtime_error);
  CHECK_THROWS_AS(aud.removeSilence(88000), std::runtime_error);
  aud.MainPath = "test3.q";
  CHECK_THROWS_AS(aud.reverseAudio(), std::runtime_error);
  CHECK_THROWS_AS(aud.removeSilence(88000), std::runtime_error);
}

TEST_CASE("Wrong File format") {
  AudioFile aud;
  aud.MainPath = "test5.wav";
  aud.OutputFilepath = "output.wav";
  CHECK_THROWS_AS(aud.CutAudio(3.2, 6), std::runtime_error);
  CHECK_THROWS_AS(aud.applySilence(3.2, 6), std::runtime_error);
}
TEST_CASE("Times Check") {
  AudioFile aud;
  aud.MainPath = "test1.wav";
  aud.SecondFilePath = "test2.wav";
  aud.OutputFilepath = "output.wav";
  CHECK_NOTHROW(aud.CutAudio(3.2, 6));
  CHECK_NOTHROW(aud.applySilence(3.2, 6));
  CHECK_NOTHROW(aud.speedUpAudioPlayback(2));
  CHECK_THROWS_AS(aud.applySilence(-3.2, 6), std::invalid_argument);
  CHECK_THROWS_AS(aud.speedUpAudioPlayback(-2), std::invalid_argument);
  CHECK_THROWS_AS(aud.CutAudio(0, -9), std::invalid_argument);
  CHECK_NOTHROW(aud.speedUpAudioPlayback(2));
}
