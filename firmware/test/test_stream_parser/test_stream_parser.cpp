#include <unity.h>

extern "C" {
#include <stdint.h>
}

void test_placeholder_parser() {
  TEST_ASSERT_TRUE(true);
}

int main(int argc, char** argv) {
  UNITY_BEGIN();
  RUN_TEST(test_placeholder_parser);
  return UNITY_END();
}
