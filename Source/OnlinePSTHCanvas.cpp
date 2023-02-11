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

OptionsBar::OptionsBar(OnlinePSTHDisplay* display_)
	: display(display_)
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
}

void OptionsBar::buttonClicked(Button* button)
{
    if (button == clearButton.get())
    {
        display->clear();
    }
}

void OptionsBar::comboBoxChanged(ComboBox* comboBox)
{
    if (comboBox == plotTypeSelector.get())
    {
        display->setPlotType(comboBox->getSelectedId());
    }
}



void OptionsBar::resized()
{
    clearButton->setBounds(getWidth() - 80, 5, 70, 25);

    plotTypeSelector->setBounds(getWidth() - 240, 5, 150, 25);

}

void OptionsBar::paint(Graphics& g)
{
    g.fillAll(Colours::orange);
}

void OptionsBar::saveCustomParametersToXml(XmlElement* xml)
{
    xml->setAttribute("plot_type", plotTypeSelector->getSelectedId());
}

void OptionsBar::loadCustomParametersFromXml(XmlElement* xml)
{

    int selectedId = xml->getIntAttribute("plot_type", 1);

    plotTypeSelector->setSelectedId(selectedId, sendNotification);
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

	optionsBar = std::make_unique<OptionsBar>(display.get());
    addAndMakeVisible(optionsBar.get());

}


void OnlinePSTHCanvas::refreshState()
{
    resized();
}

void OnlinePSTHCanvas::resized()
{

    const int scrollBarThickness = viewport->getScrollBarThickness();
    const int timescaleHeight = 50;
    const int optionsBarHeight = 50;

    viewport->setBounds(0, timescaleHeight, getWidth(), getHeight()- timescaleHeight -optionsBarHeight);
    
    display->setBounds(0, 0, getWidth()-scrollBarThickness, display->getDesiredHeight());

    scale->setBounds(10, 0, getWidth()-scrollBarThickness-150, timescaleHeight);

	optionsBar->setBounds(0, getHeight() - optionsBarHeight, getWidth(), optionsBarHeight);

}

void OnlinePSTHCanvas::paint(Graphics& g)
{
    
    g.fillAll(Colour(0,18,43));
    
    const float histogramWidth = getWidth()-viewport->getScrollBarThickness()-190;
    
    float zeroLoc = float(pre_ms) / float(pre_ms + post_ms) * histogramWidth + 10;
    
    g.setColour(Colours::white);
    g.drawLine(zeroLoc, 0, zeroLoc, getHeight(), 2.0);
    
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
