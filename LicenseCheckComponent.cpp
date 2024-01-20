#include "LicenseCheckComponent.hpp"
#include "AppLookAndFeel.hpp"

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
