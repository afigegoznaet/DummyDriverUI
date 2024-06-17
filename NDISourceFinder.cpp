/*
  ==============================================================================

	NDISourceFinder.cpp
	Created: 11 Feb 2022 11:47:04pm
	Author:  Charles Schiermeyer

  ==============================================================================
*/
#include "NDISourceFinder.hpp"
#include "LicenseConfig.hpp"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <ostream>
#include <thread>


#ifdef _WIN32

#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

std::string getHostname() {
	WORD	wVersionRequested;
	WSADATA wsaData;
	int		err;
	wVersionRequested = MAKEWORD(2, 2);
	char hostname[1024] = {};
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", err);
		return "";
	}
	gethostname(hostname, 1024);
	WSACleanup();
	return hostname;
}

#endif
#ifdef __APPLE__
#include <unistd.h>
static std::string getHostname() {
	char hostname[1024] = {};
	gethostname(hostname, 1024);
	return hostname;
}
#endif

NDISourceFinder::NDISourceFinder() {
	localhost = getHostname();

	NDIlib_find_create_t findCreateSpecs;
	findCreateSpecs.show_local_sources = true;
	findCreateSpecs.p_groups = nullptr;
	ndiFinderInstance_ =
		NDIlib_find_create_v3(&findCreateSpecs, ndi_license_config);

	runnerThread = std::thread{[this]() {
		while (!stopThread) {
			auto x = std::chrono::steady_clock::now()
					 + std::chrono::milliseconds(1000);
			run();
			std::this_thread::sleep_until(x);
		}
	}};
}

NDISourceFinder::~NDISourceFinder() {
	stopThread = true;
	runnerThread.join();
	if (ndiFinderInstance_)
		NDIlib_find_destroy(ndiFinderInstance_);
}

void NDISourceFinder::run() {

	std::vector<std::string> newSourceSet;

	uint32_t				  numSources = 0;
	const NDIlib_source_v2_t *ndiSourcesPtr =
		NDIlib_find_get_current_sources_v2(ndiFinderInstance_, &numSources);

	for (uint32_t i = 0; i < numSources; ++i) {


		// Skip if name not valid string
		if (ndiSourcesPtr[i].p_ndi_name == nullptr
			|| ndiSourcesPtr[i].p_ndi_name[0] == '\0')
			continue;

		std::string sourceName = ndiSourcesPtr[i].p_ndi_name;
		auto		startIdx = sourceName.find('(');
		auto		endIdx = sourceName.find(')');
		if (!(startIdx != std::string::npos && endIdx != std::string::npos))
			continue;
		bool isLocalHost = false;
		auto srcHost = sourceName.substr(0, startIdx - 1);
		std::transform(srcHost.begin(), srcHost.end(), srcHost.begin(),
					   ::toupper);
		if (0 == localhost.compare(srcHost))
			isLocalHost = true;
		auto noHostSourceName =
			sourceName.substr(startIdx + 1, endIdx - startIdx - 1);
		if (!isLocalHost)
			newSourceSet.emplace_back(ndiSourcesPtr[i].p_ndi_name);
	}

	std::sort(newSourceSet.begin(), newSourceSet.end());

	std::cerr << "numSources: " << numSources << std::endl;
	bool equal = std::equal(sources.begin(), sources.end(),
							newSourceSet.begin(), newSourceSet.end());
	if (!equal) {
		std::cerr << "set changed" << std::endl;
		{
			const std::scoped_lock sourceListScopedLock(sourceListMtx_);
			std::swap(sources, newSourceSet);
			if (sourceListUpdatedListener_)
				sourceListUpdatedListener_->sourceListChanged(sources);
			for (auto &src : sources) {
				std::cout << src << std::endl;
			}
		}
	}
}

void NDISourceFinder::setListener(Listener *sourceListUpdatedListener) {
	this->sourceListUpdatedListener_ = sourceListUpdatedListener;
}

void NDISourceFinder::clearCurrentSourcesMap() {
	const std::scoped_lock sourceListScopedLock(sourceListMtx_);
	sources.clear();
	sourceListUpdatedListener_->sourceListChanged(sources);
}
