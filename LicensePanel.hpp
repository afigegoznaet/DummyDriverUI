#pragma once
#include <JuceHeader.h>

class LicensePanel : public Component, public Button::Listener {
public:
	explicit LicensePanel();

	//==========================================================================
	void resized() override;
	void paint(Graphics &g) override;
	void buttonClicked(Button *buttonThatWasClicked) override;
	void show();
	void setLicense(const std::string &license);

	//==========================================================================
	std::function<void(std::string)> onLicenseActivate{nullptr};
	std::function<void()>			 onLicenseDeactivate{nullptr};


private:
	Label	   panelTitleLabel{"licensePanelTitle", "License key"};
	TextEditor licenseEditor;
	Label	   copyrightInfo{"", CharPointer_UTF8("Avsono Inc. "
											  "\xc2\xa9"
											  "2021 - 2024")};

	TextButton activateButton{"ACTIVATE"};
	TextButton deactivateButton{"DEACTIVATE"};
	TextButton cancelButton{"X"};

	//==========================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LicensePanel)
};
