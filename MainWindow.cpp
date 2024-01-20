#include "MainWindow.hpp"
#include "Assets.hpp"
#include "applookandfeel.hpp"
MainWindow::MainWindow(Component &parent) : homeTab(parent), sysTray{parent} {
	loadProgramSettings();

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

	//  License activation panel
	licensePanel.setAlwaysOnTop(true);
	licensePanel.setWantsKeyboardFocus(true);
	licensePanel.onLicenseActivate = [this] {
		licenseCheckScreen.setVisible(true);
	};
	addChildComponent(licensePanel);

	//  Initialise component size
	setSize(width, height);

	//==========================================================================
	// auto waiterLambda = [this] {
	// 	while (!shutdown) {
	// 		std::unique_lock<std::mutex> lock(cv_m);
	// 		using namespace std::chrono_literals;
	// 		cv.wait_for(lock, 100ms,
	// 					[this] { return shutdown || sourceChanged; });

	// 		if (sourceChanged) {
	// 			updateNdiSources();
	// 			sourceChanged = false;
	// 		}
	// 	}
	// };

	updateNdiSources();
	// sourceWaiter = std::async(std::launch::async, waiterLambda);

	//==========================================================================
	// Lock the app if no license is active
	bool isApplicationLicensed = true; // gc->driver->isLicenseActive();
	setApplicationUnlocked(isApplicationLicensed);

	if (!isApplicationLicensed)
		licensePanel.show();

	//==========================================================================
	// Set up driver callbacks
	// gc->driver->onLicenseCheck = [this](std::string message, std::string
	// title, 									bool success) {
	// 	{
	// 		MessageManagerLock msgLock;

	// 		if (success)
	// 			setApplicationUnlocked(true);

	// 		licenseCheckScreen.setVisible(false);
	// 	}

	// 	auto icon = success ? MessageBoxIconType::NoIcon
	// 						: MessageBoxIconType::WarningIcon;

	// 	AlertWindow::showMessageBoxAsync(icon, title, message);
	// };
}

MainWindow::~MainWindow() {
	shutdown = true;
	if (sourceWaiter.valid())
		sourceWaiter.get();
	saveProgramSettings();
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
void MainWindow::loadProgramSettings() {}

void MainWindow::saveProgramSettings() {}
