#pragma once

#include <JuceHeader.h>
#include <Assets.hpp>

class AppLookAndFeel : public LookAndFeel_V4 {
public:
	AppLookAndFeel();

	//==========================================================================
	// Interface Constants
	static constexpr int   padding{20};
	static constexpr int   labelHeight{25};
	static constexpr int   titleHeight{55};
	static constexpr int   buttonHeight{40};
	static constexpr int   buttonWidth{120};
	static constexpr float cornerSize{5.0f};

	static constexpr int panelHeight{205};
	static constexpr int panelWidth{480};

	//==========================================================================
	// Font sizes
	static constexpr float defaultFontSize{16.0f};
	static constexpr float infoFontSize{15.0f};
	static constexpr float buttonFontSize{15.0f};
	static constexpr float titleFontSize{22.0f};

private:
	//==========================================================================
	// Application colours
	//  UI colours are defined here.
	//  To access the colours, use `findColour()` method of JUCE Component
	//  class with the appropriate ColourId.
	inline static const Colour mainTextColour{0xFFFFFFFF};
	inline static const Colour inactiveTextColour{0xFF737373};

	inline static const Colour outlineColour{mainTextColour};

	inline static const Colour buttonColour{0xFFFFD500};
	inline static const Colour buttonTextColour{0xFF000000};

	inline static const Colour inputFieldColour{0xFF252B42};

	inline static const Colour frontTabColour{outlineColour};
	inline static const Colour tabColour{0xFF353638};
	inline static const Colour frontTabTextColour{buttonTextColour};
	inline static const Colour tabTextColour{mainTextColour};

	inline static const Colour alertWindowColour{0xFF161829};

public:
	//==========================================================================
	// Custom Colour IDs
	enum ColourIds {
		frontTabColourId = 1,
		tabColourId,
		frontTabTextColourId,
		tabTextColourId,
		highlightColourId,
		inactiveTextColourId,
	};

	//==========================================================================
	// Button painting
	void drawButtonBackground(Graphics &g, Button &button,
							  const Colour &backgroundColour,
							  bool			shouldDrawButtonAsHighlighted,
							  bool			shouldDrawButtonAsDown) override;

	Font getTextButtonFont(TextButton &button, int buttonHeight) override;

	//==========================================================================
	// Popup menu painting
	Font getPopupMenuFont() override;
	void drawPopupMenuItem(Graphics &g, const juce::Rectangle<int> &area,
						   const bool isSeparator, const bool isActive,
						   const bool isHighlighted, const bool isTicked,
						   const bool hasSubMenu, const String &text,
						   const String &shortcutKeyText, const Drawable *icon,
						   const Colour *const textColourToUse) override;

	//==========================================================================
	// Text editor painting
	void fillTextEditorBackground(Graphics &g, int width, int height,
								  TextEditor &textEditor) override;
	void drawTextEditorOutline(Graphics &g, int width, int height,
							   TextEditor &textEditor) override;

	//==========================================================================
	// Tab painting
	int	 getTabButtonBestWidth(TabBarButton &button, int tabDepth) override;
	void createTabTextLayout(const TabBarButton &button, float length,
							 float depth, Colour colour,
							 TextLayout &textLayout);
	void drawTabButton(TabBarButton &button, Graphics &, bool isMouseOver,
					   bool isMouseDown) override;

	//==========================================================================
	// AlertWindow painting
	void drawAlertBox(Graphics &g, AlertWindow &alert,
					  const juce::Rectangle<int> &textArea,
					  TextLayout				 &textLayout) override;
	Array<int>
		 getWidthsForTextButtons(AlertWindow &,
								 const Array<TextButton *> &buttons) override;
	int	 getAlertWindowButtonHeight() override;
	Font getAlertWindowTitleFont() override;
	Font getAlertWindowMessageFont() override;
	Font getAlertWindowFont() override;

	//==========================================================================
	// Custom component drawing
	static void
	drawCustomPanelBackground(Graphics					 &g,
							  const juce::Rectangle<int> &panelArea);

private:
	//==========================================================================
	// Custom button font
	const Typeface::Ptr regularTypeFace = Typeface::createSystemTypefaceFor(
		Assets::LatoRegular_ttf, Assets::LatoRegular_ttfSize);
	const Font buttonFont =
		Font(regularTypeFace).withPointHeight(buttonFontSize);

	//==========================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AppLookAndFeel)
};
