/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2022 Open Ephys

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

#include "OnlinePSTHCanvas.h"
#include "OnlinePSTH.h"

OptionsBar::OptionsBar(OnlinePSTHCanvas* canvas_, OnlinePSTHDisplay* display_, Timescale* timescale_)
	: canvas(canvas_), display(display_), timescale(timescale_)
{
    
    clearButton = std::make_unique<UtilityButton>("CLEAR", Font("Default", 12, Font::plain));
    clearButton->addListener(this);
    clearButton->setRadius(3.0f);
    clearButton->setClickingTogglesState(false);
    addAndMakeVisible(clearButton.get());

    plotTypeSelector = std::make_unique<ComboBox>("Plot Type Selector");
    plotTypeSelector->addItem("Histogram", 1);
    plotTypeSelector->addItem("Raster", 2);
    plotTypeSelector->addItem("Histogram + Raster", 3);
    plotTypeSelector->addItem("Line", 4);
    plotTypeSelector->addItem("Line + Raster", 5);
    plotTypeSelector->setSelectedId(1, dontSendNotification);
    plotTypeSelector->addListener(this);
    addAndMakeVisible(plotTypeSelector.get());

    columnNumberSelector = std::make_unique<ComboBox>("Column Number Selector");
    for (int i = 1; i < 7; i++)
        columnNumberSelector->addItem(String(i), i);
    columnNumberSelector->setSelectedId(1, dontSendNotification);
    columnNumberSelector->addListener(this);
    addAndMakeVisible(columnNumberSelector.get());

    rowHeightSelector = std::make_unique<ComboBox>("Row Height Selector");
    for (int i = 1; i < 5; i++)
        rowHeightSelector->addItem(String(i * 50) + " px", i * 50);
    rowHeightSelector->setSelectedId(300, dontSendNotification);
    rowHeightSelector->addListener(this);
    addAndMakeVisible(rowHeightSelector.get());

    overlayButton = std::make_unique<UtilityButton>("OFF", Font("Default", 12, Font::plain));
    overlayButton->addListener(this);
    overlayButton->setRadius(3.0f);
    overlayButton->setClickingTogglesState(true);
    addAndMakeVisible(overlayButton.get());
    
}

void OptionsBar::buttonClicked(Button* button)
{
    if (button == clearButton.get())
    {
        display->clear();
    }

    else if (button == overlayButton.get())
    {
        // display->overlayConditions(button->getToggleState());

        if (overlayButton->getToggleState())
            overlayButton->setLabel("ON");
        else
			overlayButton->setLabel("OFF");
    }
}

void OptionsBar::comboBoxChanged(ComboBox* comboBox)
{
    if (comboBox == plotTypeSelector.get())
    {
        display->setPlotType(comboBox->getSelectedId());
    } 
	else if (comboBox == columnNumberSelector.get())
	{
		const int numColumns = comboBox->getSelectedId();
        
		display->setNumColumns(numColumns);

        if (numColumns == 1)
            timescale->setVisible(true);
        else
            timescale->setVisible(false);

        canvas->resized();
	}
	else if (comboBox == rowHeightSelector.get())
	{
		display->setRowHeight(comboBox->getSelectedId());

        canvas->resized();
	}
}



void OptionsBar::resized()
{

    const int verticalOffset = 7;
    
    clearButton->setBounds(getWidth() - 100, verticalOffset, 70, 25);

    plotTypeSelector->setBounds(440, verticalOffset, 150, 25);

    rowHeightSelector->setBounds(60, verticalOffset, 80, 25);

	columnNumberSelector->setBounds(200, verticalOffset, 50, 25);

	overlayButton->setBounds(340, verticalOffset, 35, 25);

}

void OptionsBar::paint(Graphics& g)
{
    g.fillAll(Colours::black);

	g.setColour(Colours::grey);

    const int verticalOffset = 4;

	g.drawText("Row", 0, verticalOffset, 53, 15, Justification::centredRight, false);
    g.drawText("Height", 0, verticalOffset + 15, 53, 15, Justification::centredRight, false);
    g.drawText("Num", 150, verticalOffset, 43, 15, Justification::centredRight, false);
    g.drawText("Cols", 150, verticalOffset + 15, 43, 15, Justification::centredRight, false);
    g.drawText("Overlay", 240, verticalOffset, 93, 15, Justification::centredRight, false);
    g.drawText("Conditions", 240, verticalOffset + 15, 93, 15, Justification::centredRight, false);
    g.drawText("Plot", 390, verticalOffset, 43, 15, Justification::centredRight, false);
    g.drawText("Type", 390, verticalOffset + 15, 43, 15, Justification::centredRight, false);

}

