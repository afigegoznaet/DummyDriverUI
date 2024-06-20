#include "AppLookAndFeel.hpp"

AppLookAndFeel::AppLookAndFeel() {
	// Assign colours to ColourIds:
	//  Label colours
	setColour(Label::textColourId, mainTextColour);
	setColour(highlightColourId, buttonColour);
	setColour(inactiveTextColourId, inactiveTextColour);

	//  Button colours
	setColour(TextButton::buttonColourId, buttonColour);
	setColour(TextButton::textColourOnId, buttonTextColour);
	setColour(TextButton::textColourOffId, buttonTextColour);

	//  Combobox colours
	setColour(ComboBox::outlineColourId, Colours::transparentWhite);
	setColour(ComboBox::backgroundColourId, inputFieldColour);
	setColour(ComboBox::textColourId, mainTextColour);
	setColour(ComboBox::arrowColourId, outlineColour);

	setColour(PopupMenu::textColourId, mainTextColour);
	setColour(PopupMenu::highlightedTextColourId, mainTextColour);
	setColour(PopupMenu::backgroundColourId, inputFieldColour.darker(0.25f));
	setColour(PopupMenu::highlightedBackgroundColourId,
			  inputFieldColour.darker(2.0f));

	//  Text editor colours
	setColour(TextEditor::backgroundColourId, inputFieldColour);
	setColour(TextEditor::focusedOutlineColourId, outlineColour);

	//  Scrollbar colours
	setColour(ScrollBar::thumbColourId, outlineColour.withAlpha(0.5f));

	//  Tab colours
	setColour(TabbedComponent::outlineColourId, Colours::transparentWhite);
	setColour(frontTabColourId, frontTabColour);
	setColour(tabColourId, tabColour);
	setColour(frontTabTextColourId, frontTabTextColour);
	setColour(tabTextColourId, tabTextColour);

	//  AlertWindow colours
	setColour(AlertWindow::backgroundColourId, alertWindowColour);
	setColour(AlertWindow::textColourId, mainTextColour);
	setColour(AlertWindow::outlineColourId, outlineColour);
}

//==============================================================================
void AppLookAndFeel::drawButtonBackground(Graphics &g, Button &button,
										  const Colour &backgroundColour,
										  bool shouldDrawButtonAsHighlighted,
										  bool shouldDrawButtonAsDown) {
	auto baseColour =
		backgroundColour
			.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f
																	: 0.9f)
			.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

	if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
		baseColour =
			baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);

	g.setColour(baseColour);

	const auto bounds = button.getLocalBounds().toFloat();

	auto flatOnLeft = button.isConnectedOnLeft();
	auto flatOnRight = button.isConnectedOnRight();
	auto flatOnTop = button.isConnectedOnTop();
	auto flatOnBottom = button.isConnectedOnBottom();

	if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom) {
		Path path;
		path.addRoundedRectangle(
			bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
			cornerSize, cornerSize, !(flatOnLeft || flatOnTop),
			!(flatOnRight || flatOnTop), !(flatOnLeft || flatOnBottom),
			!(flatOnRight || flatOnBottom));

		g.fillPath(path);
	} else {
		g.fillRoundedRectangle(bounds, cornerSize);
	}
}

Font AppLookAndFeel::getTextButtonFont(TextButton & /*button*/,
									   int /*buttonHeight*/) {
	return buttonFont;
}

//==============================================================================
Font AppLookAndFeel::getPopupMenuFont() { return {defaultFontSize}; }

