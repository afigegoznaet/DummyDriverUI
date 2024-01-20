#pragma once
#include <JuceHeader.h>
#include <future>
#include "abouttab.hpp"
#include "hometab.hpp"

//==============================================================================
// Utility components
class LogoComponent : public Component {
public:
	LogoComponent();

	void paint(Graphics &g) override;

private:
	Image logoImage;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogoComponent)
};

class LicensePanel : public Component, public Button::Listener {
public:
	explicit LicensePanel();

	//==========================================================================
	void resized() override;
	void paint(Graphics &g) override;
	void buttonClicked(Button *buttonThatWasClicked) override;
	void show();

	//==========================================================================
	std::function<void()> onLicenseActivate{nullptr};

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

class LicenseCheckComponent : public Component {
public:
	LicenseCheckComponent();

	void resized() override;
	void paint(Graphics &g) override;

private:
	Label status{"licenseChecking", "Checking the license key..."};

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LicenseCheckComponent)
};

//==============================================================================
class CtrlPanelComponent : public Component {
public:
	CtrlPanelComponent();
	~CtrlPanelComponent() override;

	//==========================================================================
	void resized() override;
	void paint(Graphics &g) override;

	//==========================================================================
	void setApplicationUnlocked(bool shouldBeUnlocked);

	//==========================================================================
	static constexpr int width{620};
	static constexpr int height{360};

private:
	static constexpr int logoWidth{140};

	//==========================================================================
	Label titleLabel{"titleLabel", "NDI Virtual Soundcard"};

	enum TabIndex { homeTabIndex = 0, aboutTabIndex };

	std::unique_ptr<HomeTab>  homeTab;
	std::unique_ptr<AboutTab> aboutTab;
	TabbedComponent			  tabSelector{TabbedButtonBar::TabsAtBottom};

	LogoComponent		  logo;
	Image				  backgroundImage;
	LicensePanel		  licensePanel;
	LicenseCheckComponent licenseCheckScreen;

	//==========================================================================
	TooltipWindow tooltipWindow{nullptr, 700};

	//==========================================================================
	void updateNdiSources() { homeTab->updateNdiSources(); }

	//==========================================================================
	std::future<void> sourceWaiter;
	bool			  shutdown{false};

	std::condition_variable cv{};
	std::mutex				cv_m{};
	bool					sourceChanged{};

	void saveProgramSettings();
	void loadProgramSettings();

	//==========================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CtrlPanelComponent)
};
