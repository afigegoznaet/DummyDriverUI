#include "HomeTab.hpp"
#include "MainWindow.hpp"
#include "AppLookAndFeel.hpp"

HomeTab::HomeTab(MainWindow &parent) : parent_{parent} {
	//==========================================================================
	// Set up NDI Source selector
	ndiSourceLabel.setFont(AppLookAndFeel::defaultFontSize);
	ndiSourceLabel.setJustificationType(Justification::topLeft);
	addAndMakeVisible(ndiSourceLabel);

	ndiSourceCombo.setTextWhenNothingSelected("Select the NDI source");
	ndiSourceCombo.setTextWhenNoChoicesAvailable("No NDI sources available");
	addAndMakeVisible(ndiSourceCombo);


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
	// ndiOutput.setText(finder->getNDIOutputName(), dontSendNotification);
	addAndMakeVisible(ndiOutput);

	//==========================================================================
	// Set up buttons
	acceptButton.addListener(this);
	addAndMakeVisible(acceptButton);

	cancelButton.addListener(this);
	addAndMakeVisible(cancelButton);
	finder_.setListener(this);
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

	ndiSourceLabel.setBounds(settingsBounds.removeFromTop(labelHeight));
	ndiSourceCombo.setBounds(settingsBounds.removeFromTop(buttonHeight));

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
			newOutput = defaultOut;

		String rawNewSource =
			ndiSourceCombo.getItemText(ndiSourceCombo.getSelectedItemIndex());

		parent_.onAcceptClick(rawNewSource.toStdString(),
							  newOutput.toStdString());

	} else if (button == &cancelButton) {
	}
}

void HomeTab::sourceListChanged(std::vector<std::string> updatedSourcesMap) {
	MessageManagerLock msgLock;

	String oldSource =
		ndiSourceCombo.getItemText(ndiSourceCombo.getSelectedItemIndex());
	const auto selectedPosition = std::find(
		updatedSourcesMap.cbegin(), updatedSourcesMap.cend(), oldSource);


	ndiSourceCombo.clear(NotificationType::dontSendNotification);

	for (const auto &input : updatedSourcesMap)
		ndiSourceCombo.addItem(input, ndiSourceCombo.getNumItems() + 1);

	if (selectedPosition == updatedSourcesMap.cend()) {
		ndiSourceCombo.setSelectedId(0);
		return;
	}

	auto selectedIndex =
		std::distance(updatedSourcesMap.cbegin(), selectedPosition);

	ndiSourceCombo.setSelectedItemIndex(static_cast<int>(selectedIndex));
}

void HomeTab::onConfigLoad(const std::string &newSource,
						   const std::string &newOutput) {
	MessageManagerLock msgLock;

	ndiSourceCombo.clear(NotificationType::dontSendNotification);

	ndiSourceCombo.addItem(newSource, 1);
	ndiSourceCombo.setSelectedItemIndex(1);

	ndiOutput.setText(newOutput);
}
