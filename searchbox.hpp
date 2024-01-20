#pragma once
#include <JuceHeader.h>

class SearchBox;

class SearchBoxPopup  : public Component
{
public:
    explicit SearchBoxPopup (SearchBox& popupOwner);

    //==========================================================================
    void resized() override;
    void paint (Graphics& g) override;

    //==========================================================================
    void show();
    void hide();

    //==========================================================================
    struct Item
    {
        String text;
        int id = 0;
    };

    //======================================================================
    void mouseDown (const MouseEvent& event) override;

private:
    SearchBox& owner;
    TextEditor searchEditor;
    class ItemComponent;
    OwnedArray<ItemComponent> activeItems;
    Component menuSheet;
    Viewport menuViewer;

    //==========================================================================
    void add (Item newItem, bool shouldBeTicked, bool shouldBeEnabled = true);
    void clear();
    void buildMenu (StringRef searchTerm = "");

    //==========================================================================
    class ItemComponent  : public Component,
                           public SettableTooltipClient
    {
    public:
        ItemComponent (Item it, bool shouldBeTicked, bool shouldBeEnabled = true);

        //======================================================================
        void resized() override;
        void paint (Graphics& g) override;

        //======================================================================
        std::function<void(int)> onItemSelected { nullptr };

        //======================================================================
        void mouseDown (const MouseEvent& event) override;

    private:
        const Item item;
        const bool isTicked;
        const bool isEnabled;
        const Font font;
        const BorderSize<int> textBorder {5, 1, 5, 20};
        static constexpr int tickSize {30};

        //======================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ItemComponent)
    };

    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SearchBoxPopup)
};

