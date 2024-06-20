#pragma once
#include <JuceHeader.h>
#include <NDISourceFinder.hpp>
#include "SearchBox.hpp"

class HomeTab : public Component,
				public Button::Listener,
				NDISourceFinder::Listener {
public:
	explicit HomeTab(Component &parent);

	//==========================================================================
	void resized() override;
	void paint(Graphics &g) override;

	//==========================================================================
	void buttonClicked(Button *button) override;

private:
	void sourceListChanged(std::vector<std::string> updatedSourcesMap) override;
	//==========================================================================
	static constexpr int ndiSettingsWidth{400};
	juce::Rectangle<int> ndiSettings;
	const String		 defaultOut = "NDI Virtual Driver";

	Label	   ndiSourceLabel{"ndiSourceLabel", "NDI Source:"};
	SearchBox  ndiSourceCombo{"NDI Source List"};
	Label	   ndiOutputLabel{"ndiOutputLabel", "NDI Out Name:"};
	TextEditor ndiOutput{defaultOut};

	//==========================================================================
	TextButton acceptButton{"ACCEPT"};
	TextButton cancelButton{"CANCEL"};

	NDISourceFinder finder_{};
	Component	   &parent_;

	//==========================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HomeTab)
};
