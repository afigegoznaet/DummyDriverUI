#pragma once

#include <JuceHeader.h>

class LicenseCheckComponent : public Component {
public:
	LicenseCheckComponent();

	void resized() override;
	void paint(Graphics &g) override;

private:
	Label status{"licenseChecking", "Checking the license key..."};

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LicenseCheckComponent)
};
