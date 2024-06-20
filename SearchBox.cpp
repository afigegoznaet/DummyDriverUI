#include "SearchBox.hpp"
#include "AppLookAndFeel.hpp"

//==============================================================================
// SearchBox painting
static void drawSearchBox(Graphics &g, int width, int height, bool, int, int,
						  int, int, SearchBox		 &box) {
	Rectangle<int> boxBounds(0, 0, width, height);

	g.setColour(box.findColour(ComboBox::backgroundColourId));
	g.fillRoundedRectangle(boxBounds.toFloat(), AppLookAndFeel::cornerSize);

	const auto arrowBounds =
		Rectangle<int>(width - 30, 0, 20, height).withSizeKeepingCentre(12, 8);

	Path arrow;
	arrow.addTriangle(
		arrowBounds.getTopLeft().toFloat(),
		arrowBounds.getCentre().withY(arrowBounds.getBottom()).toFloat(),
		arrowBounds.getTopRight().toFloat());

	g.setColour(box.findColour(ComboBox::arrowColourId)
					.withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
	g.fillPath(arrow);
}

static Font getSearchBoxFont(SearchBox & /*box*/) {
	return {AppLookAndFeel::defaultFontSize};
}

static void positionSearchBoxText(SearchBox &box, Label &label) {
	label.setBounds(box.getLocalBounds().withTrimmedRight(35));
	label.setFont(getSearchBoxFont(box));
}

static void drawSearchBoxTextWhenNothingSelected(Graphics &g, SearchBox &box,
												 Label &label) {
	auto &lf = AppLookAndFeel::getDefaultLookAndFeel();
	g.setColour(
		lf.findColour(ComboBox::textColourId).withMultipliedAlpha(0.5f));

	auto font = label.getLookAndFeel().getLabelFont(label);

	g.setFont(font);

	auto textArea =
		lf.getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());

	g.drawFittedText(
		box.getTextWhenNothingSelected(), textArea,
		label.getJustificationType(),
		jmax(1, (int)((float)textArea.getHeight() / font.getHeight())),
		label.getMinimumHorizontalScale());
}

static Label *createSearchBoxTextBox(SearchBox &) {
	return new Label(String(), String());
}

//==============================================================================
// SearchBoxPopup::ItemComponent
SearchBoxPopup::ItemComponent::ItemComponent(Item it, bool shouldBeTicked,
											 bool shouldBeEnabled)
	: item(it), isTicked(shouldBeTicked), isEnabled(shouldBeEnabled),
	  font(AppLookAndFeel::defaultFontSize) {
	setRepaintsOnMouseActivity(true);
}

void SearchBoxPopup::ItemComponent::resized() {
	// Update tooltip
	const int textWidth = font.getStringWidth(item.text);
	const int availableWidth =
		getLocalBounds().getWidth() - tickSize - textBorder.getLeftAndRight();

	if (textWidth > availableWidth)
		setTooltip(item.text);
	else
		setTooltip("");
}

void SearchBoxPopup::ItemComponent::paint(Graphics &g) {
	if (isMouseOver() && isEnabled)
		g.setColour(findColour(PopupMenu::highlightedBackgroundColourId));
	else
		g.setColour(findColour(PopupMenu::backgroundColourId));

	auto bounds = getLocalBounds();
	g.fillRect(bounds);

	g.setColour(isEnabled ? findColour(PopupMenu::textColourId)
						  : findColour(AppLookAndFeel::inactiveTextColourId));
	const auto tickBounds = bounds.removeFromLeft(tickSize).toFloat();

	if (isTicked)
		g.fillRoundedRectangle(tickBounds.withSizeKeepingCentre(6.0f, 6.0f),
							   1.0f);

	textBorder.subtractFrom(bounds);
	g.setFont(font);
	g.drawText(item.text, bounds, Justification::centredLeft);
}

