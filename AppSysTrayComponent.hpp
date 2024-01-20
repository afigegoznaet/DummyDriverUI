#pragma once
#include <JuceHeader.h>

class AppSysTrayComponent : public SystemTrayIconComponent {
public:
	AppSysTrayComponent(Component &parent);

protected:
	void mouseDown(const MouseEvent &event) override;
	void mouseDoubleClick(const MouseEvent &event) override;

private:
	Component &parent_;
	Image	   icon;

	PopupMenu menu;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AppSysTrayComponent)
};
