/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2014 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __ONLINEPSTHCONFIGWINDOW_H_F0BD2DD9__
#define __ONLINEPSTHCONFIGWINDOW_H_F0BD2DD9__


#include <EditorHeaders.h>

#include "OnlinePSTH.h"

class OnlinePSTHEditor;
class PopupConfigurationWindow;
class OnlinePSTHTableModel;

/** 
*   Table component used to edit Trigger Source names
*/
class EditableTextCustomComponent : 
    public juce::Label,
    public Label::Listener
{
public:

    /** Constructor */
    EditableTextCustomComponent(OnlinePSTH* processor_, TriggerSource* source_, bool acquisitionIsActive_)
        : source(source_),
          processor(processor_),
          acquisitionIsActive(acquisitionIsActive_)
    {
        setEditable(false, true, false);
        addListener(this);
        setColour(Label::textColourId, Colours::white);
        setColour(Label::textWhenEditingColourId, Colours::yellow);
        setColour(TextEditor::highlightedTextColourId, Colours::yellow);
    }

    /** Responds to button clicks */
    void mouseDown(const juce::MouseEvent& event) override;
    
    /** Called when the label is updated */
    void labelTextChanged(Label* label) override;

    /** Sets row and column */
    void setRowAndColumn(const int newRow, const int newColumn);
    
    int row;
    TriggerSource* source;

private:
    
    OnlinePSTH* processor;
    bool acquisitionIsActive;
    int columnId;
};

/**
*   Table component used to edit the TTL line
*   used by a Trigger Source
*/
class LineSelectorCustomComponent : 
    public juce::Label,
    public PopupChannelSelector::Listener
{
public:

    /** Constructor */
    LineSelectorCustomComponent(TriggerSource* source_, bool acquisitionIsActive_)
        : source(source_),
          acquisitionIsActive(acquisitionIsActive_)
    {
        setEditable(false, false, false);
    }

    /** Responds to mouse clicks */
    void mouseDown(const juce::MouseEvent& event) override;
    
    /** Callback for changes in PopupChannelSelector */
    void channelStateChanged(Array<int> newChannels) override
    {

        if (newChannels.size() > 0)
		{
			source->processor->setTriggerSourceLine(source, newChannels[0]);
			setText("TTL " + String(newChannels[0] + 1), dontSendNotification);
        }
        else {
            source->processor->setTriggerSourceLine(source, -1);
            setText("NONE", dontSendNotification);
        }
    
    }
    
    /** Sets row and column */
    void setRowAndColumn(const int newRow, const int newColumn);
    
    int row;
    TriggerSource* source;

private:
    
    int columnId;
    juce::Colour textColour;
    bool acquisitionIsActive;
};


/**
*   Table component used to select the trigger type
*   (TTL, MSG, or TTL + MSG) for a Spike Channel.
*/
class TriggerTypeSelectorCustomComponent : public Component
{
public:

    /** Constructor */
    TriggerTypeSelectorCustomComponent(TriggerSource* source_, bool acquisitionIsActive_)
        : acquisitionIsActive(acquisitionIsActive_), source(source_)
    {
		assert(source != nullptr);
    }

    /** Handles mouse clicks */
    void mouseDown(const juce::MouseEvent& event) override;
    
    /** Renders the waveform type icon */
    void paint(Graphics& g) override;
    
    /** Sets row and column */
    void setRowAndColumn(const int newRow, const int newColumn);

    /** Sets a pointer to the OnlinePSTHTableModel object */
    void setTableModel(OnlinePSTHTableModel* table_) { table = table_; };

    int row;
    TriggerSource* source;

private:
    
    OnlinePSTHTableModel* table;
    int columnId;
    juce::Colour textColour;
    bool acquisitionIsActive;
};

/**
*   Table component used to display colour of each condition
*/
class ColourDisplayCustomComponent : public Component,
    public ChangeListener
{
public:

    /** Constructor */
    ColourDisplayCustomComponent(TriggerSource* source_, bool acquisitionIsActive_)
        : acquisitionIsActive(acquisitionIsActive_), source(source_)
    {
        assert(source != nullptr);
    }

    /** Handles mouse clicks */
    void mouseDown(const juce::MouseEvent& event) override;

    /** Renders the color icon */
    void paint(Graphics& g) override;

    /** Sets row and column */
    void setRowAndColumn(const int newRow, const int newColumn);

    /** Sets a pointer to the OnlinePSTHTableModel object */
    void setTableModel(OnlinePSTHTableModel* table_) { table = table_; };

    /** Responds to color changes*/
    void changeListenerCallback(ChangeBroadcaster* source);

    int row;
    TriggerSource* source;

private:

    OnlinePSTHTableModel* table;
    int columnId;
    bool acquisitionIsActive;
};


