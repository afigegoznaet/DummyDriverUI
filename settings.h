#pragma once

#include <string>
#include <iostream>
#ifdef _WINDOWS
#include "windows.h"
#endif // _WINDOWS

struct ASIONDIConfig {
	bool			  licenseActive{};
	bool			  useLocalAudio{};
	std::string		  licenseKey;
	std::string		  ndiInputDeviceName;
	std::string		  ndiOutputDeviceName;
	std::string		  expiry;
	long long		  lastOnlineKeyCheckSec;
	std::atomic_llong sampleRate{44100};

	bool save();
	bool load();

	bool operator==(const ASIONDIConfig &o) {
		return o.licenseKey == this->licenseKey;
	}

private:
#ifdef _WINDOWS
	std::string regGetStringValue(HKEY hKey, LPCSTR lpSubKey);
#endif // _WINDOWS
};
