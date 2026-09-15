#include <unity.h>

#include "network/motoamerica_parser.h"

extern "C" {
#include <stdint.h>
}

namespace {

const char* kSampleHtml = R"(
<html><body>
<div id="rider_count">3</div>
<table>
<tr id="row_1"><td>#21</td><td>Alex Masuk</td><td>1</td><td>1:23.456</td></tr>
<tr id="row_2"><td>#8</td><td>Rory McLaren</td><td>2</td><td>1:24.111</td></tr>
<tr id="row_3"><td>#44</td><td>Jamie Ross</td><td>3</td><td>1:25.222</td></tr>
</table>
</body></html>)";

}  // namespace

void test_roster_parser_extracts_active_riders() {
  pitboard::MotoAmericaParser parser;
  pitboard::TelemetryFrame frame;
  bool frameValid = false;

  TEST_ASSERT_TRUE(parser.parseCompleteBuffer(kSampleHtml, strlen(kSampleHtml), frame, frameValid));
  TEST_ASSERT_TRUE(frameValid);
  TEST_ASSERT_EQUAL_UINT8(3U, parser.activeRosterCount());
  TEST_ASSERT_EQUAL_UINT8(21U, parser.activeRoster()[0].bikeNumber);
  TEST_ASSERT_EQUAL_STRING("Alex Masuk", parser.activeRoster()[0].riderName);
  TEST_ASSERT_EQUAL_UINT8(8U, parser.activeRoster()[1].bikeNumber);
  TEST_ASSERT_EQUAL_STRING("Rory McLaren", parser.activeRoster()[1].riderName);
}

void test_tracked_rider_can_be_set_directly() {
  pitboard::MotoAmericaParser parser;

  parser.setTrackedRider(44);
  TEST_ASSERT_EQUAL_UINT8(44U, parser.trackedRiderNumber());
}

int main(int argc, char** argv) {
  UNITY_BEGIN();
  RUN_TEST(test_roster_parser_extracts_active_riders);
  RUN_TEST(test_tracked_rider_can_be_set_directly);
  return UNITY_END();
}
