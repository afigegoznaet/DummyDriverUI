#pragma once
#include <JuceHeader.h>
class AboutTab : public Component, public Button::Listener {
public:
	explicit AboutTab();

	//==========================================================================
	void resized() override;
	void buttonClicked(Button *buttonThatWasClicked) override;

	//==========================================================================
	void setApplicationLicensed(bool isLicensed);

	//==========================================================================
	std::function<void()> onLicenseButtonClick{nullptr};

private:
	//==========================================================================
	Label licensedLabel{"licensedLabel", "Unlicensed"};
	Label interfaceVersionLabel{"interfaceVersionLabel", ""};
	Label libraryVersionLabel{"libraryVersionLabel", ""};

	//==========================================================================
	TextEditor eula;

	TextButton licenseButton{"LICENSE"};
	TextButton helpButton{"HELP"};

	URL helpUrl{"https://avsono.com/"};

	//==========================================================================
	// AsioSample *driver;

	//==========================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AboutTab)
};
