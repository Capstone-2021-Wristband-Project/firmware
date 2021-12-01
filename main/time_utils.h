#pragma once

#include <string>
#include <ctime>

int monthAbbreviationToNumber(std::string abbreviation);
time_t getEpochFromTimeStrings(const std::string &dateStr, const std::string &timeStr);
void setTimeFromTimeStrings(const std::string &dateStr, const std::string &timeStr);