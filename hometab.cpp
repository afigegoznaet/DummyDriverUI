#include "hometab.hpp"
#include "applookandfeel.hpp"

HomeTab::HomeTab() {
	//==========================================================================
	// Set up NDI Source selector
	// ndiSourceLabel.setFont(AppLookAndFeel::defaultFontSize);
	// ndiSourceLabel.setJustificationType(Justification::topLeft);
	// addAndMakeVisible(ndiSourceLabel);

	// ndiSource.setTextWhenNothingSelected("Select the NDI source");
	// ndiSource.setTextWhenNoChoicesAvailable("No NDI sources available");
	// addAndMakeVisible(ndiSource);


	//==========================================================================
	// Set up NDI Output selector
	ndiOutputLabel.setFont(AppLookAndFeel::defaultFontSize);
	ndiOutputLabel.setJustificationType(Justification::topLeft);
	addAndMakeVisible(ndiOutputLabel);

	ndiOutput.setMultiLine(false, false);
	ndiOutput.setJustification(Justification::centredLeft);
	ndiOutput.setIndents(10, 0);
	ndiOutput.setFont(AppLookAndFeel::defaultFontSize);
	ndiOutput.setTextToShowWhenEmpty("Type the NDI OUT source name",
									 juce::Colours::grey);
	//	ndiOutput.setText(driver->getNDIOutputName(), dontSendNotification);
	addAndMakeVisible(ndiOutput);
	// localAudioCheckbox.setToggleState(driver->getUseLocalAudio(),
	// 								  dontSendNotification);
	// addAndMakeVisible(localAudioCheckbox);

	//==========================================================================
	// Set up buttons
	acceptButton.addListener(this);
	addAndMakeVisible(acceptButton);

	cancelButton.addListener(this);
	addAndMakeVisible(cancelButton);
}

//==============================================================================
void HomeTab::resized() {
	const auto padding = AppLookAndFeel::padding;
	const auto labelHeight = AppLookAndFeel::labelHeight;
	const auto buttonWidth = AppLookAndFeel::buttonWidth;
	const auto buttonHeight = AppLookAndFeel::buttonHeight;

	auto bounds = getLocalBounds();

	// Position buttons
	bounds.removeFromBottom(padding);
	auto buttonBounds =
		bounds.removeFromBottom(buttonHeight)
			.withSizeKeepingCentre(2 * buttonWidth + padding, buttonHeight);
	acceptButton.setBounds(buttonBounds.removeFromLeft(buttonWidth));
	cancelButton.setBounds(buttonBounds.removeFromRight(buttonWidth));
	bounds.removeFromBottom(padding);

	// Position setting panel
	ndiSettings = bounds.withSizeKeepingCentre(ndiSettingsWidth + padding,
											   bounds.getHeight());

	auto settingsBounds = ndiSettings.reduced(padding / 2);

	// ndiSourceLabel.setBounds(settingsBounds.removeFromTop(labelHeight));
	// ndiSource.setBounds(settingsBounds.removeFromTop(buttonHeight));

	ndiOutputLabel.setBounds(settingsBounds.removeFromTop(labelHeight));
	ndiOutput.setBounds(settingsBounds.removeFromTop(buttonHeight));
	// localAudioCheckbox.setBounds(settingsBounds.removeFromTop(buttonHeight));
}

void HomeTab::paint(Graphics &g) {
	g.setColour(Colours::white);
	const float lineThickness = 1.0f;
	const float cornerRadius = 5.0f;
	g.drawRoundedRectangle(ndiSettings.toFloat().reduced(lineThickness / 2.0f),
						   cornerRadius, lineThickness);
}

//==============================================================================
void HomeTab::buttonClicked(Button *button) {
	if (button == &acceptButton) {
		String newOutput = ndiOutput.getText();

		if (newOutput.isEmpty())
			newOutput = "NDI Virtual Sound";

		// bool useLocalAudio = localAudioCheckbox.getToggleState();

		String oldOuptut = ""; // driver->getNDIOutputName();

		// String rawNewSource =
		// 	ndiSource.getItemText(ndiSource.getSelectedItemIndex());

		// String newSource = rawNewSource.replace("(" + oldOuptut + ")",
		// 										"(" + newOutput + ")", false);

		// driver->onAcceptClick(newSource.toStdString(),
		// newOutput.toStdString(), 					  useLocalAudio);

		// Signal UI termination
		JUCEApplicationBase::quit();
	} else if (button == &cancelButton) {
		// Signal UI termination
		JUCEApplicationBase::quit();
	}
}

//==============================================================================
void HomeTab::updateNdiSources() {
	// MessageManagerLock msgLock;

	// ndiSource.clear(NotificationType::dontSendNotification);

	// auto &availableInputs = driver->getNDIAvailableInputs();

	// for (const auto &input : availableInputs)
	// 	ndiSource.addItem(input, ndiSource.getNumItems() + 1);

	// const auto selectedPosition =
	// 	std::find(availableInputs.cbegin(), availableInputs.cend(),
	// 			  driver->getNDIInputName());

	// if (selectedPosition == availableInputs.cend()) {
	// 	ndiSource.setSelectedId(0);
	// 	return;
	// }

	// auto selectedIndex =
	// 	std::distance(availableInputs.cbegin(), selectedPosition);
	// ndiSource.setSelectedItemIndex(static_cast<int>(selectedIndex));
}
