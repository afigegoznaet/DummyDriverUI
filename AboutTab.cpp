#include "AboutTab.hpp"
#include "AppLookAndFeel.hpp"
#include "Assets.hpp"

AboutTab::AboutTab() {
	const auto infoColour = findColour(TextEditor::highlightedTextColourId);
	const auto infoFontSize = AppLookAndFeel::infoFontSize;

	//==========================================================================
	// Set up info labels
	//  License info
	licensedLabel.setJustificationType(Justification::topLeft);
	licensedLabel.setColour(Label::textColourId, infoColour);
	licensedLabel.setFont(infoFontSize);
	addAndMakeVisible(licensedLabel);

	//  Interface version
	auto interfaceVersionString = String("").removeCharacters("\"");
	interfaceVersionLabel.setText(interfaceVersionString, dontSendNotification);
	interfaceVersionLabel.setJustificationType(Justification::centredTop);
	interfaceVersionLabel.setColour(Label::textColourId, infoColour);
	interfaceVersionLabel.setFont(infoFontSize);
	addAndMakeVisible(interfaceVersionLabel);

	//  Library version
	auto libraryVersionString = String("").removeCharacters("\"");
	libraryVersionLabel.setText(libraryVersionString, dontSendNotification);
	libraryVersionLabel.setJustificationType(Justification::topRight);
	libraryVersionLabel.setColour(Label::textColourId, infoColour);
	libraryVersionLabel.setFont(infoFontSize);
	addAndMakeVisible(libraryVersionLabel);

	//==========================================================================
	// Set up EULA viewer
	const auto eulaText =
		String::createStringFromData(Assets::EULA_txt, Assets::EULA_txtSize);
	eula.setMultiLine(true, true);
	eula.setReadOnly(true);
	eula.setText(eulaText);
	eula.setFont(AppLookAndFeel::infoFontSize);
	eula.setJustification(Justification::horizontallyJustified);
	addAndMakeVisible(eula);

	//==========================================================================
	// Set up buttons
	//  License button
	licenseButton.addListener(this);
	addAndMakeVisible(licenseButton);

	//  Help button
	helpButton.addListener(this);
	addAndMakeVisible(helpButton);
}

void AboutTab::buttonClicked(Button *button) {
	if (button == &licenseButton) {
		if (onLicenseButtonClick != nullptr)
			onLicenseButtonClick();
	} else if (button == &helpButton) {
		if (!helpUrl.launchInDefaultBrowser())
			AlertWindow::showMessageBoxAsync(
				MessageBoxIconType::WarningIcon, "ERROR",
				"Couldn't launch the default web browser");
	}
}

void AboutTab::resized() {
	const auto padding = AppLookAndFeel::padding;
	const auto labelHeight = AppLookAndFeel::labelHeight;
	const auto buttonWidth = AppLookAndFeel::buttonWidth;
	const auto buttonHeight = AppLookAndFeel::buttonHeight;

	auto bounds = getLocalBounds();
	bounds.removeFromLeft(padding);
	bounds.removeFromRight(padding);

	// Position info labels
	auto	  infoBounds = bounds.removeFromTop(labelHeight);
	const int infoWidth = infoBounds.getWidth() / 3;
	licensedLabel.setBounds(infoBounds.removeFromLeft(infoWidth));
	libraryVersionLabel.setBounds(infoBounds.removeFromRight(infoWidth));
	interfaceVersionLabel.setBounds(infoBounds);

	// Position buttons
	bounds.removeFromBottom(padding);
	auto buttonBounds =
		bounds.removeFromBottom(buttonHeight)
			.withSizeKeepingCentre(2 * buttonWidth + padding, buttonHeight);
	licenseButton.setBounds(buttonBounds.removeFromLeft(buttonWidth));
	helpButton.setBounds(buttonBounds.removeFromRight(buttonWidth));
	bounds.removeFromBottom(padding);

	// Position EULA viewer
	eula.setBounds(bounds);
}

//==============================================================================
void AboutTab::setApplicationLicensed(bool isLicensed) {
	licensedLabel.setText(isLicensed ? "Licensed" : "Unlicensed",
						  dontSendNotification);
}
