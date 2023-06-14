/**
 * Заголовочный файл, в котором хранятся все структуры, методы и нужные
 * библиотеки.
 */
#include <fstream>
#include <iostream>
#include <vector>
/**
 * \brief Представляет собой структуру .wav файла.
 *
 * Создан для упрощения работы с данными файлами.
 * Наглядно иллюстрирует структуру .wav файлов.
 */
struct WavHeader { /// Содержит символы «RIFF» в ASCII кодировке
  char chunkID[4];
  /// Это оставшийся размер цепочки, начиная с этой позиции. Иначе говоря, это
  /// размер файла минус 8, то есть, исключены поля chunkId и chunkSize.
  unsigned long chunkSize; /// Содержит символы «WAVE»
  char format[4];          /// Содержит символы "fmt "
  char subchunk1ID[4]; /// Это оставшийся размер подцепочки, начиная с этой
                       /// позиции.
  unsigned long subchunk1Size; /// Аудио формат (формат сжатия)
  unsigned short audioFormat; /// Количество каналов
  unsigned short numChannels; /// Частота дискретизации
  unsigned int
      sampleRate; /// Количество байт, переданных за секунду воспроизведения.
  unsigned int
      byteRate; /// Количество байт для одного сэмпла, включая все каналы.
  unsigned short blockAlign;    /// Количество бит в сэмпле
  unsigned short bitsPerSample; /// Содержит символы «data»
  char subchunk2ID[4]; /// Количество байт в области данных
  unsigned int subchunk2Size;
};
/**
 * \brief Представляет собой структуру данных аудиофайла.
 */