void AppLookAndFeel::drawPopupMenuItem(
	Graphics &g, const Rectangle<int> &area, const bool isSeparator,
	const bool isActive, const bool isHighlighted, const bool isTicked,
	const bool hasSubMenu, const String &text, const String &shortcutKeyText,
	const Drawable *icon, const Colour *const textColourToUse) {
	if (isSeparator) {
		auto r = area.reduced(5, 0);
		r.removeFromTop(roundToInt(((float)r.getHeight() * 0.5f) - 0.5f));

		g.setColour(findColour(PopupMenu::textColourId).withAlpha(0.3f));
		g.fillRect(r.removeFromTop(1));
	} else {
		auto textColour =
			(textColourToUse == nullptr ? findColour(PopupMenu::textColourId)
										: *textColourToUse);

		auto r = area.reduced(1);

		if (isHighlighted && isActive) {
			g.setColour(findColour(PopupMenu::highlightedBackgroundColourId));
			g.fillRect(r);

			g.setColour(findColour(PopupMenu::highlightedTextColourId));
		} else {
			g.setColour(textColour.withMultipliedAlpha(isActive ? 1.0f : 0.5f));
		}

		r.reduce(jmin(5, area.getWidth() / 20), 0);

		auto font = getPopupMenuFont();

		auto maxFontHeight = (float)r.getHeight() / 1.3f;

		if (font.getHeight() > maxFontHeight)
			font.setHeight(maxFontHeight);

		g.setFont(font);

		auto iconArea = r.removeFromLeft(roundToInt(maxFontHeight)).toFloat();

		if (icon != nullptr) {
			icon->drawWithin(g, iconArea,
							 RectanglePlacement::centred
								 | RectanglePlacement::onlyReduceInSize,
							 1.0f);
			r.removeFromLeft(roundToInt(maxFontHeight * 0.5f));
		} else if (isTicked) {
			g.fillRoundedRectangle(iconArea.withSizeKeepingCentre(6.0f, 6.0f),
								   1.0f);
		}

		if (hasSubMenu) {
			auto arrowH = 0.6f * getPopupMenuFont().getAscent();

			auto x = static_cast<float>(r.removeFromRight((int)arrowH).getX());
			auto halfH = static_cast<float>(r.getCentreY());

			Path path;
			path.startNewSubPath(x, halfH - arrowH * 0.5f);
			path.lineTo(x + arrowH * 0.6f, halfH);
			path.lineTo(x, halfH + arrowH * 0.5f);

			g.strokePath(path, PathStrokeType(2.0f));
		}

		r.removeFromRight(3);
		g.drawFittedText(text, r, Justification::centredLeft, 1);

		if (shortcutKeyText.isNotEmpty()) {
			auto f2 = font;
			f2.setHeight(f2.getHeight() * 0.75f);
			f2.setHorizontalScale(0.95f);
			g.setFont(f2);

			g.drawText(shortcutKeyText, r, Justification::centredRight, true);
		}
	}
}

//==============================================================================
void AppLookAndFeel::fillTextEditorBackground(Graphics &g, int width,
											  int		  height,
											  TextEditor &textEditor) {
	g.setColour(textEditor.findColour(TextEditor::backgroundColourId));
	g.fillRoundedRectangle(0, 0, width, height, cornerSize);
}

void AppLookAndFeel::drawTextEditorOutline(Graphics &g, int width, int height,
										   TextEditor &textEditor) {
	if (textEditor.isEnabled() && textEditor.hasKeyboardFocus(true)
		&& !textEditor.isReadOnly()) {
		g.setColour(textEditor.findColour(TextEditor::focusedOutlineColourId)
						.withMultipliedAlpha(0.75f));

		const float lineThickness = 1.0f;
		const auto	bounds = juce::Rectangle<float>(0, 0, width, height);
		g.drawRoundedRectangle(bounds.reduced(0.5f * lineThickness), cornerSize,
							   lineThickness);
	}
}

//==============================================================================
int AppLookAndFeel::getTabButtonBestWidth(TabBarButton &button,
										  int /*tabDepth*/) {
	const int numTabs = button.getTabbedButtonBar().getNumTabs();
	const int totalWidth = button.getTabbedButtonBar().getWidth();
	return jmax(buttonWidth, totalWidth / numTabs);
}

void AppLookAndFeel::createTabTextLayout(const TabBarButton &button,
										 float		 length, float /*depth*/,
										 Colour		 colour,
										 TextLayout &textLayout) {
	Font font = buttonFont;
	font.setUnderline(button.hasKeyboardFocus(false));

	AttributedString s;
	s.setJustification(Justification::centred);
	s.append(button.getButtonText().trim(), font, colour);

	textLayout.createLayout(s, length);
}

void AppLookAndFeel::drawTabButton(TabBarButton &button, Graphics &g,
								   bool /*isMouseOver*/, bool /*isMouseDown*/) {
	const Rectangle<int> activeArea(button.getActiveArea());

	const TabbedButtonBar::Orientation o =
		button.getTabbedButtonBar().getOrientation();

	const Colour bgColour =
		findColour(button.getToggleState() ? frontTabColourId : tabColourId)
			.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.75f);

	const Colour textColour =
		findColour(button.getToggleState() ? frontTabTextColourId
										   : tabTextColourId)
			.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.25f);

	g.setColour(bgColour);
	g.fillRect(activeArea);

	g.setColour(button.findColour(TabbedButtonBar::tabOutlineColourId));

	Rectangle<int> r(activeArea);

	if (o != TabbedButtonBar::TabsAtBottom)
		g.fillRect(r.removeFromTop(1));
	if (o != TabbedButtonBar::TabsAtTop)
		g.fillRect(r.removeFromBottom(1));
	if (o != TabbedButtonBar::TabsAtRight)
		g.fillRect(r.removeFromLeft(1));
	if (o != TabbedButtonBar::TabsAtLeft)
		g.fillRect(r.removeFromRight(1));

	const Rectangle<float> area(button.getTextArea().toFloat());

	float length = area.getWidth();
	float depth = area.getHeight();

	if (button.getTabbedButtonBar().isVertical())
		std::swap(length, depth);

	TextLayout textLayout;
	createTabTextLayout(button, length, depth, textColour, textLayout);

	AffineTransform t;

	switch (o) {
	case TabbedButtonBar::TabsAtLeft:
		t = t.rotated(MathConstants<float>::pi * -0.5f)
				.translated(area.getX(), area.getBottom());
		break;
	case TabbedButtonBar::TabsAtRight:
		t = t.rotated(MathConstants<float>::pi * 0.5f)
				.translated(area.getRight(), area.getY());
		break;
	case TabbedButtonBar::TabsAtTop:
	case TabbedButtonBar::TabsAtBottom:
		t = t.translated(area.getX(), area.getY());
		break;
	default:
		jassertfalse;
		break;
	}

	g.addTransform(t);
	textLayout.draw(g, Rectangle<float>(length, depth));
}