void OptionsBar::saveCustomParametersToXml(XmlElement* xml)
{
    xml->setAttribute("plot_type", plotTypeSelector->getSelectedId());
    xml->setAttribute("num_cols", columnNumberSelector->getSelectedId());
    xml->setAttribute("row_height", rowHeightSelector->getSelectedId());
    xml->setAttribute("overlay", overlayButton->getToggleState());
}

void OptionsBar::loadCustomParametersFromXml(XmlElement* xml)
{
    columnNumberSelector->setSelectedId(xml->getIntAttribute("num_cols", 1), sendNotification);
    rowHeightSelector->setSelectedId(xml->getIntAttribute("row_height", 300), sendNotification);
    overlayButton->setToggleState(xml->getBoolAttribute("overlay", false), sendNotification);
    plotTypeSelector->setSelectedId(xml->getIntAttribute("plot_type", 1), sendNotification);
}


OnlinePSTHCanvas::OnlinePSTHCanvas()
{
    
    scale = std::make_unique<Timescale>();
    addAndMakeVisible(scale.get());

    viewport = std::make_unique<Viewport>();
    viewport->setScrollBarsShown(true, true);

    display = std::make_unique<OnlinePSTHDisplay>();
    viewport->setViewedComponent(display.get(), false);
    addAndMakeVisible(viewport.get());
    display->setBounds(0, 50, 500, 100);

	optionsBar = std::make_unique<OptionsBar>(this, display.get(), scale.get());
    addAndMakeVisible(optionsBar.get());

}


void OnlinePSTHCanvas::refreshState()
{
    resized();
}

void OnlinePSTHCanvas::resized()
{

    const int scrollBarThickness = viewport->getScrollBarThickness();
    const int timescaleHeight = 40;
    const int optionsBarHeight = 40;

    if (scale->isVisible())
    {
        scale->setBounds(10, 0, getWidth() - scrollBarThickness - 150, timescaleHeight);
        viewport->setBounds(0, timescaleHeight, getWidth(), getHeight() - timescaleHeight - optionsBarHeight);
    }
    else {
        viewport->setBounds(0, 10, getWidth(), getHeight() - 10 - optionsBarHeight);
    }

    display->setBounds(0, 0, getWidth()-scrollBarThickness, display->getDesiredHeight());
    display->resized();

	optionsBar->setBounds(0, getHeight() - optionsBarHeight, getWidth(), optionsBarHeight);

}

void OnlinePSTHCanvas::paint(Graphics& g)
{
    
    g.fillAll(Colour(0,18,43));
    
}

void OnlinePSTHCanvas::setWindowSizeMs(int pre_ms_, int post_ms_)
{
    pre_ms = pre_ms_;
    post_ms = post_ms_;
    
    display->setWindowSizeMs(pre_ms, post_ms);
    scale->setWindowSizeMs(pre_ms, post_ms);
    
    repaint();
}

void OnlinePSTHCanvas::setBinSizeMs(int bin_size)
{
    display->setBinSizeMs(bin_size);
}

void OnlinePSTHCanvas::pushEvent(const TriggerSource* source, uint16 streamId, int64 sample_number)
{
    display->pushEvent(source, streamId, sample_number);
}


void OnlinePSTHCanvas::pushSpike(const SpikeChannel* channel, int64 sample_number, int sortedId)
{
    display->pushSpike(channel, sample_number, sortedId);
}

void OnlinePSTHCanvas::addSpikeChannel(const SpikeChannel* channel, const TriggerSource* source)
{
    display->addSpikeChannel(channel, source);
}

void OnlinePSTHCanvas::updateColourForSource(const TriggerSource* source)
{
    display->updateColourForSource(source);
}

void OnlinePSTHCanvas::prepareToUpdate()
{
    display->prepareToUpdate();
}



void OnlinePSTHCanvas::saveCustomParametersToXml(XmlElement* xml)
{
    optionsBar->saveCustomParametersToXml(xml);
}

void OnlinePSTHCanvas::loadCustomParametersFromXml(XmlElement* xml)
{
    
    optionsBar->loadCustomParametersFromXml(xml);
}
