#pragma once
#include <JuceHeader.h>
#include <future>
#include "AboutTab.hpp"
#include "HomeTab.hpp"
#include "AppSysTrayComponent.hpp"
#include "LicensePanel.hpp"
#include "LicenseCheckComponent.hpp"
#include "settings.hpp"


#define ONLINE_CHECK_DAYS_INTERVAL 30

class MainWindow : public Component {
public:
	MainWindow(Component &parent);
	~MainWindow() override;

	//==========================================================================
	void resized() override;
	void paint(Graphics &g) override;

	//==========================================================================
	void setApplicationUnlocked(bool shouldBeUnlocked);

	void onAcceptClick(std::string newSource, std::string newOutput);

private:
	void checkLicenseExpired(std::string license);
	void deactivate();

	//==========================================================================
	std::future<void> sourceWaiter;
	bool			  shutdown{false};

	//==========================================================================
	static constexpr int width{620};
	static constexpr int height{360};
	static constexpr int logoWidth{140};

	//==========================================================================
	Label titleLabel{"titleLabel", "NDI Virtual Soundcard"};

	enum TabIndex { homeTabIndex = 0, aboutTabIndex };

	HomeTab			homeTab;
	AboutTab		aboutTab;
	TabbedComponent tabSelector{TabbedButtonBar::TabsAtBottom};

	std::unique_ptr<Drawable> logo;
	DrawableButton			  logoBtn{"logo", DrawableButton::ImageFitted};

	Image				  backgroundImage;
	LicensePanel		  licensePanel;
	LicenseCheckComponent licenseCheckScreen;

	//==========================================================================
	TooltipWindow		tooltipWindow{nullptr, 700};
	AppSysTrayComponent sysTray;

	LocalSettings config;

	//==========================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};
