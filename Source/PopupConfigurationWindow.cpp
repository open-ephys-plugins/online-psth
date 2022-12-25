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

#include "PopupConfigurationWindow.h"

#include "OnlinePSTHEditor.h"
#include <stdio.h>

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void EditableTextCustomComponent::mouseDown(const MouseEvent& event)
{
    //owner->selectRowsBasedOnModifierKeys(row, event.mods, false);
    Label::mouseDown(event);
}

void EditableTextCustomComponent::setRowAndColumn(const int newRow, const int newColumn)
{
    row = newRow;
    columnId = newColumn;
    setText(source->name, dontSendNotification);
    
}

void EditableTextCustomComponent::labelTextChanged(Label* label)
{
    String candidateName = label->getText();

    String newName = processor->ensureUniqueName(candidateName);

    label->setText(newName, dontSendNotification);

    processor->setTriggerSourceName(source, newName);
}


void LineSelectorCustomComponent::mouseDown(const juce::MouseEvent& event)
{
    if (acquisitionIsActive)
        return;

    std::vector<bool> channelStates;

    for (int i = 0; i < 16; i++)
    {
		if (i == source->line)
			channelStates.push_back(true);
		else
			channelStates.push_back(false);
    }

    auto* channelSelector = new PopupChannelSelector(this, channelStates);
    
    channelSelector->setChannelButtonColour(Colour(0, 174, 239));
    channelSelector->setMaximumSelectableChannels(1);

     CallOutBox& myBox
        = CallOutBox::launchAsynchronously(std::unique_ptr<Component>(channelSelector),
            getScreenBounds(),
            nullptr);
}
    
void LineSelectorCustomComponent::setRowAndColumn(const int newRow, const int newColumn)
{
    
    if (source->line > -1)
    {
		String lineName = "TTL " + String(source->line + 1);
		setText(lineName, dontSendNotification);
	}
	else
	{
		setText("NONE", dontSendNotification);
    }

}


void TriggerTypeSelectorCustomComponent::mouseDown(const juce::MouseEvent& event)
{
    if (acquisitionIsActive)
        return;

    TriggerType newType;

    switch (source->type)
    {
    case TriggerType::TTL_TRIGGER:
        newType = TriggerType::MSG_TRIGGER;
        break;
    case TriggerType::MSG_TRIGGER:
        newType = TriggerType::TTL_AND_MSG_TRIGGER;
        break;
    case TriggerType::TTL_AND_MSG_TRIGGER:
        newType = TriggerType::TTL_TRIGGER;
        break;
    default:
        break;
    }

    source->processor->setTriggerSourceTriggerType(source, newType);
    
    repaint();
}

    
void TriggerTypeSelectorCustomComponent::paint(Graphics& g)
{

    int width = getWidth();
	int height = getHeight();
    
    switch (source->type)
    {
    case TriggerType::TTL_TRIGGER:
        g.setColour(Colours::blue);
        g.fillRoundedRectangle(6, 6, width - 12, height - 12, 4);
        g.setColour(Colours::white);
        g.drawText("TTL", 4, 4, width - 8, height - 8, Justification::centred);
        break;
    case TriggerType::MSG_TRIGGER:
        g.setColour(Colours::violet);
        g.fillRoundedRectangle(6, 6, width - 12, height - 12, 4);
        g.setColour(Colours::white);
        g.drawText("MSG", 4, 4, width - 8, height - 8, Justification::centred);
        break;
    case TriggerType::TTL_AND_MSG_TRIGGER:
        g.setColour(Colours::blueviolet);
        g.fillRoundedRectangle(6, 6, width - 12, height - 12, 4);
        g.setColour(Colours::white);
        g.drawText("TTL + MSG", 4, 4, width - 8, height - 8, Justification::centred);
        break;
    default:
        break;
    }

}
    
void TriggerTypeSelectorCustomComponent::setRowAndColumn(const int newRow, const int newColumn)
{
    row = newRow;
    repaint();
}



void DeleteButtonCustomComponent::mouseDown(const juce::MouseEvent& event)
{
    if (acquisitionIsActive)
        return;

    table->deleteSelectedRows(row);
}


void DeleteButtonCustomComponent::paint(Graphics& g)
{

    int width = getWidth();
    int height = getHeight();

    if (acquisitionIsActive)
    {
        g.setColour(Colours::grey);
    }
    else
    {
        g.setColour(Colours::red);
    }

    g.fillEllipse(7, 7, width - 14, height - 14);
    g.setColour(Colours::white);
    g.drawLine(9, height / 2, getWidth() - 9, height / 2, 3.0);
    
    
}

void DeleteButtonCustomComponent::setRowAndColumn(const int newRow, const int newColumn)
{
    row = newRow;
    repaint();
}



