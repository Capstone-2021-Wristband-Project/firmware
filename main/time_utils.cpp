#include "time_utils.h"

#include <algorithm>
#include <sys/time.h>

int monthAbbreviationToNumber(std::string abbreviation) {
    // convert to lower case
    std::for_each(abbreviation.begin(), abbreviation.end(), [](char & c){
        c = ::tolower(c);
    });

    // convert to number
    if (abbreviation == "jan") {
        return 0;
    } else if (abbreviation == "feb") {
        return 1;
    } else if (abbreviation == "mar") {
        return 2;
    } else if (abbreviation == "apr") {
        return 3;
    } else if (abbreviation == "may") {
        return 4;
    } else if (abbreviation == "jun") {
        return 5;
    } else if (abbreviation == "jul") {
        return 6;
    } else if (abbreviation == "aug") {
        return 7;
    } else if (abbreviation == "sep") {
        return 8;
    } else if (abbreviation == "oct") {
        return 9;
    } else if (abbreviation == "nov") {
        return 10;
    } else {
        return 11;
    }
}

time_t getEpochFromTimeStrings(const std::string &dateStr, const std::string &timeStr){
    tm timeStruct;
    // date
    std::string monthStr = dateStr.substr(0, 3);
    timeStruct.tm_mon = monthAbbreviationToNumber(monthStr);
    timeStruct.tm_mday = std::stoi(dateStr.substr(4, 2));
    timeStruct.tm_year = std::stoi(dateStr.substr(7, 4)) - 1900;

    // time of day
    timeStruct.tm_hour = std::stoi(timeStr.substr(0, 2));
    timeStruct.tm_min = std::stoi(timeStr.substr(3, 2));
    timeStruct.tm_sec = std::stoi(timeStr.substr(6, 2));

    return mktime(&timeStruct);
}

void setTimeFromTimeStrings(const std::string &dateStr, const std::string &timeStr){
    time_t secondsSinceEpoch = getEpochFromTimeStrings(dateStr, timeStr);
    timeval tv;
    tv.tv_sec = secondsSinceEpoch;
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);
}