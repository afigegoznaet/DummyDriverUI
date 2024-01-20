#include "ctrlpanelcomponent.hpp"
#include "Assets.hpp"
#include "applookandfeel.hpp"

//==============================================================================
LogoComponent::LogoComponent() {
	// Load logo image
	logoImage =
		ImageCache::getFromMemory(Assets::logo_png, Assets::logo_pngSize);
}

void LogoComponent::paint(Graphics &g) {
	g.drawImage(logoImage, getLocalBounds().toFloat(),
				RectanglePlacement::xRight);
}

//==============================================================================


LicensePanel::LicensePanel() {
	//==========================================================================
	// Set up UI elements
	//  Title
	panelTitleLabel.setJustificationType(Justification::centred);
	panelTitleLabel.setFont(AppLookAndFeel::titleFontSize);
	addAndMakeVisible(panelTitleLabel);

	//  License editor
	licenseEditor.setMultiLine(false, false);
	licenseEditor.setJustification(Justification::centred);
	licenseEditor.setIndents(0, 0);
	licenseEditor.setFont(AppLookAndFeel::defaultFontSize);
	licenseEditor.setTextToShowWhenEmpty("Enter your license key",
										 juce::Colours::grey);
	addAndMakeVisible(licenseEditor);

	//  Copyright notice
	copyrightInfo.setJustificationType(Justification::centred);
	copyrightInfo.setColour(Label::textColourId, juce::Colours::grey);
	copyrightInfo.setFont(AppLookAndFeel::infoFontSize);
	addAndMakeVisible(copyrightInfo);

	//==========================================================================
	// Set up buttons
	//  Activate button
	activateButton.addListener(this);
	addAndMakeVisible(activateButton);

	//  Dectivate button
	deactivateButton.addListener(this);
	addAndMakeVisible(deactivateButton);

	//  Cancel button
	cancelButton.addListener(this);
	addAndMakeVisible(cancelButton);
}

void LicensePanel::resized() {
	const auto width = AppLookAndFeel::panelWidth;
	const auto height = AppLookAndFeel::panelHeight;
	const auto padding = AppLookAndFeel::padding;
	const auto titleHeight = AppLookAndFeel::titleHeight;
	const auto buttonWidth = AppLookAndFeel::buttonWidth;
	const auto buttonHeight = AppLookAndFeel::buttonHeight;

	auto bounds = getLocalBounds().withSizeKeepingCentre(width, height);

	// Position cancel button
	cancelButton.setBoundsRelative(0.83f, 0.24f, 0.04f, 0.07f);


	// Position title
	panelTitleLabel.setBounds(bounds.removeFromTop(titleHeight));


	// Position license editor
	licenseEditor.setBounds(
		bounds.removeFromTop(buttonHeight)
			.withSizeKeepingCentre(width - 4 * padding, buttonHeight));

	// Position copyright info
	copyrightInfo.setBounds(bounds.removeFromTop(buttonHeight));

	// Position buttons
	bounds.removeFromBottom(padding);
	auto buttonBounds =
		bounds.removeFromBottom(buttonHeight)
			.withSizeKeepingCentre(2 * buttonWidth + padding, buttonHeight);
	activateButton.setBounds(buttonBounds.removeFromLeft(buttonWidth));
	deactivateButton.setBounds(buttonBounds.removeFromRight(buttonWidth));
}

void LicensePanel::paint(Graphics &g) {
	const auto width = AppLookAndFeel::panelWidth;
	const auto height = AppLookAndFeel::panelHeight;
	const auto bounds = getLocalBounds().withSizeKeepingCentre(width, height);
	AppLookAndFeel::drawCustomPanelBackground(g, bounds);
}

void LicensePanel::buttonClicked(Button *button) {
	if (button == &activateButton) {
		auto input = licenseEditor.getText().toUpperCase();

		if (onLicenseActivate != nullptr)
			onLicenseActivate();

		setVisible(false);
		// driver->onActivateClick(input.toStdString());
	}

	if (button == &deactivateButton) {
		auto input = licenseEditor.getText().toUpperCase();

		if (onLicenseActivate != nullptr)
			onLicenseActivate();

		setVisible(false);
		// driver->onDeactivateClick();
	} else if (button == &cancelButton) {
		setVisible(false);
	}
}