/**
*   Table component used to delete electrodes
*/
class DeleteButtonCustomComponent : public Component
{
public:

    /** Constructor */
    DeleteButtonCustomComponent(bool acquisitionIsActive_)
        : acquisitionIsActive(acquisitionIsActive_)
    {
    }

    /** Handles mouse click events */
    void mouseDown(const juce::MouseEvent& event) override;

    /** Renders the delete icon */
    void paint(Graphics& g) override;

    /** Sets row and column */
    void setRowAndColumn(const int newRow, const int newColumn);

    /** Sets a pointer to the SpikeDetectorTableModel object */
    void setTableModel(OnlinePSTHTableModel* table_) { table = table_; };

    int row;

private:
    OnlinePSTHTableModel* table;
    int columnId;
    bool acquisitionIsActive;
};

/**
*   TableListBoxModel used for editing Spike Channel parameters
*/
class OnlinePSTHTableModel : public TableListBoxModel
{

public:

    /** Constructor */
    OnlinePSTHTableModel(OnlinePSTHEditor* editor,
                            PopupConfigurationWindow* owner,
                            bool acquisitionIsActive);

    /** Column types*/
    enum Columns {
        INDEX = 1,
        NAME,
        LINE,
        TYPE,
        COLOUR,
        DELETE
    };

    /** Callback when a cell is clicked (not a sub-component) */
    void cellClicked(int rowNumber, int columnId, const MouseEvent& event) override;

    /** Called whenever a cell needs to be updated; creates custom components inside each cell*/
    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected,
        Component* existingComponentToUpdate) override;

    /** Returns the number of rows in the table */
    int getNumRows() override;
    
    /** Updates the underlying TriggerSource objects */
    void update(Array<TriggerSource*> triggerSources);

    /** Determines row colors */
    void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    
    /** Deletes the SpikeChannel objects associated with each row */
    void deleteSelectedRows(int rowThatWasClicked);

    /** Paints the INDEX and TYPE columns*/
    void paintCell(Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    Array<TriggerSource*> triggerSources;
    TableListBox* table;
private:

    OnlinePSTHEditor* editor;
    
    PopupConfigurationWindow* owner;

    bool acquisitionIsActive;

};

/** 
    Interface to generate new Spike Channels
*/
class TriggerSourceGenerator : 
    public Component,
    public PopupChannelSelector::Listener,
    public Button::Listener,
    public Label::Listener
{
public:

    /** Constructor */
    TriggerSourceGenerator(OnlinePSTHEditor* editor,
                          PopupConfigurationWindow* window,
                          int channelCount,
                          bool acquisitionIsActive);

    /** Destructor*/
    ~TriggerSourceGenerator() { }

    /** Responds to changes in the PopupChannelSelector*/
    void channelStateChanged(Array<int> selectedChannels);

    /** Responds to button clicks*/
    void buttonClicked(Button* button);

    /** Responds to Label */
    void labelTextChanged(Label* label);

    /** Draws border and text */
    void paint(Graphics& g);

private:

    OnlinePSTHEditor* editor;
    PopupConfigurationWindow* window;

    int channelCount;
    String lastLabelValue;
    Array<int> startChannels;

    std::unique_ptr<Label> triggerSourceCountLabel;
    std::unique_ptr<ComboBox> triggerTypeSelector;
    std::unique_ptr<Button> channelSelectorButton;
    std::unique_ptr<UtilityButton> plusButton;
};

/**
*   Popup window used to edit Spike Channel settings
*/
class PopupConfigurationWindow : public Component,
    public ScrollBar::Listener
{

public:
    
    /** Constructor */
    PopupConfigurationWindow(OnlinePSTHEditor* editor, 
                             Array<TriggerSource*> triggerSources,
                             bool acquisitionIsActive);

    /** Destructor */
    ~PopupConfigurationWindow() { }

    /** Updates the window with a new set of Spike Channels*/
    void update(Array<TriggerSource*> triggerSources);

    /** Custom table header component (not currently used)*/
    //std::unique_ptr<TableHeaderComponent> tableHeader;

    /** Custom table model*/
    std::unique_ptr<OnlinePSTHTableModel> tableModel;

    /** Custom list box for Spike Channel settings*/
    std::unique_ptr<TableListBox> table;
    
    /** Listens for viewport scrolling */
    void scrollBarMoved(ScrollBar* scrollBar, double newRangeStart);

private:
    OnlinePSTHEditor* editor;

    std::unique_ptr<TriggerSourceGenerator> triggerSourceGenerator;
    
    std::unique_ptr<Viewport> viewport;

    int scrollDistance = 0;
    
    bool updating = false;
};


#endif  // __SPIKEDETECTORCONFIGWINDOW_H_F0BD2DD9__
