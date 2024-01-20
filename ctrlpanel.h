#pragma once

class ASIONDIControlPanel {
public:
	static int controlPanel();

private:
	inline static std::condition_variable cv{};
	inline static std::mutex			  cv_m{};
	inline static bool					  sourceNumChanged{false};
};
