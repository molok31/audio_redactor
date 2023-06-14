#include "AudioFile.h"
/** Компилируемый файл, используется для консольного управления командами.
 */
int main() {
  /// Создание объекта класса аудиозаписи
  AudioFile aud;
  int choice = 0;
  while (choice != 8) {
    /// Вывод меню
    std::cout << "======== Menu =======\n";
    std::cout << "1. Merge 2 audio tracks\n";
    std::cout << "2. Cut audio file\n";
    std::cout << "3. Overlay 2 audio tracks on top of each other\n";
    std::cout << "4. Speed up/slow down audio track\n";
    std::cout << "5. Reverse audio track\n";
    std::cout << "6. Cut the silence from the audio track\n";
    std::cout << "7. Put silence on the segment\n";
    std::cout << "8. Exit\n";
    std::cout << "Select an option (1-8): ";
    std::cin >> choice;
    switch (choice) {
    case 1:
      std::cout << "Enter the path to the first file\n";
      std::cin >> aud.MainPath;
      std::cout << "Enter the path to the second file\n";
      std::cin >> aud.SecondFilePath;
      std::cout << "Enter the name of the resulting file\n";
      std::cin >> aud.OutputFilepath;
      aud.mergeWavFiles();
      break;
    case 2:
      std::cout << "Enter the path to the first file\n";
      std::cin >> aud.MainPath;
      std::cout << "Enter the name of the resulting file\n";
      std::cin >> aud.OutputFilepath;
      std::cout << "Enter start time\n";
      double StartTime;
      std::cin >> StartTime;
      double EndTime;
      std::cout << "Enter end time\n";
      std::cin >> EndTime;
      aud.CutAudio(StartTime, EndTime);
      break;
    case 3:
      std::cout << "Enter the path to the first file\n";
      std::cin >> aud.MainPath;
      std::cout << "Enter the path to the second file\n";
      std::cin >> aud.SecondFilePath;
      std::cout << "Enter the name of the resulting file\n";
      std::cin >> aud.OutputFilepath;
      aud.mixWavFiles();
      break;
    case 4:
      std::cout << "Enter the path to the first file\n";
      std::cin >> aud.MainPath;
      std::cout << "Enter the name of the resulting file\n";
      std::cin >> aud.OutputFilepath;
      std::cout << "Enter time x. If x < 1, then the speed of reproduction "
                   "decreases, if x > 1, then the speed increases\n";
      double multiplier;
      std::cin >> multiplier;
      aud.speedUpAudioPlayback(multiplier);
      break;
    case 5:
      std::cout << "Enter the path to the first file\n";
      std::cin >> aud.MainPath;
      std::cout << "Enter the name of the resulting file\n";
      std::cin >> aud.OutputFilepath;
      aud.reverseAudio();
      break;
    case 6:
      std::cout << "Enter the path to the first file\n";
      std::cin >> aud.MainPath;
      std::cout << "Enter the name of the resulting file\n";
      std::cin >> aud.OutputFilepath;
      int removalSensitivity;
      std::cout << "Enter removal sensitivity\n";
      std::cin >> removalSensitivity;
      aud.removeSilence(removalSensitivity);
      break;
    case 7:
      std::cout << "Enter the path to the first file\n";
      std::cin >> aud.MainPath;
      std::cout << "Enter the name of the resulting file\n";
      std::cin >> aud.OutputFilepath;
      std::cout << "Enter silence start time\n";
      double Start;
      std::cin >> Start;
      double End;
      std::cout << "Enter silence end time\n";
      std::cin >> End;
      aud.applySilence(Start, End);
      break;
    deafult:
      std::cout << "Exit programm";
      choice = 8;
      break;
    }
  }
  return 0;
}