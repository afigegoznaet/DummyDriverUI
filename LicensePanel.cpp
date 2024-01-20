#include "LicensePanel.hpp"
#include "AppLookAndFeel.hpp"
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
