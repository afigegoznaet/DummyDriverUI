/*
  ==============================================================================

	NDISourceFinder.h
	Created: 11 Feb 2022 11:47:04pm
	Author:  Charles Schiermeyer

  ==============================================================================
*/

#pragma once
#include <mutex>
#include <vector>
#include <thread>
#ifdef LINUX
#include <stddef.h>
#include <unordered_map>
#endif

#include <Processing.NDI.Advanced.h>

struct NDISourceFinder {
	NDISourceFinder();
	~NDISourceFinder();
	void run();

	struct Listener {
		virtual ~Listener() {}
		virtual void
		sourceListChanged(std::vector<std::string> updatedSourcesMap) = 0;
	};

	void setListener(Listener *sourceListUpdatedListener);
	void clearCurrentSourcesMap();

private:
	std::vector<std::string> sources{};
	NDIlib_find_instance_t	 ndiFinderInstance_ = nullptr;

	std::string localhost{};

	Listener * sourceListUpdatedListener_{};
	std::mutex sourceListMtx_;

	std::atomic_bool stopThread{};

	std::thread runnerThread;
};
