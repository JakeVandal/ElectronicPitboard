#include "display/segment_driver.h"

#include "config.h"

namespace pitboard {

SegmentDriver::SegmentDriver() = default;

void SegmentDriver::begin() {
  pinMode(kSegmentClkPin, OUTPUT);
  pinMode(kSegmentLatchPin, OUTPUT);
  pinMode(kSegmentDataPin, OUTPUT);
  pinMode(kDisplayBlankPin, OUTPUT);

  digitalWrite(kSegmentClkPin, LOW);
  digitalWrite(kSegmentLatchPin, LOW);
  digitalWrite(kDisplayBlankPin, LOW);
  clear();
}

void SegmentDriver::update(const SegmentLine& line) {
  (void)line;
  // In production this would map ASCII characters to 7-segment patterns and shift them out.
  digitalWrite(kSegmentLatchPin, HIGH);
  delayMicroseconds(2);
  digitalWrite(kSegmentLatchPin, LOW);
}

void SegmentDriver::clear() {
  digitalWrite(kDisplayBlankPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(kDisplayBlankPin, LOW);
}

void SegmentDriver::setBrightness(uint8_t brightness) {
  brightness_ = brightness;
}

}  // namespace pitboard
