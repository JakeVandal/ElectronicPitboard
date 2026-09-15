#include "display/hoysund_tft.h"

#include "config.h"

namespace pitboard {

HoysundTft::HoysundTft() = default;

void HoysundTft::begin() {
  pinMode(kHoysundTftCsPin, OUTPUT);
  pinMode(kHoysundTftDcPin, OUTPUT);
  pinMode(kHoysundTftRstPin, OUTPUT);
  pinMode(kHoysundTouchIrqPin, INPUT);

  digitalWrite(kHoysundTftCsPin, HIGH);
  digitalWrite(kHoysundTftDcPin, HIGH);
  digitalWrite(kHoysundTftRstPin, LOW);
  delay(10);
  digitalWrite(kHoysundTftRstPin, HIGH);
}

void HoysundTft::renderStatus(const char* text) {
  (void)text;
}

void HoysundTft::renderQr(const char* payload) {
  (void)payload;
}

}  // namespace pitboard