void SearchBoxPopup::ItemComponent::mouseDown(const MouseEvent &event) {
	if (event.mods == ModifierKeys::leftButtonModifier && isEnabled
		&& onItemSelected != nullptr) {
		onItemSelected(item.id);
	}
}

//==============================================================================
// SearchBoxPopup
SearchBoxPopup::SearchBoxPopup(SearchBox &popupOwner) : owner(popupOwner) {
	searchEditor.setMultiLine(false, false);
	searchEditor.setJustification(Justification::centredLeft);
	searchEditor.setIndents(10, 0);
	searchEditor.setFont(AppLookAndFeel::defaultFontSize);
	searchEditor.setColour(TextEditor::focusedOutlineColourId,
						   Colours::transparentWhite);
	searchEditor.setTextToShowWhenEmpty("Type to search NDI sources...",
										juce::Colours::grey);
	searchEditor.onTextChange = [this] { buildMenu(searchEditor.getText()); };
	addAndMakeVisible(searchEditor);

	menuViewer.setViewedComponent(&menuSheet, false);
	menuViewer.setScrollBarsShown(true, false);
	addAndMakeVisible(menuViewer);

	setAlwaysOnTop(true);

	setLookAndFeel(&lookAndFeel);
}

SearchBoxPopup::~SearchBoxPopup() { setLookAndFeel(nullptr); }

void SearchBoxPopup::resized() {
	const auto width = owner.getWidth();
	const auto itemHeight = AppLookAndFeel::buttonHeight;
	const auto numItems = activeItems.size();
	const auto padding = AppLookAndFeel::padding;

	// Set component bounds
	const auto bounds = getParentComponent()->getLocalBounds();
	setBounds(bounds);

	const auto ownerBounds =
		getParentComponent()->getLocalArea(&owner, owner.getLocalBounds());

	// Set search editor bounds
	searchEditor.setBounds(ownerBounds.withTrimmedRight(35));

	// Set menu viewer bounds
	const int viewerHeight =
		jmin(bounds.getBottom() - ownerBounds.getBottom() - padding,
			 itemHeight * numItems);
	menuViewer.setBounds(juce::Rectangle<int>(120, 140, width, viewerHeight));

	// Set menu sheet size
	menuSheet.setSize(width, itemHeight * numItems);
	const auto itemBounds = juce::Rectangle<int>(width, itemHeight);

	for (int i = 0; i < numItems; ++i)
		activeItems[i]->setBounds(itemBounds.withY(i * itemHeight));
}

void SearchBoxPopup::paint(Graphics &g) {
	g.setColour(findColour(PopupMenu::backgroundColourId));
	g.fillRect(menuViewer.getBounds());
}

void SearchBoxPopup::show() {
	owner.getTopLevelComponent()->addAndMakeVisible(this);
	buildMenu();
}

void SearchBoxPopup::hide() {
	getParentComponent()->removeChildComponent(this);
	searchEditor.clear();
}

void SearchBoxPopup::add(Item newItem, bool shouldBeTicked,
						 bool shouldBeEnabled) {
	// Can't add id 0
	jassert(newItem.id != 0);

	if (newItem.id != 0) {
		auto *newComponent = activeItems.add(std::make_unique<ItemComponent>(
			newItem, shouldBeTicked, shouldBeEnabled));

		newComponent->onItemSelected = [this](int idSelected) {
			jassert(idSelected != 0);

			if (idSelected != 0)
				owner.setSelectedId(idSelected);

			owner.hidePopup();
		};

		menuSheet.addAndMakeVisible(newComponent);
	}
}

void SearchBoxPopup::clear() { activeItems.clear(); }

void SearchBoxPopup::mouseDown(const MouseEvent & /*event*/) {
	owner.hidePopup();
}

