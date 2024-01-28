#include <memory>
#include <JuceHeader.h>
#include "settings.hpp"
#include <codecvt>
#include <fstream>

#define ORG_NAME "Avsono"
#define APP_NAME "NDI Virtual Soundcard"

#ifdef _WINDOWS
#include "windows.h"

const char *szPrefsRegKey = "Software\\NDIVS";

const char *szDeviceNDI = "Device NDI";
const char *szDeviceNDIOut = "Device NDI out";
const char *LicisActive = "LicisActive";
const char *szExpiry = "Exp";
const char *licKey = "Lic Key";
#endif // _WINDOWS


template <typename T>
T byteswap(const T &val) {
	constexpr size_t sizeInBytes = sizeof(T);
	uint8_t			 newval[sizeInBytes];
	const uint8_t	*byteRepr = reinterpret_cast<const uint8_t *>(&val);
	for (size_t i = 0; i < sizeInBytes; i++) {
		newval[i] = byteRepr[sizeInBytes - i - 1];
	}
	return *reinterpret_cast<const T *>(newval);
}

template <typename T>
void readValue(std::ifstream &in, T &val) {
	static_assert(std::is_same_v<T, bool> || std::is_same_v<T, std::string>
					  || std::is_same_v<T, long long>,
				  "Unexpected type received");

	uint32_t type_id{};
	uint8_t	 is_null{};
	uint32_t size{};

	const auto goBack = [&val, &in] {
		val = {};
		in.unget();
		in.unget();
		in.unget();
		in.unget();
		std::cerr << "Unexpected type received" << '\n';
	};

	in.read(reinterpret_cast<char *>(&type_id), sizeof(uint32_t));

	if (in.eof()) {
		val = {};
		return;
	}
	type_id = byteswap(type_id);

	switch (type_id) {
	case 1:
		if (!std::is_same_v<T, bool>) {
			return goBack();
		}
		break;
	case 4:
		if (!std::is_same_v<T, long long>) {
			return goBack();
		}
		break;
	case 10:
		if (!std::is_same_v<T, std::string>) {
			return goBack();
		}
		break;
	default:
		return goBack();
	}

	in.read(reinterpret_cast<char *>(&is_null), 1);

	if (type_id == 10 && !is_null) {
		in.read(reinterpret_cast<char *>(&size), sizeof(uint32_t));
		size = byteswap(size);
	}

	if (!is_null) {
		if constexpr (std::is_same_v<T, std::string>) {
			std::vector<char> wsvec(size + 2, '\0');
			in.read(wsvec.data(), size);
			for (int i = 0; i < wsvec.size(); i += 2) {
				std::swap(wsvec[i], wsvec[i + 1]);
			}

			std::wstring temp = (wchar_t *)wsvec.data();
			using convert_type = std::codecvt_utf8<wchar_t>;
			std::wstring_convert<convert_type, wchar_t> converter;

			// use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
			val = converter.to_bytes(temp);
		} else {
			in.read(reinterpret_cast<char *>(&val), sizeof(T));
			val = byteswap(val);
		}
	}
}


template <typename T>
void writeValue(std::ofstream &out, T &val) {
	static_assert(std::is_same_v<T, bool> || std::is_same_v<T, std::wstring>
					  || std::is_same_v<T, long long>,
				  "Unexpected type received");

	uint32_t type_id{};
	uint8_t	 is_null{};
	uint32_t size{};

	if constexpr (std::is_same_v<T, bool>) {
		type_id = 1;
	}
	if constexpr (std::is_same_v<T, std::wstring>) {
		type_id = 10;
		size = val.length() * 2;
	}
	if constexpr (std::is_same_v<T, long long>) {
		type_id = 4;
	}

	type_id = byteswap(type_id);
	char *t1 = reinterpret_cast<char *>(&type_id);
	char  t2[4];
	for (int i = 0; i < 4; i++)
		t2[i] = t1[i];
	out.write(reinterpret_cast<char *>(&type_id), sizeof(uint32_t));
	out.flush();
	out.write(reinterpret_cast<char *>(&is_null), 1);
	out.flush();

	if (!is_null) {
		if constexpr (std::is_same_v<T, std::wstring>) {
			std::vector<char> wsvec(size, '\0');

			memcpy(wsvec.data(), val.data(), size);
			for (int i = 0; i < wsvec.size(); i += 2) {
				std::swap(wsvec[i], wsvec[i + 1]);
			}

			size = byteswap(size);
			out.write(reinterpret_cast<char *>(&size), sizeof(uint32_t));
			out.write(wsvec.data(), wsvec.size());
		} else {
			val = byteswap(val);
			out.write(reinterpret_cast<char *>(&val), sizeof(T));
		}
	}
}