void LicensePanel::show() {
	// Pre-fill license editor if there is an active license
	// if (driver->isLicenseActive())
	// 	licenseEditor.setText(driver->getLicenseKey());
	// else
	// 	licenseEditor.clear();

	setVisible(true);
}

//==============================================================================
LicenseCheckComponent::LicenseCheckComponent() {
	// Set up status label
	status.setJustificationType(Justification::centred);
	status.setFont(AppLookAndFeel::titleFontSize);
	addAndMakeVisible(status);
}

void LicenseCheckComponent::resized() { status.setBounds(getLocalBounds()); }

void LicenseCheckComponent::paint(Graphics &g) {
	auto width = AppLookAndFeel::panelWidth;
	auto height = AppLookAndFeel::panelHeight;
	auto bounds = getLocalBounds().withSizeKeepingCentre(width, height);
	AppLookAndFeel::drawCustomPanelBackground(g, bounds);
}

//==============================================================================

CtrlPanelComponent::CtrlPanelComponent() {
	loadProgramSettings();

	//==========================================================================
	// Set up UI elements
	//  Background image
	backgroundImage = ImageCache::getFromMemory(Assets::background_png,
												Assets::background_pngSize);

	//  Logo
	addAndMakeVisible(logo);

	//  Title
	titleLabel.setJustificationType(Justification::centred);
	titleLabel.setFont(AppLookAndFeel::titleFontSize);
	addAndMakeVisible(titleLabel);

	//  Tabbed components
	tabSelector.setTabBarDepth(AppLookAndFeel::buttonHeight);
	tabSelector.setOutline(0);
	homeTab = std::make_unique<HomeTab>();
	aboutTab = std::make_unique<AboutTab>();
	aboutTab->onLicenseButtonClick = [this] { licensePanel.show(); };

	tabSelector.addTab("Home", Colours::transparentBlack, homeTab.get(), false,
					   homeTabIndex);
	tabSelector.addTab("About", Colours::transparentBlack, aboutTab.get(),
					   false, aboutTabIndex);
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
	auto waiterLambda = [this] {
		while (!shutdown) {
			std::unique_lock<std::mutex> lock(cv_m);
			using namespace std::chrono_literals;
			cv.wait_for(lock, 100ms,
						[this] { return shutdown || sourceChanged; });

			if (sourceChanged) {
				updateNdiSources();
				sourceChanged = false;
			}
		}
	};

	updateNdiSources();
	sourceWaiter = std::async(std::launch::async, waiterLambda);

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

CtrlPanelComponent::~CtrlPanelComponent() {
	shutdown = true;
	sourceWaiter.get();
	saveProgramSettings();
}

//==============================================================================
void CtrlPanelComponent::resized() {
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
	logo.setBounds(bounds.removeFromBottom(buttonHeight)
					   .reduced(padding, 0)
					   .removeFromRight(logoWidth));
}

void CtrlPanelComponent::paint(Graphics &g) {
	// Draw background image
	g.setOpacity(1.0f); // Make sure the image is drawn opaque
	g.drawImage(backgroundImage, getLocalBounds().toFloat());
}

//==============================================================================
void CtrlPanelComponent::setApplicationUnlocked(bool shouldBeUnlocked) {
	if (!shouldBeUnlocked)
		tabSelector.setCurrentTabIndex(aboutTabIndex);

	auto *homeButton =
		tabSelector.getTabbedButtonBar().getTabButton(homeTabIndex);
	homeButton->setEnabled(shouldBeUnlocked);

	aboutTab->setApplicationLicensed(shouldBeUnlocked);
}

//==============================================================================
void CtrlPanelComponent::loadProgramSettings() {}

void CtrlPanelComponent::saveProgramSettings() {}