struct AudioFile {      /// Путь основого файла
  std::string MainPath; /// Путь получаемого файла.
  std::string
      OutputFilepath; /// Путь второго файла, если он требуется для функции.
  std::string SecondFilePath;
  /** Проверка на правильность значений времени.
   * \param start, end Значения начала и конца времени. Должны быть
   * неотрицательными. Конец времени должен быть больше начала времени. \return
   * Ничего, но если значения неправильные, то выбрасывает ошибку.
   */
  void CHECK_TIME(double start, double end) {
    if (start < 0 || end < 0 || start > end) {
      throw std::invalid_argument{"Enter is not correct"};
    }
  }
  /*! Проверка на правильность формата файла.
   * \param  header тип данных WavHeader. В нём проверяется соотвествие первым
   * двум полям RIFF и WAVE соответственно. \return Ничего, но если поля
   * содержат неправильные данные, то выбрасывает ошибку.
   */
  void CHECK_FORMAT(WavHeader header) {
    if (std::string(header.chunkID, 4) != "RIFF" ||
        std::string(header.format, 4) != "WAVE") {
      throw std::runtime_error("Error format file");
    }
  }
  /** Проверка на правильное открытие файла.
   * \param file Открытый файл.
   * \return Ничего, но если файл не получилось открыть, то выбрасывает ошибку.
   */
  void CHECK_OPEN_FILE(std::ifstream &file) {
    if (!file) {
      throw std::runtime_error("Error with open file");
    }
  }
  /** Слияние двух дорожек.
   * \param Ничего не принимает, но использует 3 пути к файлам. 2 - источники,
   * из которых берётся звук. 1 - Файл, получаемый при работе метода.
   */
  void mergeWavFiles() {
    std::string file1 = this->MainPath;
    std::string outputFile = this->OutputFilepath;
    std::string file2 = this->SecondFilePath;
    std::ifstream source1(file1, std::ios::binary);
    std::ifstream source2(file2, std::ios::binary);
    std::ofstream output(outputFile, std::ios::binary);
    CHECK_OPEN_FILE(source1);
    CHECK_OPEN_FILE(source2);
    WavHeader header1, header2;
    source1.read(reinterpret_cast<char *>(&header1), sizeof(WavHeader));
    source2.read(reinterpret_cast<char *>(&header2), sizeof(WavHeader));
    CHECK_FORMAT(header1);
    CHECK_FORMAT(header2);
    unsigned int totalSamples1 =
        header1.subchunk2Size / (header1.bitsPerSample / 8);
    unsigned int totalSamples2 =
        header2.subchunk2Size / (header2.bitsPerSample / 8);
    unsigned int totalSamplesCombined = totalSamples1 + totalSamples2;

    header1.chunkSize += totalSamples2 * (header1.bitsPerSample / 8);
    header1.subchunk2Size += totalSamples2 * (header1.bitsPerSample / 8);
    output.write(reinterpret_cast<char *>(&header1), sizeof(WavHeader));

    std::vector<char> buffer(totalSamples1 * (header1.bitsPerSample / 8));
    source1.read(buffer.data(), buffer.size());
    output.write(buffer.data(), buffer.size());

    buffer.resize(totalSamples2 * (header2.bitsPerSample / 8));
    source2.read(buffer.data(), buffer.size());
    output.write(buffer.data(), buffer.size());
  }
  /** Выборочное звучание файла.
   * \param startTime, endTime - Значения начала и конца времени. Метод создаёт
   * файл, который воспроизводит аудио с начала времени до конца времени.
   * Остальное не воспроизводится.
   */
  void CutAudio(float startTime, float endTime) {
    std::string sourceFile = this->MainPath;
    std::string outputFile = this->OutputFilepath;
    std::ifstream source(sourceFile, std::ios::binary);
    std::ofstream output(outputFile, std::ios::binary);
    CHECK_OPEN_FILE(source);
    CHECK_TIME(startTime, endTime);
    WavHeader header;
    source.read(reinterpret_cast<char *>(&header), 44);
    CHECK_FORMAT(header);
    float audioDuration = header.subchunk2Size / header.byteRate;
    if (endTime > audioDuration) {
      endTime = audioDuration;
    }
    unsigned int startByte =
        static_cast<unsigned int>(startTime * header.byteRate) + 44;
    unsigned int endByte =
        static_cast<unsigned int>(endTime * header.byteRate) + 44;
    output.write(reinterpret_cast<char *>(&header), 44);
    long int countbytes = 0;
    unsigned int remainingBytes = startByte - 44;
    std::vector<char> buffer(remainingBytes);
    source.read(buffer.data(), remainingBytes);
    output.write(buffer.data(), remainingBytes);
    countbytes += remainingBytes;
    source.seekg(endByte);
    remainingBytes = (audioDuration - endTime) * header.byteRate - 44;
    countbytes += remainingBytes;
    buffer.resize(remainingBytes);
    source.read(buffer.data(), remainingBytes);
    output.write(buffer.data(), remainingBytes);
    header.chunkSize = 44 + countbytes;
    header.subchunk2Size = header.chunkSize - 44;
    output.seekp(0);
    output.write(reinterpret_cast<char *>(&header), sizeof(WavHeader));
    source.close();
    output.close();
  }
  /** Ускорение или замедление воспроизведения.
   * \param speedMultiplier Множитель ускорения воспроизведения звука. Если
   * значение переменной < 1, то скорость воспроизведение замедляется, если > 1,
   * то скорость воспроизведения увеличивается.
   */
  void speedUpAudioPlayback(float speedMultiplier) {
    std::string sourceFile = this->MainPath;
    std::string outputFile = this->OutputFilepath;
    std::ifstream source(sourceFile, std::ios::binary);
    std::ofstream output(outputFile, std::ios::binary);
    CHECK_OPEN_FILE(source);
    WavHeader header;
    source.read(reinterpret_cast<char *>(&header), sizeof(WavHeader));
    CHECK_FORMAT(header);
    if (speedMultiplier < 0) {
      throw std::invalid_argument{"Multiplier enter is not correct"};
    }
    unsigned int originalSampleRate = header.sampleRate;
    unsigned int newSampleRate =
        static_cast<unsigned int>(originalSampleRate * speedMultiplier);
    header.sampleRate = newSampleRate;
    header.byteRate = header.byteRate * newSampleRate / originalSampleRate;
    unsigned int originalSubchunk2Size = header.subchunk2Size;
    unsigned int newSubchunk2Size =
        static_cast<unsigned int>(originalSubchunk2Size / speedMultiplier);
    header.chunkSize = newSubchunk2Size + sizeof(WavHeader) - 8;
    header.subchunk2Size = 2 * newSubchunk2Size;
    output.write(reinterpret_cast<const char *>(&header), sizeof(WavHeader));
    output << source.rdbuf();
    source.close();
    output.close();
  }
  /** Воспроизведение аудио в обратном режиме.
   * Метод создаёт новый файл, в котором звук идёт задом-наперёд.
   */
  void reverseAudio() {
    std::string sourceFile = this->MainPath;
    std::string outputFile = this->OutputFilepath;
    std::ifstream source(sourceFile, std::ios::binary);
    std::ofstream output(outputFile, std::ios::binary);
    CHECK_OPEN_FILE(source);
    WavHeader header;
    source.read(reinterpret_cast<char *>(&header), sizeof(WavHeader));
    CHECK_FORMAT(header);
    output.write(reinterpret_cast<char *>(&header), sizeof(WavHeader));
    const int dataSize = header.subchunk2Size;
    std::vector<char> data(dataSize);
    source.read(data.data(), dataSize);
    for (int i = dataSize - 2; i >= 0; i -= 2) {
      output.write(&data[i], 2);
    }
  }
  /** Накладывание тишины на промежуток.
   * \param startTime, endTime Значения начала и конца времени. Метод создаёт
   * файл, в котором на данном промежутки наложена тишина.
   */
  void applySilence(double startTime, double endTime) {
    std::string sourceFile = this->MainPath;
    std::string outputFile = this->OutputFilepath;
    std::ifstream source(sourceFile, std::ios::binary);
    std::ofstream output(outputFile, std::ios::binary);
    CHECK_OPEN_FILE(source);
    CHECK_TIME(startTime, endTime);
    WavHeader header;
    source.read(reinterpret_cast<char *>(&header), sizeof(WavHeader));
    CHECK_FORMAT(header);
    output.write(reinterpret_cast<char *>(&header), sizeof(WavHeader));
    unsigned int audioDuration = header.subchunk2Size / header.byteRate;
    if (endTime > audioDuration) {
      endTime = audioDuration;
    }
    unsigned int startByte =
        static_cast<unsigned int>(startTime * header.byteRate) + 44;
    unsigned int endByte =
        static_cast<unsigned int>(endTime * header.byteRate) + 44;
    output.write(reinterpret_cast<char *>(&header), 44);
    long int countbytes = 0;
    unsigned int remainingBytes = startByte - 44;
    std::vector<char> buffer(remainingBytes);
    source.read(buffer.data(), remainingBytes);
    output.write(buffer.data(), remainingBytes);
    countbytes += remainingBytes;
    unsigned int bytesToSilence = endByte - startByte;
    for (unsigned int i = 0; i < bytesToSilence; i++) {
      char zeroByte = 0;
      output.write(&zeroByte, sizeof(char));
    }
    buffer.resize(header.subchunk2Size - endByte);
    source.read(buffer.data(), header.subchunk2Size - endByte);
    output.write(buffer.data(), header.subchunk2Size - endByte);
    source.close();
    output.close();
  }
  /** Накладывание двух дорожек на одну.
   * Метод создаёт файл с наложенными двумя дорожками из 2 файлов. Получается
   * минимальная длительность из двух файлов.
   */
  void mixWavFiles() {
    std::string sourceFile1 = this->MainPath;
    std::string outputFile = this->OutputFilepath;
    std::string sourceFile2 = this->SecondFilePath;
    std::ifstream source1(sourceFile1, std::ios::binary);
    std::ifstream source2(sourceFile2, std::ios::binary);
    std::ofstream output(outputFile, std::ios::binary);
    CHECK_OPEN_FILE(source1);
    CHECK_OPEN_FILE(source2);
    WavHeader header1, header2;
    source1.read(reinterpret_cast<char *>(&header1), sizeof(WavHeader));
    source2.read(reinterpret_cast<char *>(&header2), sizeof(WavHeader));
    CHECK_FORMAT(header1);
    CHECK_FORMAT(header2);
    unsigned int maxSamples = std::max(header1.sampleRate, header2.sampleRate);
    header1.sampleRate = header2.sampleRate = maxSamples;
    header1.byteRate = header2.byteRate + header1.byteRate;
    header1.subchunk2Size = header2.subchunk2Size + header1.subchunk2Size;
    output.write(reinterpret_cast<const char *>(&header1), sizeof(WavHeader));
    char buffer1[1];
    char buffer2[1];
    while (!source1.eof() && !source2.eof()) {
      source1.read(buffer1, 1);
      source2.read(buffer2, 1);
      const unsigned int bytesRead1 = source1.gcount();
      const unsigned int bytesRead2 = source2.gcount();
      unsigned int bytesToWrite{0};
      if (bytesRead1 < bytesRead2) {
        bytesToWrite = bytesRead1;
      } else {
        bytesToWrite = bytesRead2;
      }
      const float sample1 = static_cast<float>(buffer1[0]);
      const float sample2 = static_cast<float>(buffer2[0]);
      const float mixedSample = sample1 + sample2;
      buffer1[0] = static_cast<char>(mixedSample);
      output.write(buffer1, bytesToWrite);
    }
    source1.close();
    source2.close();
    output.close();
  }
  /** Удаление тишины.
  \param removalSensitivity Значение чувствительности к удалению тишины.
  Программа ищет промежутки из нулей данной длины, и если все они состоят из
  нулей, то удаляет её.
  */
  void removeSilence(int removalSensitivity) {
    std::string sourceFile = this->MainPath;
    std::string outputFile = this->OutputFilepath;
    std::ifstream source(sourceFile, std::ios::binary);
    std::ofstream output(outputFile, std::ios::binary);
    WavHeader header;
    source.read(reinterpret_cast<char *>(&header), sizeof(WavHeader));
    CHECK_OPEN_FILE(source);
    CHECK_FORMAT(header);
    output.write(reinterpret_cast<const char *>(&header), sizeof(WavHeader));
    char buffer[removalSensitivity];
    while (source.read(buffer, removalSensitivity)) {
      for (int i = 0; i < 5; i++) {
        short sample = (buffer[2 * i + 1] << 8) | buffer[2 * i];
        if (sample != 0) {
          output.write(buffer, removalSensitivity);
          header.subchunk2Size -= removalSensitivity;
          break;
        }
      }
    }
    source.close();
    output.close();
  }
};