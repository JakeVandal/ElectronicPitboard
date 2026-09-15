#include "display/secondary_screen.h"

#include "config.h"

namespace pitboard {

SecondaryScreen::SecondaryScreen() = default;

void SecondaryScreen::begin() {
  pinMode(kSecondaryLcdTxPin, OUTPUT);
  pinMode(kSecondaryLcdRxPin, INPUT);
  pinMode(kSecondaryLcdResetPin, OUTPUT);
  digitalWrite(kSecondaryLcdResetPin, HIGH);
}

void SecondaryScreen::updateTelemetry(float speedMph, float rpm, int position, int lap) {
  (void)speedMph;
  (void)rpm;
  (void)position;
  (void)lap;
}

}  // namespace pitboard