OnlinePSTHTableModel::OnlinePSTHTableModel(OnlinePSTHEditor* editor_,
    PopupConfigurationWindow* owner_,
    bool acquisitionIsActive_)
    : editor(editor_), owner(owner_), acquisitionIsActive(acquisitionIsActive_)
{

}

void OnlinePSTHTableModel::cellClicked(int rowNumber, int columnId, const MouseEvent& event)
{
    
    //std::cout << rowNumber << " " << columnId << " : selected " << std::endl;

    //if (event.mods.isRightButtonDown())
    //    std::cout << "Right click!" << std::endl;

}

void OnlinePSTHTableModel::deleteSelectedRows(int rowThatWasClicked)
{
    SparseSet<int> selectedRows = table->getSelectedRows();

    if (!acquisitionIsActive)
    {
        Array<TriggerSource*> channelsToDelete;

        for (int i = 0; i < triggerSources.size(); i++)
        {
            if (selectedRows.contains(i) || i == rowThatWasClicked)
                channelsToDelete.add(triggerSources[i]);
        }

        editor->removeTriggerSources(owner, channelsToDelete);

        table->deselectAllRows();

    }
}

Component* OnlinePSTHTableModel::refreshComponentForCell(int rowNumber,
    int columnId, 
    bool isRowSelected,
    Component* existingComponentToUpdate)
{
    if (columnId == OnlinePSTHTableModel::Columns::NAME)
    {
        auto* textLabel = static_cast<EditableTextCustomComponent*> (existingComponentToUpdate);

        if (textLabel == nullptr)
        {
            OnlinePSTH* processor = (OnlinePSTH*)editor->getProcessor();

            textLabel = new EditableTextCustomComponent(processor, triggerSources[rowNumber],
                                                        acquisitionIsActive);
        }
            
        textLabel->setColour(Label::textColourId, Colours::white);
        textLabel->setRowAndColumn(rowNumber, columnId);
        
        return textLabel;
    }
    else if (columnId == OnlinePSTHTableModel::Columns::LINE)
    {
        auto* linesLabel = static_cast<LineSelectorCustomComponent*> (existingComponentToUpdate);

        if (linesLabel == nullptr)
        {
            linesLabel = new LineSelectorCustomComponent(
                triggerSources[rowNumber],
                acquisitionIsActive);
        }

        linesLabel->setColour(Label::textColourId, Colours::white);
        linesLabel->setRowAndColumn(rowNumber, columnId);
        
        return linesLabel;
    }
    else if (columnId == OnlinePSTHTableModel::Columns::TYPE)
    {
        auto* selectorButton = static_cast<TriggerTypeSelectorCustomComponent*> (existingComponentToUpdate);

        if (selectorButton == nullptr)
        {
            selectorButton = new TriggerTypeSelectorCustomComponent(triggerSources[rowNumber],
                acquisitionIsActive);
        }

        selectorButton->setRowAndColumn(rowNumber, columnId);
        selectorButton->setTableModel(this);
        
        return selectorButton;

    }
    else if (columnId == OnlinePSTHTableModel::Columns::DELETE)
    {
        auto* deleteButton = static_cast<DeleteButtonCustomComponent*> (existingComponentToUpdate);

        if (deleteButton == nullptr)
        {
            deleteButton = new DeleteButtonCustomComponent(
                acquisitionIsActive);
        }

        deleteButton->setRowAndColumn(rowNumber, columnId);
        deleteButton->setTableModel(this);

        return deleteButton;
    }

    jassert(existingComponentToUpdate == nullptr);

    return nullptr;
}

int OnlinePSTHTableModel::getNumRows()
{
    return triggerSources.size();
}

void OnlinePSTHTableModel::update(Array<TriggerSource*> triggerSources_)
{
    triggerSources = triggerSources_;

    table->updateContent();

    /*triggerTypeComponents.clear();
    
    for (int i = 0; i < getNumRows(); i++)
    {

        Component* c = table->getCellComponent(OnlinePSTHTableMode::Columns::THRESHOLD, i);

        if (c == nullptr)
            continue;

        ThresholdSelectorCustomComponent* th = (ThresholdSelectorCustomComponent*)c;

        th->setSpikeChannel(trikg[i]);

        th->repaint();

    }*/

   
}


void OnlinePSTHTableModel::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        if (rowNumber % 2 == 0)
            g.fillAll(Colour(100, 100, 100));
        else
            g.fillAll(Colour(80, 80, 80));

        return;
    }

    if (rowNumber >= triggerSources.size())
        return;

    if (triggerSources[rowNumber]->line > -1)
    {
        if (rowNumber % 2 == 0)
            g.fillAll(Colour(50, 50, 50));
        else
            g.fillAll(Colour(30, 30, 30));
        
        return;
    }

    if (rowNumber % 2 == 0)
        g.fillAll(Colour(90, 50, 50));
    else
        g.fillAll(Colour(60, 30, 30));
        
}