//==============================================================================
void AppLookAndFeel::drawAlertBox(Graphics &g, AlertWindow &alert,
								  const Rectangle<int> &textArea,
								  TextLayout		   &textLayout) {
	g.setColour(alert.findColour(AlertWindow::outlineColourId));
	g.drawRoundedRectangle(alert.getLocalBounds().toFloat(), cornerSize, 1.0f);

	auto bounds = alert.getLocalBounds().reduced(1);
	g.reduceClipRegion(bounds);

	g.setColour(alert.findColour(AlertWindow::backgroundColourId));
	g.fillRoundedRectangle(bounds.toFloat(), cornerSize);

	auto iconSpaceUsed = 0;

	auto iconWidth = 80;
	auto iconSize = jmin(iconWidth + 50, bounds.getHeight() + 20);

	if (alert.containsAnyExtraComponents() || alert.getNumButtons() > 2)
		iconSize = jmin(iconSize, textArea.getHeight() + 50);

	Rectangle<int> iconRect(iconSize / -10, iconSize / -10, iconSize, iconSize);

	if (alert.getAlertType() != MessageBoxIconType::NoIcon) {
		Path   icon;
		char   character;
		Colour colour;
		int	   glyphOffset = 0;

		if (alert.getAlertType() == MessageBoxIconType::WarningIcon) {
			character = '!';
			glyphOffset = 10;

			icon.addTriangle(
				(float)iconRect.getX() + (float)iconRect.getWidth() * 0.5f,
				(float)iconRect.getY(), static_cast<float>(iconRect.getRight()),
				static_cast<float>(iconRect.getBottom()),
				static_cast<float>(iconRect.getX()),
				static_cast<float>(iconRect.getBottom()));

			icon = icon.createPathWithRoundedCorners(5.0f);
			colour = buttonColour;
		} else {
			colour = inputFieldColour;
			character = alert.getAlertType() == MessageBoxIconType::InfoIcon
							? 'i'
							: '?';

			icon.addEllipse(iconRect.toFloat());
		}

		GlyphArrangement ga;
		ga.addFittedText({(float)iconRect.getHeight() * 0.8f, Font::bold},
						 String::charToString((juce_wchar)(uint8)character),
						 static_cast<float>(iconRect.getX()),
						 static_cast<float>(iconRect.getY() + glyphOffset),
						 static_cast<float>(iconRect.getWidth()),
						 static_cast<float>(iconRect.getHeight()),
						 Justification::centred, false);
		ga.createPath(icon);

		icon.setUsingNonZeroWinding(false);
		g.setColour(colour);
		g.fillPath(icon);

		iconSpaceUsed = iconWidth + 50;
	}

	g.setColour(alert.findColour(AlertWindow::textColourId));

	Rectangle<int> alertBounds(
		bounds.getX() + iconSpaceUsed, 30, bounds.getWidth(),
		bounds.getHeight() - getAlertWindowButtonHeight() - 20);

	textLayout.draw(g, alertBounds.toFloat());
}

Array<int>
AppLookAndFeel::getWidthsForTextButtons(AlertWindow &,
										const Array<TextButton *> &buttons) {
	const int  n = buttons.size();
	Array<int> buttonWidths;

	for (int i = 0; i < n; ++i)
		buttonWidths.add(buttonWidth - 10);

	return buttonWidths;
}

int	 AppLookAndFeel::getAlertWindowButtonHeight() { return buttonHeight; }
Font AppLookAndFeel::getAlertWindowTitleFont() { return {titleFontSize}; }
Font AppLookAndFeel::getAlertWindowMessageFont() { return {defaultFontSize}; }
Font AppLookAndFeel::getAlertWindowFont() { return {defaultFontSize}; }

//==============================================================================
void AppLookAndFeel::drawCustomPanelBackground(
	Graphics &g, const juce::Rectangle<int> &panelArea) {
	auto &lf = getDefaultLookAndFeel();
	g.setColour(lf.findColour(AlertWindow::backgroundColourId));
	g.fillRoundedRectangle(panelArea.toFloat(), cornerSize);
	g.setColour(lf.findColour(AlertWindow::outlineColourId));
	g.drawRoundedRectangle(panelArea.toFloat().reduced(2.0f), cornerSize, 1.0f);
}
