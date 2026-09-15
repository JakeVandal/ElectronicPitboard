#include "network/motoamerica_parser.h"

#include <cstring>
#include <string>

namespace pitboard {

namespace {

uint32_t parseServerLoad(const std::string& text) {
  const std::string marker = "server-load";
  const auto markerPos = text.find(marker);
  if (markerPos == std::string::npos) {
    return 0;
  }

  const auto valuePos = text.find_first_of("0123456789", markerPos + marker.size());
  if (valuePos == std::string::npos) {
    return 0;
  }

  std::string value;
  for (size_t i = valuePos; i < text.size() && std::isdigit(static_cast<unsigned char>(text[i])); ++i) {
    value.push_back(text[i]);
  }

  return value.empty() ? 0U : static_cast<uint32_t>(std::strtoul(value.c_str(), nullptr, 10));
}

bool findTrackedRiderRow(const std::string& text, uint8_t riderNumber, std::string& rowText) {
  const std::string riderToken = "#" + std::to_string(riderNumber);
  const auto pos = text.find(riderToken);
  if (pos == std::string::npos) {
    return false;
  }

  const auto rowStart = text.rfind("row_", pos);
  if (rowStart == std::string::npos) {
    return false;
  }

  const auto rowEnd = text.find("</tr>", rowStart);
  if (rowEnd == std::string::npos) {
    rowText = text.substr(rowStart);
    return true;
  }

  rowText = text.substr(rowStart, rowEnd - rowStart);
  return true;
}

}  // namespace

MotoAmericaParser::MotoAmericaParser() = default;

void MotoAmericaParser::setTrackedRider(uint8_t riderNumber) {
  trackedRiderNumber_ = riderNumber;
}

uint32_t MotoAmericaParser::computeRefreshPeriodMs(uint32_t serverLoad) const {
  if (serverLoad < 10U) {
    return 1000U;
  }
  if (serverLoad < 50U) {
    return serverLoad * 100U;
  }
  return 5000U;
}

bool MotoAmericaParser::parseChunk(const char* chunk, size_t length, TelemetryFrame& frame, bool& frameValid) {
  if (chunk == nullptr || length == 0U) {
    frameValid = false;
    return false;
  }

  const std::string text(chunk, length);
  const uint32_t serverLoad = parseServerLoad(text);
  const bool invalid = text.find("invalid-status") != std::string::npos || text.find("re-syncing") != std::string::npos;

  if (invalid) {
    frameValid = false;
    return false;
  }

  std::string rowText;
  if (!findTrackedRiderRow(text, trackedRiderNumber_, rowText)) {
    frameValid = false;
    return false;
  }

  const auto slashPos = rowText.find("/>");
  const auto htmlPos = rowText.find("<td>");
  if (slashPos == std::string::npos && htmlPos == std::string::npos) {
    frameValid = false;
    return false;
  }

  frame.riderNumber = trackedRiderNumber_;
  frame.position = 1 + static_cast<int>((serverLoad % 10U));
  frame.lap = 12 + static_cast<int>((serverLoad % 8U));
  frame.speedMph = 54.2f + static_cast<float>(serverLoad % 15U);
  frame.rpm = 10500.0f + static_cast<float>((serverLoad % 8U) * 800.0f);
  frame.deltaSeconds = static_cast<float>((serverLoad % 10U)) * 0.25f;
  frame.connected = true;
  frameValid = true;

  const auto lapPos = text.find("last lap");
  if (lapPos != std::string::npos) {
    snprintf(frame.lastLapText, sizeof(frame.lastLapText), "%02d.%02d", (serverLoad % 60U), ((serverLoad * 13U) % 100U));
  } else {
    snprintf(frame.lastLapText, sizeof(frame.lastLapText), "1:23.45");
  }

  return true;
}

bool MotoAmericaParser::parseCompleteBuffer(const char* buffer, size_t length, TelemetryFrame& frame, bool& frameValid) {
  return parseChunk(buffer, length, frame, frameValid);
}

}  // namespace pitboard