//==============================================================================
class SearchBox  : public Component,
                   public SettableTooltipClient,
                   public Value::Listener,
                   private AsyncUpdater
{
public:
    //==============================================================================
    /** Creates a search-box.

        On construction, the text field will be empty, so you should call the
        setSelectedId() or setText() method to choose the initial value before
        displaying it.

        @param componentName    the name to set for the component (see Component::setName())
    */
    explicit SearchBox (const String& componentName = {});

    /** Destructor. */
    ~SearchBox() override;

    //==============================================================================
    /** Sets the style of justification to be used for positioning the text.

        The default is Justification::centredLeft. The text is displayed using a
        Label component inside the SearchBox.
    */
    void setJustificationType (Justification justification);

    /** Returns the current justification for the text box.
        @see setJustificationType
    */
    Justification getJustificationType() const noexcept;

    //==============================================================================
    /** Adds an item to be shown in the drop-down list.

        @param newItemText      the text of the item to show in the list
        @param newItemId        an associated ID number that can be set or retrieved - see
                                getSelectedId() and setSelectedId(). Note that this value can not
                                be 0!
        @see setItemEnabled, addSeparator, addSectionHeading, getNumItems, getItemText, getItemId
    */
    void addItem (const String& newItemText, int newItemId);

    /** Removes all the items from the drop-down list.

        If this call causes the content to be cleared, and a change-message
        will be broadcast according to the notification parameter.

        @see addItem, getNumItems
    */
    void clear (NotificationType notification = sendNotificationAsync);

    /** Returns the number of items that have been added to the list.

        Note that this doesn't include headers or separators.
    */
    int getNumItems() const noexcept;

    /** Returns the text for one of the items in the list.
        Note that this doesn't include headers or separators.
        @param index    the item's index from 0 to (getNumItems() - 1)
    */
    String getItemText (int index) const;

    /** Returns the ID for one of the items in the list.
        Note that this doesn't include headers or separators.
        @param index    the item's index from 0 to (getNumItems() - 1)
    */
    int getItemId (int index) const noexcept;

    /** Returns the index in the list of a particular item ID.
        If no such ID is found, this will return -1.
    */
    int indexOfItemId (int itemId) const noexcept;

    //==============================================================================
    /** Returns the ID of the item that's currently shown in the box.

        If no item is selected, or if the text is editable and the user
        has entered something which isn't one of the items in the list, then
        this will return 0.

        @see setSelectedId, getSelectedItemIndex, getText
    */
    int getSelectedId() const noexcept;

    /** Returns a Value object that can be used to get or set the selected item's ID.

        You can call Value::referTo() on this object to make the combo box control
        another Value object.
    */
    Value& getSelectedIdAsValue()                       { return currentId; }

    /** Sets one of the items to be the current selection.

        This will set the SearchBox's text to that of the item that matches
        this ID.

        @param newItemId        the new item to select
        @param notification     determines the type of change notification that will
                                be sent to listeners if the value changes
        @see getSelectedId, setSelectedItemIndex, setText
    */
    void setSelectedId (int newItemId,
                        NotificationType notification = sendNotificationAsync);

    //==============================================================================
    /** Returns the index of the item that's currently shown in the box.

        If no item is selected, or if the text is editable and the user
        has entered something which isn't one of the items in the list, then
        this will return -1.

        @see setSelectedItemIndex, getSelectedId, getText
    */
    int getSelectedItemIndex() const;

    /** Sets one of the items to be the current selection.

        This will set the SearchBox's text to that of the item at the given
        index in the list.

        @param newItemIndex     the new item to select
        @param notification     determines the type of change notification that will
                                be sent to listeners if the value changes
        @see getSelectedItemIndex, setSelectedId, setText
    */
    void setSelectedItemIndex (int newItemIndex,
                               NotificationType notification = sendNotificationAsync);

    //==============================================================================
    /** Returns the text that is currently shown in the search-box's text field.

        If the SearchBox has editable text, then this text may have been edited
        by the user; otherwise it will be one of the items from the list, or
        possibly an empty string if nothing was selected.

        @see setText, getSelectedId, getSelectedItemIndex
    */
    String getText() const;

    /** Programmatically opens the text editor to allow the user to edit the current item.

        This is the same effect as when the box is clicked-on.
        @see Label::showEditor();
    */
    void showEditor();

    /** Pops up the combo box's list.
        This is virtual so that you can override it with your own custom popup
        mechanism if you need some really unusual behaviour.
    */
    virtual void showPopup();

    /** Hides the combo box's popup list, if it's currently visible. */
    void hidePopup();

    /** Returns true if the popup menu is currently being shown. */
    bool isPopupActive() const noexcept                 { return menuActive; }

    //==============================================================================
    /**
        A class for receiving events from a SearchBox.

        You can register a SearchBox::Listener with a SearchBox using the SearchBox::addListener()
        method, and it will be called when the selected item in the box changes.

        @see SearchBox::addListener, SearchBox::removeListener
    */
    class Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() = default;

        /** Called when a SearchBox has its selected item changed. */
        virtual void searchBoxChanged (SearchBox* searchBoxThatHasChanged) = 0;
    };

    /** Registers a listener that will be called when the box's content changes. */
    void addListener (Listener* listener);

    /** Deregisters a previously-registered listener. */
    void removeListener (Listener* listener);

    //==============================================================================
    /** You can assign a lambda to this callback object to have it called when the selected ID is changed. */
    std::function<void()> onChange;

    //==============================================================================
    /** Sets a message to display when there is no item currently selected.
        @see getTextWhenNothingSelected
    */
    void setTextWhenNothingSelected (const String& newMessage);

    /** Returns the text that is shown when no item is selected.
        @see setTextWhenNothingSelected
    */
    String getTextWhenNothingSelected() const;

    /** Sets the message to show when there are no items in the list, and the user clicks
        on the drop-down box.

        By default it just says "no choices", but this lets you change it to something more
        meaningful.
    */
    void setTextWhenNoChoicesAvailable (const String& newMessage);

    /** Returns the text shown when no items have been added to the list.
        @see setTextWhenNoChoicesAvailable
    */
    String getTextWhenNoChoicesAvailable() const;

    /** This can be used to allow the scroll-wheel to nudge the chosen item.
        By default it's disabled, and I'd recommend leaving it disabled if there's any
        chance that the control might be inside a scrollable list or viewport.
    */
    void setScrollWheelEnabled (bool enabled) noexcept;

    //==============================================================================
    /** @internal */
    void enablementChanged() override;
    /** @internal */
    void colourChanged() override;
    /** @internal */
    void focusGained (Component::FocusChangeType) override;
    /** @internal */
    void focusLost (Component::FocusChangeType) override;
    /** @internal */
    void handleAsyncUpdate() override;
    /** @internal */
    String getTooltip() override                        { return label->getTooltip(); }
    /** @internal */
    void mouseDown (const MouseEvent&) override;
    /** @internal */
    void mouseDrag (const MouseEvent&) override;
    /** @internal */
    void mouseUp (const MouseEvent&) override;
    /** @internal */
    void mouseWheelMove (const MouseEvent&, const MouseWheelDetails&) override;
    /** @internal */
    void lookAndFeelChanged() override;
    /** @internal */
    void paint (Graphics&) override;
    /** @internal */
    void resized() override;
    /** @internal */
    bool keyStateChanged (bool) override;
    /** @internal */
    bool keyPressed (const KeyPress&) override;
    /** @internal */
    void valueChanged (Value&) override;
    /** @internal */
    void parentHierarchyChanged() override;

private:
    //==========================================================================
    Array<SearchBoxPopup::Item> menuItems;
    SearchBoxPopup popupMenu { *this };

    Value currentId;
    int lastCurrentId = 0;
    bool isButtonDown = false, menuActive = false, scrollWheelEnabled = false;
    float mouseWheelAccumulator = 0;
    ListenerList<Listener> listeners;
    std::unique_ptr<Label> label;
    String textWhenNothingSelected, noChoicesMessage;

    //==============================================================================
    /** Gives the SearchBox a tooltip. */
    void setTooltip (const String& newTooltip) override;

    //==========================================================================
    const SearchBoxPopup::Item* getItemForId (int) const noexcept;
    const SearchBoxPopup::Item* getItemForIndex (int) const noexcept;
    bool selectIfEnabled (int index);
    bool nudgeSelectedItem (int delta);
    void sendChange (NotificationType);
    void showPopupIfNotActive();
    void setLabelText (const String& newText);

    friend class SearchBoxPopup;
    
    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SearchBox)
};