void SearchBoxPopup::buildMenu(StringRef searchTerm) {
	clear();

	if (owner.menuItems.isEmpty()) {
		add({owner.getTextWhenNoChoicesAvailable(), 1}, false, false);
		resized();
		return;
	}

	const auto selectedId = owner.getSelectedId();

	for (auto &item : owner.menuItems) {
		if (item.text.containsIgnoreCase(searchTerm)) {
			bool shouldBeTicked = (item.id == selectedId);
			add(item, shouldBeTicked);
		}
	}

	if (activeItems.size() == 0) {
		add({"No matching NDI sources", 1}, false, false);
	}

	resized();
}

//==============================================================================
// SearchBox
SearchBox::SearchBox(const String &name)
	: Component(name), noChoicesMessage(TRANS("(no choices)")) {
	setRepaintsOnMouseActivity(true);
	lookAndFeelChanged();
	currentId.addListener(this);
}

SearchBox::~SearchBox() {
	currentId.removeListener(this);
	hidePopup();
	label.reset();
}

//==============================================================================
void SearchBox::setJustificationType(Justification justification) {
	label->setJustificationType(justification);
}

Justification SearchBox::getJustificationType() const noexcept {
	return label->getJustificationType();
}

void SearchBox::setTooltip(const String &newTooltip) {
	SettableTooltipClient::setTooltip(newTooltip);
	label->setTooltip(newTooltip);
}

//==============================================================================
void SearchBox::addItem(const String &newItemText, int newItemId) {
	// you can't add empty strings to the list..
	jassert(newItemText.isNotEmpty());

	// IDs must be non-zero, as zero is used to indicate a lack of selection.
	jassert(newItemId != 0);

	// you shouldn't use duplicate item IDs!
	jassert(getItemForId(newItemId) == nullptr);

	if (newItemText.isNotEmpty() && newItemId != 0) {
		menuItems.add({newItemText, newItemId});
	}
}

void SearchBox::clear(const NotificationType notification) {
	menuItems.clear();

	setSelectedItemIndex(-1, notification);
}

//==============================================================================
const SearchBoxPopup::Item *SearchBox::getItemForId(int itemId) const noexcept {
	if (itemId != 0) {
		for (auto &item : menuItems) {
			if (item.id == itemId)
				return &item;
		}
	}

	return nullptr;
}

const SearchBoxPopup::Item *
SearchBox::getItemForIndex(const int index) const noexcept {
	if (menuItems[index].id == 0)
		return nullptr;

	return &menuItems.getReference(index);
}

int SearchBox::getNumItems() const noexcept { return menuItems.size(); }

String SearchBox::getItemText(const int index) const {
	if (auto *item = getItemForIndex(index))
		return item->text;

	return {};
}

int SearchBox::getItemId(const int index) const noexcept {
	if (auto *item = getItemForIndex(index))
		return item->id;

	return 0;
}

int SearchBox::indexOfItemId(const int itemId) const noexcept {
	if (itemId != 0) {
		for (int i = 0; i < menuItems.size(); ++i) {
			if (menuItems[i].id == itemId)
				return i;
		}
	}

	return -1;
}

//==============================================================================
int SearchBox::getSelectedItemIndex() const {
	auto index = indexOfItemId(currentId.getValue());

	if (getText() != getItemText(index))
		index = -1;

	return index;
}

void SearchBox::setSelectedItemIndex(const int				index,
									 const NotificationType notification) {
	setSelectedId(getItemId(index), notification);
}

int SearchBox::getSelectedId() const noexcept {
	if (auto *item = getItemForId(currentId.getValue()))
		if (getText() == item->text)
			return item->id;

	return 0;
}

void SearchBox::setSelectedId(const int				 newItemId,
							  const NotificationType notification) {
	auto *item = getItemForId(newItemId);
	auto  newItemText = item != nullptr ? item->text : String();

	if (lastCurrentId != newItemId || label->getText() != newItemText) {
		setLabelText(newItemText);
		lastCurrentId = newItemId;
		currentId = newItemId;

		repaint(); // for the benefit of the 'none selected' text

		sendChange(notification);
	}
}

