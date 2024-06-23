#ifdef _WINDOWS
#include <memory>
#include "windows.h"
#endif // _WINDOWS

#include <JuceHeader.h>
#include "Utilities.hpp"
using namespace std::chrono_literals;

// https://stackoverflow.com/questions/26895428/how-do-i-parse-an-iso-8601-date-with-optional-milliseconds-to-a-struct-tm-in-c
std::tm parseIsoDate(std::string dateStr) {
	int	  y, M, d, h, m;
	float s;
	int	  tzh = 0, tzm = 0;

	if (6 < sscanf(dateStr.c_str(), "%d-%d-%dT%d:%d:%f%d:%dZ", &y, &M, &d, &h,
				   &m, &s, &tzh, &tzm)) {
		if (tzh < 0) {
			tzm = -tzm; // Fix the sign on minutes.
		}
	}

	std::tm time = {0};
	time.tm_year = y - 1900; // Year since 1900
	time.tm_mon = M - 1;	 // 0-11
	time.tm_mday = d;		 // 1-31
	time.tm_hour = h;		 // 0-23
	time.tm_min = m;		 // 0-59
	time.tm_sec = (int)s;	 // 0-61 (0-60 in C++11)

	return time;
}

bool isDatePastToday(std::string date) {
	std::tm t = parseIsoDate(date);

	auto now = std::chrono::time_point_cast<std::chrono::seconds>(
		std::chrono::system_clock::now() + 21600s);
	auto expiryTime = std::chrono::time_point_cast<std::chrono::seconds>(
		std::chrono::system_clock::from_time_t(mktime(&t)));

	return !(now >= expiryTime);
}

std::string getMachineUUID() {
	return OnlineUnlockStatus::MachineIDUtilities::getUniqueMachineID()
		.toStdString();
}

std::string getDriverDirectoryPath() {
#ifdef _WINDOWS
	char	path[MAX_PATH];
	HMODULE hm = NULL;

	if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
							  | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
						  (LPCSTR)&getDriverDirectoryPath, &hm)
		== 0) {
		int ret = GetLastError();
		return "";
	}
	if (GetModuleFileName(hm, path, sizeof(path)) == 0) {
		int ret = GetLastError();
		return "";
	}

	std::string dllPath{path};
	size_t		lastSeparatorPos = dllPath.find_last_of("\\");
	dllPath.erase(lastSeparatorPos, dllPath.size() - lastSeparatorPos);

	return dllPath;
#else
	return "";
#endif // _WINDOWS
}
