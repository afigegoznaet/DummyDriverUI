#pragma once

#include <string>
#include <chrono>

std::tm parseIsoDate(std::string dateStr);
bool isDatePastToday(std::string date);
std::string getMachineUUID();
std::string getDriverDirectoryPath();
