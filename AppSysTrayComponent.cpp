#include "AppSysTrayComponent.hpp"
#include "Assets.hpp"

AppSysTrayComponent::AppSysTrayComponent(Component &parent) : parent_{parent} {
	icon = ImageCache::getFromMemory(Assets::icon_png, Assets::icon_pngSize);
	setIconImage(icon, icon);
	menu.addItem("Show UI", [this] { parent_.setVisible(true); });
	menu.addItem("Exit", [] { JUCEApplicationBase::quit(); });
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