void OnlinePSTHTableModel::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if (columnId == OnlinePSTHTableModel::Columns::INDEX)
    {
        g.setColour(Colours::white);
        g.drawText(String(rowNumber + 1), 4, 0, width, height, Justification::centred);
    }

}


TriggerSourceGenerator::TriggerSourceGenerator(OnlinePSTHEditor* editor_,
    PopupConfigurationWindow* window_,
    int channelCount_, bool acquisitionIsActive)
    : editor(editor_), window(window_), channelCount(16)
{

    lastLabelValue = "1";
    triggerSourceCountLabel = std::make_unique<Label>("Label", lastLabelValue);
    triggerSourceCountLabel->setEditable(true);
    triggerSourceCountLabel->addListener(this);
    triggerSourceCountLabel->setJustificationType(Justification::right);
    triggerSourceCountLabel->setBounds(120, 5, 35, 20);
    triggerSourceCountLabel->setColour(Label::textColourId, Colours::lightgrey);
    addAndMakeVisible(triggerSourceCountLabel.get());

    triggerTypeSelector = std::make_unique<ComboBox>("Trigger Source Type");
    triggerTypeSelector->setBounds(157, 5, 125, 20);
    triggerTypeSelector->addItem("TTL only", TriggerType::TTL_TRIGGER);
    triggerTypeSelector->addItem("Message only", TriggerType::MSG_TRIGGER);
    triggerTypeSelector->addItem("TTL + Message", TriggerType::TTL_AND_MSG_TRIGGER);
    triggerTypeSelector->setSelectedId(TriggerType::TTL_TRIGGER);
    addAndMakeVisible(triggerTypeSelector.get());

    channelSelectorButton = std::make_unique<UtilityButton>("Channels", Font("Default", 16, Font::plain));
    channelSelectorButton->addListener(this);
    channelSelectorButton->setBounds(290, 5, 80, 20);
    addAndMakeVisible(channelSelectorButton.get());

    plusButton = std::make_unique<UtilityButton>("+", Font("Default", 16, Font::plain));
    plusButton->addListener(this);
    plusButton->setBounds(380, 5, 20, 20);
    addAndMakeVisible(plusButton.get());

    if (acquisitionIsActive)
    {
        triggerSourceCountLabel->setEnabled(false);
        triggerTypeSelector->setEnabled(false);
        channelSelectorButton->setEnabled(false);
        plusButton->setEnabled(false);
    }
}



void TriggerSourceGenerator::labelTextChanged(Label* label)
{
    int value = label->getText().getIntValue();

    if (value < 1)
    {
        label->setText(lastLabelValue, dontSendNotification);
        return;
    }

    if (value > 16)
    {
        label->setText("16", dontSendNotification);
    }
    else {
        label->setText(String(value), dontSendNotification);
    }

    lastLabelValue = label->getText();

}


void TriggerSourceGenerator::buttonClicked(Button* button)
{

    if (button == plusButton.get() && channelCount > 0)
    {

        int numTriggerSourcesToAdd = triggerSourceCountLabel->getText().getIntValue();
        TriggerType triggerType = (TriggerType)triggerTypeSelector->getSelectedId();

        //std::cout << "Button clicked! Sending " << startChannels.size() << " start channels " << std::endl;

        if (startChannels.size() == 0)
        {
            Array<int> startChannels;
            
			for (int i = 0; i < numTriggerSourcesToAdd; i++)
			{
				startChannels.add(i);
			}
            
            editor->addTriggerSources(window, startChannels, triggerType);
        }
            
        else
            editor->addTriggerSources(window, startChannels, triggerType);

    }
    else if (button == channelSelectorButton.get() && channelCount > 0)
    {

        std::vector<bool> channelStates;

        int numTriggerChannelsToAdd = triggerSourceCountLabel->getText().getIntValue();

        int skip = 1;

        int channelsAdded = 0;

        for (int i = 0; i < channelCount; i++)
        {
            if (startChannels.size() == 0)
            {
                if (i % skip == 0 && channelsAdded < numTriggerChannelsToAdd)
                {
                    channelStates.push_back(true);
                    channelsAdded++;
                }
                else
                    channelStates.push_back(false);
            }
            else {
                if (startChannels.contains(i))
                    channelStates.push_back(true);
                else
                    channelStates.push_back(false);
            }
            
        }

        auto* channelSelector = new PopupChannelSelector(this, channelStates);

        channelSelector->setChannelButtonColour(Colour(197, 62, 199));

        channelSelector->setMaximumSelectableChannels(numTriggerChannelsToAdd);

        CallOutBox& myBox
            = CallOutBox::launchAsynchronously(std::unique_ptr<Component>(channelSelector),
                button->getScreenBounds(),
                nullptr);
    }

}