bool SearchBox::selectIfEnabled(const int index) {
	if (auto *item = getItemForIndex(index)) {
		setSelectedItemIndex(index);
		return true;
	}

	return false;
}

bool SearchBox::nudgeSelectedItem(int delta) {
	for (int i = getSelectedItemIndex() + delta;
		 isPositiveAndBelow(i, getNumItems()); i += delta)
		if (selectIfEnabled(i))
			return true;

	return false;
}

void SearchBox::valueChanged(Value &) {
	if (lastCurrentId != (int)currentId.getValue())
		setSelectedId(currentId.getValue());
}

//==============================================================================
String SearchBox::getText() const { return label->getText(); }

void SearchBox::showEditor() { label->showEditor(); }

//==============================================================================
void SearchBox::setTextWhenNothingSelected(const String &newMessage) {
	if (textWhenNothingSelected != newMessage) {
		textWhenNothingSelected = newMessage;
		repaint();
	}
}

String SearchBox::getTextWhenNothingSelected() const {
	return textWhenNothingSelected;
}

void SearchBox::setTextWhenNoChoicesAvailable(const String &newMessage) {
	noChoicesMessage = newMessage;
}

String SearchBox::getTextWhenNoChoicesAvailable() const {
	return noChoicesMessage;
}

//==============================================================================
void SearchBox::paint(Graphics &g) {
	drawSearchBox(g, getWidth(), getHeight(), isButtonDown, label->getRight(),
				  0, getWidth() - label->getRight(), getHeight(), *this);

	if (textWhenNothingSelected.isNotEmpty() && label->getText().isEmpty()
		&& !label->isBeingEdited())
		drawSearchBoxTextWhenNothingSelected(g, *this, *label);
}

void SearchBox::resized() {
	if (getHeight() > 0 && getWidth() > 0)
		positionSearchBoxText(*this, *label);
}

void SearchBox::enablementChanged() {
	if (!isEnabled())
		hidePopup();

	repaint();
}

void SearchBox::colourChanged() { lookAndFeelChanged(); }

void SearchBox::parentHierarchyChanged() { lookAndFeelChanged(); }

void SearchBox::lookAndFeelChanged() {
	repaint();

	{
		std::unique_ptr<Label> newLabel(createSearchBoxTextBox(*this));
		jassert(newLabel != nullptr);

		if (label != nullptr) {
			newLabel->setJustificationType(label->getJustificationType());
			newLabel->setTooltip(label->getTooltip());
			newLabel->setText(label->getText(), dontSendNotification);
		}

		std::swap(label, newLabel);
	}

	addAndMakeVisible(label.get());

	setWantsKeyboardFocus(true);
	label->setEditable(false, false);
	label->setMinimumHorizontalScale(1.0f);
	label->setBorderSize(BorderSize<int>(5, 11, 5, 11));
	label->onTextChange = [this] { triggerAsyncUpdate(); };
	label->addMouseListener(this, false);
	label->setAccessible(false);

	label->setColour(Label::backgroundColourId, Colours::transparentBlack);
	label->setColour(Label::textColourId, findColour(ComboBox::textColourId));

	label->setColour(TextEditor::textColourId,
					 findColour(ComboBox::textColourId));
	label->setColour(TextEditor::backgroundColourId, Colours::transparentBlack);
	label->setColour(TextEditor::highlightColourId,
					 findColour(TextEditor::highlightColourId));
	label->setColour(TextEditor::outlineColourId, Colours::transparentBlack);

	resized();
}

//==============================================================================
bool SearchBox::keyPressed(const KeyPress &key) {
	if (key == KeyPress::upKey || key == KeyPress::leftKey) {
		nudgeSelectedItem(-1);
		return true;
	}

	if (key == KeyPress::downKey || key == KeyPress::rightKey) {
		nudgeSelectedItem(1);
		return true;
	}

	if (key == KeyPress::returnKey) {
		showPopupIfNotActive();
		return true;
	}

	return false;
}

