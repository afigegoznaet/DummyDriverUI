#pragma once

#include <string>
#include <atomic>

struct LocalSettings {
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

	bool operator==(const LocalSettings &o) {
		return o.licenseKey == this->licenseKey;
	}
};