void TriggerSourceGenerator::channelStateChanged(Array<int> selectedChannels)
{
    startChannels = selectedChannels;

    //std::cout << "Size of start channels: " << startChannels.size() << std::endl;
}

void TriggerSourceGenerator::paint(Graphics& g)
{
    g.setColour(Colours::darkgrey);
    g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 4.0f);
    g.setColour(Colours::lightgrey);
    g.drawText("ADD CONDITIONS: ", 17, 6, 120, 19, Justification::left, false);
}

PopupConfigurationWindow::PopupConfigurationWindow(OnlinePSTHEditor* editor_, 
                                                   Array<TriggerSource*> triggerSources, 
                                                   bool acquisitionIsActive) 
    : editor(editor_)
{
    //tableHeader.reset(new TableHeaderComponent());

    setSize(310, 40);

    triggerSourceGenerator = std::make_unique<TriggerSourceGenerator>(editor, this,
                16, 
                acquisitionIsActive);
    addAndMakeVisible(triggerSourceGenerator.get());

    tableModel.reset(new OnlinePSTHTableModel(editor, this, acquisitionIsActive));

    table = std::make_unique<TableListBox>("Trigger Source Table", tableModel.get());
    tableModel->table = table.get();
    table->setHeader(std::make_unique<TableHeaderComponent>());

    table->getHeader().addColumn("#", OnlinePSTHTableModel::Columns::INDEX, 30, 30, 30, TableHeaderComponent::notResizableOrSortable);
    table->getHeader().addColumn("Name", OnlinePSTHTableModel::Columns::NAME, 180, 180, 180, TableHeaderComponent::notResizableOrSortable);
    table->getHeader().addColumn("TTL Line", OnlinePSTHTableModel::Columns::LINE, 100, 100, 100, TableHeaderComponent::notResizableOrSortable);
    table->getHeader().addColumn("Type", OnlinePSTHTableModel::Columns::TYPE, 90, 90, 90, TableHeaderComponent::notResizableOrSortable);
    table->getHeader().addColumn(" ", OnlinePSTHTableModel::Columns::DELETE, 30, 30, 30, TableHeaderComponent::notResizableOrSortable);

    table->getHeader().setColour(TableHeaderComponent::ColourIds::backgroundColourId, Colour(240, 240, 240));
    table->getHeader().setColour(TableHeaderComponent::ColourIds::highlightColourId, Colour(50, 240, 240));
    table->getHeader().setColour(TableHeaderComponent::ColourIds::textColourId, Colour(40, 40, 40));

    table->setHeaderHeight(30);
    table->setRowHeight(30);
    table->setMultipleSelectionEnabled(true);
    
    viewport = std::make_unique<Viewport>();

    viewport->setViewedComponent(table.get(), false);
    viewport->setScrollBarsShown(true, false);
    viewport->getVerticalScrollBar().addListener(this);
    
    addAndMakeVisible(viewport.get());
    update(triggerSources);

    
}

void PopupConfigurationWindow::scrollBarMoved(ScrollBar* scrollBar, double newRangeStart)
{

    if (!updating)
    {
        scrollDistance = viewport->getViewPositionY();
    }
    
}


void PopupConfigurationWindow::update(Array<TriggerSource*> triggerSources)
{

    if (triggerSources.size() > 0)
    {

        updating = true;
        
        tableModel->update(triggerSources);

        int maxRows = 16;

        int numRowsVisible = triggerSources.size() <= maxRows ? triggerSources.size() : maxRows;

        int scrollBarWidth = 0;

        if (triggerSources.size() > maxRows)
        {
            viewport->getVerticalScrollBar().setVisible(true);
            scrollBarWidth += 20;
        }
        else {
            viewport->getVerticalScrollBar().setVisible(false);
        }
          
        setSize(450 + scrollBarWidth, (numRowsVisible + 1) * 30 + 10 + 40);
        viewport->setBounds(5, 5, 430 + scrollBarWidth, (numRowsVisible + 1) * 30);
        table->setBounds(0, 0, 430 + scrollBarWidth, (triggerSources.size() + 1) * 30);
        
        viewport->setViewPosition(0, scrollDistance);
        
        table->setVisible(true);

        triggerSourceGenerator->setBounds(10, viewport->getBottom() + 8, 430, 30);
        
        updating = false;

    }
    else {
        tableModel->update(triggerSources);
        table->setVisible(false);
        setSize(450, 45);
        triggerSourceGenerator->setBounds(10, 8, 430, 30);
    }
    
}