bool ASIONDIConfig::save() {

	auto orgSettingsFolder =
		File::getSpecialLocation(
			File::SpecialLocationType::userApplicationDataDirectory)
			.getChildFile(ORG_NAME);

	if (!orgSettingsFolder.isDirectory())
		orgSettingsFolder.createDirectory();

	auto appSettingsFolder = orgSettingsFolder.getChildFile(APP_NAME);

	if (!appSettingsFolder.isDirectory())
		appSettingsFolder.createDirectory();
	// assert(appSettingsFolder.isDirectory());
	File settingsFile(
		appSettingsFolder.getChildFile("settings.bin").getFullPathName());
	if (settingsFile.exists()) {
		settingsFile.deleteFile();
	}
	std::ofstream settings(appSettingsFolder.getChildFile("settings.bin")
							   .getFullPathName()
							   .toStdString(),
						   std::ios_base::out | std::ios_base::binary
							   | std::ios_base::trunc);

	writeValue(settings, licenseActive);
	writeValue(settings, useLocalAudio);
	writeValue(settings, std::wstring{licenseKey.begin(), licenseKey.end()});
	writeValue(settings, std::wstring{ndiInputDeviceName.begin(),
									  ndiInputDeviceName.end()});
	writeValue(settings, std::wstring{ndiOutputDeviceName.begin(),
									  ndiOutputDeviceName.end()});
	writeValue(settings, std::wstring{expiry.begin(), expiry.end()});
	writeValue(settings, lastOnlineKeyCheckSec);
	long long sr = sampleRate.load();
	writeValue(settings, sr);
	return true;
}

bool ASIONDIConfig::load() {

	auto orgSettingsFolder =
		File::getSpecialLocation(
			File::SpecialLocationType::userApplicationDataDirectory)
			.getChildFile(ORG_NAME);

	if (!orgSettingsFolder.isDirectory())
		orgSettingsFolder.createDirectory();

	auto appSettingsFolder = orgSettingsFolder.getChildFile(APP_NAME);

	if (!appSettingsFolder.isDirectory())
		appSettingsFolder.createDirectory();
	// assert(appSettingsFolder.isDirectory());
	File settingsFile(
		appSettingsFolder.getChildFile("settings.bin").getFullPathName());
	if (!settingsFile.exists())
		return false;

	std::ifstream settings(appSettingsFolder.getChildFile("settings.bin")
							   .getFullPathName()
							   .toStdString());
	readValue(settings, licenseActive);
	readValue(settings, useLocalAudio);
	readValue(settings, licenseKey);
	readValue(settings, ndiInputDeviceName);
	readValue(settings, ndiOutputDeviceName);
	readValue(settings, expiry);
	readValue(settings, lastOnlineKeyCheckSec);
	long long sr{};
	readValue(settings, sr);
	if (sr == 0)
		sr = 44100;
	sampleRate.store(sr);

	return true;
}

#ifdef _WINDOWS
std::string ASIONDIConfig::regGetStringValue(HKEY hKey, LPCSTR lpSubKey) {
	DWORD size = 0;
	RegGetValueA(hKey, NULL, lpSubKey, RRF_RT_REG_SZ, NULL, NULL, &size);

	if (size > 0) {
		std::unique_ptr<byte> s(new byte[size]);
		RegQueryValueExA(hKey, lpSubKey, NULL, NULL, (BYTE *)s.get(), &size);
		return std::string{reinterpret_cast<char *>(s.get())};
	}

	return "";
}
#endif // _WINDOWS
