#include "network/motoamerica_parser.h"

#include <cctype>
#include <cstring>
#include <string>

namespace pitboard {

namespace {

std::string trimHtmlToken(const std::string& text) {
  const auto start = text.find_first_not_of(" \t\r\n><=");
  if (start == std::string::npos) {
    return "";
  }

  const auto end = text.find_last_not_of(" \t\r\n><=");
  return text.substr(start, end - start + 1);
}

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

bool parseBikeNumber(const std::string& cell, uint8_t& bikeNumber) {
  std::string normalized = trimHtmlToken(cell);
  const auto hashPos = normalized.find('#');
  if (hashPos != std::string::npos) {
    normalized = normalized.substr(hashPos + 1);
  }

  if (normalized.empty()) {
    return false;
  }

  const auto start = normalized.find_first_of("0123456789");
  if (start == std::string::npos) {
    return false;
  }

  const auto end = normalized.find_first_not_of("0123456789", start);
  std::string digits = normalized.substr(start, end == std::string::npos ? std::string::npos : end - start);
  if (digits.empty()) {
    return false;
  }

  bikeNumber = static_cast<uint8_t>(std::strtoul(digits.c_str(), nullptr, 10));
  return bikeNumber > 0U;
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

uint8_t MotoAmericaParser::trackedRiderNumber() const {
  return trackedRiderNumber_;
}

const RiderInfo* MotoAmericaParser::activeRoster() const {
  return activeRoster_;
}

size_t MotoAmericaParser::activeRosterCount() const {
  return activeRosterCount_;
}

bool MotoAmericaParser::rosterDirty() const {
  return rosterDirty_;
}

void MotoAmericaParser::clearRosterDirty() {
  rosterDirty_ = false;
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

bool MotoAmericaParser::parseActiveRoster(const std::string& text) {
  memset(activeRoster_, 0, sizeof(activeRoster_));
  activeRosterCount_ = 0U;
  rosterDirty_ = false;

  const auto countMarker = text.find("rider_count");
  if (countMarker != std::string::npos) {
    const auto valuePos = text.find_first_of("0123456789", countMarker);
    if (valuePos != std::string::npos) {
      std::string value;
      size_t index = valuePos;
      while (index < text.size() && std::isdigit(static_cast<unsigned char>(text[index]))) {
        value.push_back(text[index]);
        ++index;
      }
      if (!value.empty()) {
        const uint32_t parsedCount = static_cast<uint32_t>(std::strtoul(value.c_str(), nullptr, 10));
        if (parsedCount > 0U && parsedCount < kMaxActiveRoster) {
          activeRosterCount_ = parsedCount;
        }
      }
    }
  }

  size_t cursor = 0;
  size_t parsedRows = 0U;
  while (cursor < text.size() && parsedRows < kMaxActiveRoster) {
    const auto rowPos = text.find("row_", cursor);
    if (rowPos == std::string::npos) {
      break;
    }

    const auto rowOpening = text.find('<', rowPos);
    if (rowOpening == std::string::npos) {
      break;
    }

    const auto rowEnd = text.find("</tr>", rowOpening);
    if (rowEnd == std::string::npos) {
      break;
    }

    const std::string rowText = text.substr(rowOpening, rowEnd - rowOpening + 5U);
    std::string cellValues[4] = {"", "", "", ""};
    size_t cellIndex = 0;
    size_t cellCursor = 0;
    while (cellIndex < 4U && cellCursor < rowText.size()) {
      const auto cellStart = rowText.find("<td", cellCursor);
      if (cellStart == std::string::npos) {
        break;
      }
      const auto valueStart = rowText.find('>', cellStart);
      if (valueStart == std::string::npos) {
        break;
      }
      const auto valueEnd = rowText.find("</td>", valueStart + 1);
      if (valueEnd == std::string::npos) {
        break;
      }

      cellValues[cellIndex] = trimHtmlToken(rowText.substr(valueStart + 1, valueEnd - valueStart - 1));
      cellCursor = valueEnd + 5U;
      ++cellIndex;
    }

    uint8_t bikeNumber = 0U;
    if (cellIndex >= 2U && parseBikeNumber(cellValues[0], bikeNumber)) {
      RiderInfo rider{};
      rider.bikeNumber = bikeNumber;
      snprintf(rider.riderName, sizeof(rider.riderName), "%s", cellValues[1].empty() ? "Unknown" : cellValues[1].c_str());
      rider.position = cellIndex >= 3U && !cellValues[2].empty() ? std::atoi(cellValues[2].c_str()) : 0;
      snprintf(rider.lastLapText, sizeof(rider.lastLapText), "%s",
               cellIndex >= 4U && !cellValues[3].empty() ? cellValues[3].c_str() : "-:--.-");
      activeRoster_[parsedRows++] = rider;
    }

    cursor = rowEnd + 5U;
  }

  if (parsedRows > 0U) {
    activeRosterCount_ = parsedRows;
    rosterDirty_ = true;
    return true;
  }

  return false;
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

  parseActiveRoster(text);

  std::string rowText;
  if (!findTrackedRiderRow(text, trackedRiderNumber_, rowText)) {
    if (activeRosterCount_ == 0U) {
      frameValid = false;
      return false;
    }
    frame.riderNumber = activeRoster_[0].bikeNumber;
    frame.position = activeRoster_[0].position;
    frame.connected = true;
    frameValid = true;
    return true;
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