bool SearchBox::keyStateChanged(const bool isKeyDown) {
	// only forward key events that aren't used by this component
	return isKeyDown
		   && (KeyPress::isKeyCurrentlyDown(KeyPress::upKey)
			   || KeyPress::isKeyCurrentlyDown(KeyPress::leftKey)
			   || KeyPress::isKeyCurrentlyDown(KeyPress::downKey)
			   || KeyPress::isKeyCurrentlyDown(KeyPress::rightKey));
}

//==============================================================================
void SearchBox::focusGained(FocusChangeType) { repaint(); }
void SearchBox::focusLost(FocusChangeType) { repaint(); }

//==============================================================================
void SearchBox::showPopupIfNotActive() {
	if (!menuActive)
		showPopup();
}

void SearchBox::hidePopup() {
	if (menuActive) {
		menuActive = false;
		popupMenu.hide();
		repaint();
	}
}

void SearchBox::showPopup() {
	menuActive = true;
	popupMenu.show();
}

//==============================================================================
void SearchBox::mouseDown(const MouseEvent &e) {
	beginDragAutoRepeat(300);

	isButtonDown = isEnabled() && !e.mods.isPopupMenu();

	if (isButtonDown && (e.eventComponent == this || !label->isEditable()))
		showPopupIfNotActive();
}

void SearchBox::mouseDrag(const MouseEvent &e) {
	beginDragAutoRepeat(50);

	if (isButtonDown && e.mouseWasDraggedSinceMouseDown())
		showPopupIfNotActive();
}

void SearchBox::mouseUp(const MouseEvent &e2) {
	if (isButtonDown) {
		isButtonDown = false;
		repaint();

		auto e = e2.getEventRelativeTo(this);

		if (reallyContains(e.getPosition(), true)
			&& (e2.eventComponent == this || !label->isEditable())) {
			showPopupIfNotActive();
		}
	}
}

void SearchBox::mouseWheelMove(const MouseEvent		   &e,
							   const MouseWheelDetails &wheel) {
	if (!menuActive && scrollWheelEnabled && e.eventComponent == this
		&& wheel.deltaY != 0.0f) {
		mouseWheelAccumulator += wheel.deltaY * 5.0f;

		while (mouseWheelAccumulator > 1.0f) {
			mouseWheelAccumulator -= 1.0f;
			nudgeSelectedItem(-1);
		}

		while (mouseWheelAccumulator < -1.0f) {
			mouseWheelAccumulator += 1.0f;
			nudgeSelectedItem(1);
		}
	} else {
		Component::mouseWheelMove(e, wheel);
	}
}

void SearchBox::setScrollWheelEnabled(bool enabled) noexcept {
	scrollWheelEnabled = enabled;
}

//==============================================================================
void SearchBox::addListener(SearchBox::Listener *l) { listeners.add(l); }
void SearchBox::removeListener(SearchBox::Listener *l) { listeners.remove(l); }

void SearchBox::handleAsyncUpdate() {
	Component::BailOutChecker checker(this);
	listeners.callChecked(checker,
						  [this](Listener &l) { l.searchBoxChanged(this); });

	if (checker.shouldBailOut())
		return;

	if (onChange != nullptr)
		onChange();

	if (auto *handler = getAccessibilityHandler())
		handler->notifyAccessibilityEvent(AccessibilityEvent::valueChanged);
}

void SearchBox::sendChange(const NotificationType notification) {
	if (notification != dontSendNotification)
		triggerAsyncUpdate();

	if (notification == sendNotificationSync)
		handleUpdateNowIfNeeded();
}

void SearchBox::setLabelText(const String &newText) {
	label->setText(newText, dontSendNotification);

	int textWidth = getSearchBoxFont(*this).getStringWidth(newText);
	int availableWidth =
		label->getWidth() - label->getBorderSize().getLeftAndRight();

	if (textWidth > availableWidth)
		setTooltip(newText);
	else
		setTooltip("");
}
