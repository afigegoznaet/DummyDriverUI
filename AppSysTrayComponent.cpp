#include "AppSysTrayComponent.hpp"
#include "Assets.hpp"
#include <thread>

AppSysTrayComponent::AppSysTrayComponent(Component &parent) : parent_{parent} {
	icon = ImageCache::getFromMemory(Assets::icon_png, Assets::icon_pngSize);
	setIconImage(icon, icon);
	menu.addItem("Show UI", [this] { parent_.setVisible(true); });
	menu.addItem("Exit", [this] {
		// MessageManager::callAsync([this]{
		{
			Timer::callAfterDelay(100, [] {
				// JUCEApplication::getInstance()->systemRequestedQuit();
				// JUCEApplication::getInstance()->shutdown();
				JUCEApplication::getInstance()->systemRequestedQuit();
			}); // std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});
}

void AppSysTrayComponent::mouseDown(const MouseEvent &event) {
	if (event.mods.isLeftButtonDown()) {
		parent_.setVisible(true);
	}
	if (event.mods.isRightButtonDown()) {
		menu.show();
	}
}
void AppSysTrayComponent::mouseDoubleClick(const MouseEvent &event) {
	parent_.setVisible(true);
}
