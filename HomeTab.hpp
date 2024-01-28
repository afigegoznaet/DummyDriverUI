#pragma once
#include <JuceHeader.h>
#include "searchbox.hpp"

class HomeTab : public Component, public Button::Listener {
public:
	explicit HomeTab(Component &parent);

	//==========================================================================
	void resized() override;
	void paint(Graphics &g) override;

	//==========================================================================
	void buttonClicked(Button *button) override;

	//==========================================================================
	void updateNdiSources();

private:
	//==========================================================================
	static constexpr int ndiSettingsWidth{400};
	juce::Rectangle<int> ndiSettings;
	const String		 defaultOut = "NDI Virtual Driver";

	// Label		 ndiSourceLabel{"ndiSourceLabel", "NDI Source:"};
	// SearchBox	 ndiSource{"NDI Source"};
	Label	   ndiOutputLabel{"ndiOutputLabel", "NDI Out Name:"};
	TextEditor ndiOutput{defaultOut};

	//==========================================================================
	TextButton acceptButton{"ACCEPT"};
	TextButton cancelButton{"CANCEL"};

	Component &parent_;

	//==========================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HomeTab)
};
