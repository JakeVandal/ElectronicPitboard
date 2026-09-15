#include "display/segment_driver.h"

#include <cstring>

#include "config.h"

namespace pitboard {

namespace {

constexpr uint8_t kSegA = 0x01;
constexpr uint8_t kSegB = 0x02;
constexpr uint8_t kSegC = 0x04;
constexpr uint8_t kSegD = 0x08;
constexpr uint8_t kSegE = 0x10;
constexpr uint8_t kSegF = 0x20;
constexpr uint8_t kSegG = 0x40;
constexpr uint8_t kSegDP = 0x80;

}  // namespace

SegmentDriver::SegmentDriver() = default;

void SegmentDriver::begin() {
  pinMode(kPinSegClk, OUTPUT);
  pinMode(kPinSegLatch, OUTPUT);
  pinMode(kPinSegData, OUTPUT);
  pinMode(kPinSegOePwm, OUTPUT);

  digitalWrite(kPinSegClk, LOW);
  digitalWrite(kPinSegLatch, LOW);
  analogWrite(kPinSegOePwm, brightness_);
  clear();
  runStartupTest();
}

void SegmentDriver::update(const SegmentLine& line) {
  if (line.text == nullptr) {
    clear();
    return;
  }

  std::array<char, 18> ordered = {};
  std::memcpy(ordered.data(), line.text, std::min<size_t>(line.text[0] ? strlen(line.text) : 0, ordered.size()));
  for (size_t i = 0; i < ordered.size(); ++i) {
    rowBuffer_[0][i] = ordered[i];
  }
  flushRows();
}

void SegmentDriver::clear() {
  for (auto& row : rowBuffer_) {
    row.fill(' ');
  }
  flushRows();
}

void SegmentDriver::setBrightness(uint8_t brightness) {
  brightness_ = brightness;
  analogWrite(kPinSegOePwm, brightness_);
}

void SegmentDriver::runStartupTest() {
  for (uint8_t i = 0; i < 10; ++i) {
    for (auto& row : rowBuffer_) {
      row.fill('8');
    }
    flushRows();
    delay(40);
    clear();
    delay(25);
  }
}

void SegmentDriver::setRow1_LapTime(uint8_t min, uint8_t sec, uint8_t hundredths) {
  char buffer[12] = {0};
  snprintf(buffer, sizeof(buffer), "%02d:%02d.%02d", min, sec, hundredths);
  std::memcpy(rowBuffer_[0].data(), buffer, std::min<size_t>(strlen(buffer), rowBuffer_[0].size()));
  flushRows();
}

void SegmentDriver::setRow2_Position(uint8_t position, uint8_t lap) {
  char buffer[12] = {0};
  snprintf(buffer, sizeof(buffer), "P%02dL%02d", position, lap);
  std::memcpy(rowBuffer_[1].data(), buffer, std::min<size_t>(strlen(buffer), rowBuffer_[1].size()));
  flushRows();
}

void SegmentDriver::setRow3_Gap(float gapSeconds) {
  char buffer[12] = {0};
  snprintf(buffer, sizeof(buffer), "%+05.2f", gapSeconds);
  std::memcpy(rowBuffer_[2].data(), buffer, std::min<size_t>(strlen(buffer), rowBuffer_[2].size()));
  flushRows();
}

void SegmentDriver::setRow3_Message(const char* text) {
  if (text == nullptr) {
    return;
  }
  std::memset(rowBuffer_[2].data(), ' ', rowBuffer_[2].size());
  const size_t len = std::min<size_t>(strlen(text), rowBuffer_[2].size());
  std::memcpy(rowBuffer_[2].data(), text, len);
  flushRows();
}

uint8_t SegmentDriver::encodeCharacter(char ch) {
  switch (ch) {
    case '0': return encodeDigit(0);
    case '1': return encodeDigit(1);
    case '2': return encodeDigit(2);
    case '3': return encodeDigit(3);
    case '4': return encodeDigit(4);
    case '5': return encodeDigit(5);
    case '6': return encodeDigit(6);
    case '7': return encodeDigit(7);
    case '8': return encodeDigit(8);
    case '9': return encodeDigit(9);
    case 'A': case 'a': return kSegA | kSegB | kSegC | kSegE | kSegF | kSegG;
    case 'B': case 'b': return kSegC | kSegD | kSegE | kSegF | kSegG;
    case 'C': case 'c': return kSegA | kSegD | kSegE | kSegF;
    case 'D': case 'd': return kSegB | kSegC | kSegD | kSegE | kSegG;
    case 'E': case 'e': return kSegA | kSegD | kSegE | kSegF | kSegG;
    case 'F': case 'f': return kSegA | kSegE | kSegF | kSegG;
    case 'G': case 'g': return kSegA | kSegB | kSegF | kSegG | kSegD | kSegC;
    case 'H': case 'h': return kSegB | kSegC | kSegE | kSegF | kSegG;
    case 'I': case 'i': return kSegB | kSegC;
    case 'J': case 'j': return kSegB | kSegC | kSegD;
    case 'L': case 'l': return kSegD | kSegE | kSegF;
    case 'N': case 'n': return kSegC | kSegE | kSegG;
    case 'O': case 'o': return kSegC | kSegD | kSegE | kSegG;
    case 'P': case 'p': return kSegA | kSegB | kSegE | kSegF | kSegG;
    case 'R': case 'r': return kSegE | kSegG;
    case 'S': case 's': return kSegA | kSegC | kSegD | kSegF | kSegG;
    case 'T': case 't': return kSegD | kSegE | kSegF | kSegG;
    case 'U': case 'u': return kSegB | kSegC | kSegD | kSegE;
    case 'V': case 'v': return kSegB | kSegC | kSegD | kSegE;
    case 'X': case 'x': return kSegA | kSegC | kSegE | kSegG;
    case 'Y': case 'y': return kSegB | kSegC | kSegD | kSegF | kSegG;
    case 'Z': case 'z': return kSegA | kSegB | kSegD | kSegE | kSegG;
    case '-': return kSegG;
    case '+': return kSegG | kSegF | kSegB;
    case '.': return kSegDP;
    case '_': return kSegD;
    case ' ': return 0;
    default: return 0;
  }
}

uint8_t SegmentDriver::encodeDigit(uint8_t digit) {
  switch (digit) {
    case 0: return kSegA | kSegB | kSegC | kSegD | kSegE | kSegF;
    case 1: return kSegB | kSegC;
    case 2: return kSegA | kSegB | kSegG | kSegE | kSegD;
    case 3: return kSegA | kSegB | kSegG | kSegC | kSegD;
    case 4: return kSegF | kSegG | kSegB | kSegC;
    case 5: return kSegA | kSegF | kSegG | kSegC | kSegD;
    case 6: return kSegA | kSegF | kSegE | kSegD | kSegC | kSegG;
    case 7: return kSegA | kSegB | kSegC;
    case 8: return kSegA | kSegB | kSegC | kSegD | kSegE | kSegF | kSegG;
    case 9: return kSegA | kSegB | kSegC | kSegD | kSegF | kSegG;
    default: return 0;
  }
}

void SegmentDriver::writeShiftRegister(uint8_t value) {
  shiftOut(kPinSegData, kPinSegClk, MSBFIRST, value);
}

void SegmentDriver::flushRows() {
  for (uint8_t rowIndex = 0; rowIndex < 3; ++rowIndex) {
    for (uint8_t digitIdx = 0; digitIdx < 6; ++digitIdx) {
      const char ch = rowBuffer_[rowIndex][digitIdx];
      writeShiftRegister(encodeCharacter(ch));
    }
  }

  digitalWrite(kPinSegLatch, HIGH);
  delayMicroseconds(2);
  digitalWrite(kPinSegLatch, LOW);
}

}  // namespace pitboard
