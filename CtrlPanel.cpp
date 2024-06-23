#include <future>
#include <codecvt>

#include "RestCall.hpp"

#include "Utilities.hpp"
#include "CtrlPanel.hpp"
#include <Processing.NDI.Advanced.h>

void registerMetatypes() {
	// qRegisterMetaTypeStreamOperators<ASIONDIConfig>("ASIONDIConfig");
}

int ASIONDIControlPanel::controlPanel() {
	static bool scanDevices = false;

	registerMetatypes();

	std::string driverDirectoryPath = getDriverDirectoryPath();
	{
		// pDriver->checkLicenseExpired();

		// Init finder
		// Not required, but "correct" (see the SDK documentation.
		if (!NDIlib_initialize())
			return -1;

		// We are going to create an NDI finder that locates sources on the
		// network.
		// pDriver->setNdiFinder(NDIlib_find_create_v2());
		// if (!pDriver->getNdiFinder())
		// 	return -1;

		// Scan devices periodically on a separate thread to keep devices list
		// updated
		// std::thread deviceScanThread([pDriver]() {
		// 	// Allow device scanning by setting control variable
		// 	scanDevices = true;

		// 	// Scan and update devices list every 5 seconds
		// 	while (scanDevices) {

		// 		// Scan devices
		// 		// Get the updated list of sources
		// 		uint32_t			   no_sources = 0;
		// 		const NDIlib_source_t *p_sources = nullptr;

		// 		// Check if scanning is stopped before waiting for devices
		// 		if (!scanDevices)
		// 			break;

		// 		// Wait up till 1 second to check for new sources to be added or
		// 		// removed
		// 		bool sourcesChanged =
		// 			NDIlib_find_wait_for_sources(pDriver->getNdiFinder(), 500);

		// 		if (scanDevices)
		// 			p_sources = NDIlib_find_get_current_sources(
		// 				pDriver->getNdiFinder(), &no_sources);

		// 		// Update sources only if there's a change in the sources list
		// 		if (sourcesChanged) {
		// 			pDriver->getNDIAvailableInputs().clear();

		// 			for (size_t i = 0; i < no_sources; i++) {
		// 				if (pDriver->getNDIOutputName()
		// 					!= p_sources[i].p_ndi_name) {
		// 					pDriver->getNDIAvailableInputs().push_back(
		// 						p_sources[i].p_ndi_name);
		// 				}
		// 			}

		// 			{
		// 				std::lock_guard lock(cv_m);
		// 				sourceNumChanged = true;
		// 			}

		// 			cv.notify_one();
		// 		}

		// 		// Wait for .5 seconds before scanning again
		// 		using namespace std::chrono_literals;
		// 		std::this_thread::sleep_for(500ms);
		// 	}
		// });

		// Detach device scanning thread from parent thread to keep it running
		// deviceScanThread.detach();

		// if (!pDriver->isLicenseActive()) {
		// 	pDriver->stopNdi();
		// }

		// launchJuceGUI(pDriver);

		// Stop device scanning
		scanDevices = false;

		// Wait for scanner thread to complete
		std::this_thread::sleep_for(std::chrono::seconds(1));

		// Finish finder
		// Destroy the NDI finder
		// NDIlib_find_destroy(pDriver->getNdiFinder());

		// Finished
		NDIlib_destroy();
	}

	return 0;
}
