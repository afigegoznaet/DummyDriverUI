#pragma once
#include <JuceHeader.h>
#include <NDISourceFinder.hpp>
#include "SearchBox.hpp"

class MainWindow;

class HomeTab : public Component,
				public Button::Listener,
				NDISourceFinder::Listener {
public:
	explicit HomeTab(MainWindow &parent);

	//==========================================================================
	void resized() override;
	void paint(Graphics &g) override;

	//==========================================================================
	void buttonClicked(Button *button) override;
	void onConfigLoad(const std::string &newSource,
					  const std::string &newOutput);

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
	MainWindow	   &parent_;

	//==========================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HomeTab)
};
