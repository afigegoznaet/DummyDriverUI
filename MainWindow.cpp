#include "MainWindow.hpp"
#include "Assets.hpp"
#include "AppLookAndFeel.hpp"
#include "RestCall.hpp"
#include "Utilities.hpp"

auto epoch_seconds = [] {
	return duration_cast<std::chrono::milliseconds>(
			   std::chrono::system_clock::now().time_since_epoch())
		.count();
};

MainWindow::MainWindow(Component &parent) : homeTab(*this), sysTray{parent} {

	//==========================================================================
	// Set up UI elements
	//  Background image
	backgroundImage = ImageCache::getFromMemory(Assets::background_png,
												Assets::background_pngSize);

	//  Logo
	logo =
		Drawable::createFromImageData(Assets::logo_png, Assets::logo_pngSize);
	logoBtn.setImages(logo.get());
	logoBtn.setEnabled(true);
	addAndMakeVisible(logoBtn);

	//  Title
	titleLabel.setJustificationType(Justification::centred);
	titleLabel.setFont(AppLookAndFeel::titleFontSize);
	addAndMakeVisible(titleLabel);

	//  Tabbed components
	tabSelector.setTabBarDepth(AppLookAndFeel::buttonHeight);
	tabSelector.setOutline(0);
	aboutTab.onLicenseButtonClick = [this] { licensePanel.show(); };

	tabSelector.addTab("Home", Colours::transparentBlack, &homeTab, false,
					   homeTabIndex);
	tabSelector.addTab("About", Colours::transparentBlack, &aboutTab, false,
					   aboutTabIndex);
	addAndMakeVisible(tabSelector);

	//  License check screen
	licenseCheckScreen.setAlwaysOnTop(true);
	licenseCheckScreen.setWantsKeyboardFocus(true);
	addChildComponent(licenseCheckScreen);

	config.load();
	homeTab.onConfigLoad(config.ndiInputDeviceName, config.ndiOutputDeviceName);
	//  License activation panel
	licensePanel.setAlwaysOnTop(true);
	licensePanel.setWantsKeyboardFocus(true);
	licensePanel.onLicenseActivate = [this](auto license) {
		checkLicenseExpired(license);
	};
	licensePanel.onLicenseDeactivate = [this] { deactivate(); };
	licensePanel.setLicense(config.licenseKey);
	addChildComponent(licensePanel);

	//  Initialise component size
	setSize(width, height);


	//==========================================================================
	// Lock the app if no license is active
	bool isApplicationLicensed =
		config.licenseActive; // gc->driver->isLicenseActive();
	setApplicationUnlocked(isApplicationLicensed);

	if (!isApplicationLicensed)
		licensePanel.show();
}

void MainWindow::deactivate() {
	licenseCheckScreen.setVisible(true);
	auto res = deactivate_machine(getMachineUUID());
	licenseCheckScreen.setVisible(true);
	config.licenseActive = false;
	config.expiry = "";
	config.lastOnlineKeyCheckSec = epoch_seconds();
	config.licenseKey = "";
	config.save();
	if (res.errorCode != 0) {
		AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon,
									"Deactivation failed", res.error);
	}

	AlertWindow::showMessageBox(MessageBoxIconType::InfoIcon,
								"Deactivation succeeded", "Driver deactivated");

	licenseCheckScreen.setVisible(false);

	setApplicationUnlocked(false);
}

void MainWindow::checkLicenseExpired(std::string license) {
	licenseCheckScreen.setVisible(true);
	config.licenseActive = false;
	if (config.licenseActive && !(config.expiry.size() == 0)
		&& !isDatePastToday(config.expiry)) {
		config.licenseActive = false;
	}

	if (!config.licenseActive
		|| epoch_seconds() > (config.lastOnlineKeyCheckSec
							  + 60 * 60 * 24 * ONLINE_CHECK_DAYS_INTERVAL)) {
		auto resp = validate_license_key(license, getMachineUUID());
		licenseCheckScreen.setVisible(false);
		if (resp.errorCode != 0) {
			AlertWindow::showMessageBox(MessageBoxIconType::WarningIcon,
										"Activation failed", resp.error);
		}

		AlertWindow::showMessageBox(MessageBoxIconType::InfoIcon,
									"Activation succeeded",
									"Driver successfully activated");

		config.licenseActive = true;
		config.expiry = resp.expiry;
		config.lastOnlineKeyCheckSec = epoch_seconds();
		config.licenseKey = license;
		config.save();
		setApplicationUnlocked(true);
	}
}


MainWindow::~MainWindow() {
	shutdown = true;
	if (sourceWaiter.valid())
		sourceWaiter.get();
	config.save();
	setLookAndFeel(nullptr);
}

//==============================================================================
void MainWindow::resized() {
	const auto padding = AppLookAndFeel::padding;
	const auto titleHeight = AppLookAndFeel::titleHeight;
	const auto buttonHeight = AppLookAndFeel::buttonHeight;
	auto	   bounds = getLocalBounds();

	// License check screen bounds
	licenseCheckScreen.setBounds(bounds);
	licensePanel.setBounds(bounds);

	// Title bounds
	titleLabel.setBounds(bounds.removeFromTop(titleHeight));

	// Tabbed Component bounds
	tabSelector.setBounds(bounds);

	// Logo bounds
	bounds.removeFromBottom(buttonHeight + padding);
	logoBtn.setBounds(bounds.removeFromBottom(buttonHeight)
						  .reduced(padding, 0)
						  .removeFromRight(logoWidth));
}

void MainWindow::paint(Graphics &g) {
	// Draw background image
	g.setOpacity(1.0f); // Make sure the image is drawn opaque
	g.drawImage(backgroundImage, getLocalBounds().toFloat());
}

//==============================================================================
void MainWindow::setApplicationUnlocked(bool shouldBeUnlocked) {
	if (!shouldBeUnlocked)
		tabSelector.setCurrentTabIndex(aboutTabIndex);

	auto *homeButton =
		tabSelector.getTabbedButtonBar().getTabButton(homeTabIndex);
	homeButton->setEnabled(shouldBeUnlocked);

	aboutTab.setApplicationLicensed(shouldBeUnlocked);
}

//==============================================================================
void MainWindow::onAcceptClick(std::string newSource, std::string newOutput) {
	config.ndiInputDeviceName = std::move(newSource);
	config.ndiOutputDeviceName = std::move(newOutput);
	config.save();
}
