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

OnlinePSTHCanvas::OnlinePSTHCanvas()
{
    
    scale = new Timescale();
    addAndMakeVisible(scale);
    
    clearButton = new UtilityButton("CLEAR", Font("Default", 12, Font::plain));
    clearButton->addListener(this);
    clearButton->setRadius(3.0f);
    clearButton->setClickingTogglesState(false);
    addAndMakeVisible(clearButton);
    
    plotTypeSelector = new ComboBox("Plot Type Selector");
    plotTypeSelector->addItem("Histogram", 1);
    plotTypeSelector->addItem("Raster", 2);
    plotTypeSelector->addItem("Histogram + Raster", 3);
    plotTypeSelector->addItem("Line", 4);
    plotTypeSelector->addItem("Line + Raster", 5);
    plotTypeSelector->setSelectedId(1, dontSendNotification);
    plotTypeSelector->addListener(this);
    addAndMakeVisible(plotTypeSelector);

    viewport = new Viewport();
    viewport->setScrollBarsShown(true, true);

    display = new OnlinePSTHDisplay();
    viewport->setViewedComponent(display, false);
    addAndMakeVisible(viewport);
    display->setBounds(0, 50, 500, 100);

}


void OnlinePSTHCanvas::refreshState()
{
    resized();
}

void OnlinePSTHCanvas::resized()
{

    int scrollBarThickness = viewport->getScrollBarThickness();
    int yOffset = 50;
    
    clearButton->setBounds(getWidth()-80, 12, 70, 25);
    
    plotTypeSelector->setBounds(getWidth()-240, 12, 150, 25);
    
    viewport->setBounds(0, yOffset, getWidth(), getHeight()-yOffset);
    
    display->setBounds(0, yOffset, getWidth()-scrollBarThickness, display->getDesiredHeight());

    scale->setBounds(10, 0, getWidth()-scrollBarThickness-150, 50);

}

void OnlinePSTHCanvas::paint(Graphics& g)
{
    
    g.fillAll(Colour(0,18,43));
    
    const float histogramWidth = getWidth()-viewport->getScrollBarThickness()-190;
    
    float zeroLoc = float(pre_ms) / float(pre_ms + post_ms) * histogramWidth + 10;
    
    g.setColour(Colours::white);
    //g.drawLine(0, getHeight()-3, histogramWidth, getHeight()-3, 3.0);
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

void OnlinePSTHCanvas::buttonClicked(Button* button)
{
    if (button == clearButton)
    {
        display->clear();
    }
}

void OnlinePSTHCanvas::comboBoxChanged(ComboBox* comboBox)
{
    if (comboBox == plotTypeSelector)
    {
        display->setPlotType(comboBox->getSelectedId());
    }
}


void OnlinePSTHCanvas::saveCustomParametersToXml(XmlElement* xml)
{
    xml->setAttribute("plot_type", plotTypeSelector->getSelectedId());
}

void OnlinePSTHCanvas::loadCustomParametersFromXml(XmlElement* xml)
{
    
    int selectedId = xml->getIntAttribute("plot_type", 1);
    
    plotTypeSelector->setSelectedId(selectedId, sendNotification);
}
