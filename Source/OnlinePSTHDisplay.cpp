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

#include "OnlinePSTHDisplay.h"
#include "OnlinePSTH.h"

OnlinePSTHDisplay::OnlinePSTHDisplay()
{

}


void OnlinePSTHDisplay::refresh()
{
    for (auto hist : histograms)
    {
        hist->repaint();
    }
}


void OnlinePSTHDisplay::prepareToUpdate()
{
    histograms.clear();
    triggerSourceMap.clear();
    spikeChannelMap.clear();
    setBounds(0, 0, getWidth(), 0);
}


void OnlinePSTHDisplay::resized()
{
    totalHeight = 0;
	const int numPlots = histograms.size();
    const int leftEdge = 10;
	const int rightEdge = getWidth() - borderSize;
    const int histogramWidth = (rightEdge - leftEdge - borderSize * (numColumns - 1)) / numColumns;

    int index = -1;
    int overlayIndex = 0;
    int row = 0;
    int col = 0;
    bool drawBackground = true;

    SpikeChannel* latestChannel = nullptr;

    for (auto hist : histograms)
    {
        if (overlayConditions)
        {
            if (hist->spikeChannel != latestChannel)
            {
                latestChannel = const_cast<SpikeChannel*>(hist->spikeChannel);
                drawBackground = true;
                index++;
                overlayIndex = 0;
            }

        }
        else {
            index++;
        }
        
		row = index / numColumns;
		col = index % numColumns;

        hist->drawBackground(drawBackground);
		hist->setBounds(leftEdge + col * (histogramWidth + borderSize),
                       row * (histogramHeight + borderSize), 
                       histogramWidth, histogramHeight);
       
        hist->setOverlayMode(overlayConditions);
        hist->setOverlayIndex(overlayIndex);
        

        if (overlayConditions)
        {
            drawBackground = false;
            overlayIndex++;
        }
            
        //std::cout << "Histogram bounds: 0, " << totalHeight << ", " << getWidth() << ", " << histogramHeight << std::endl;
    }

    totalHeight = (row + 1) * (histogramHeight + borderSize);
}


void OnlinePSTHDisplay::addSpikeChannel(const SpikeChannel* channel, const TriggerSource* source)
{

    Histogram* h = new Histogram(channel, source);
    h->setPlotType(plotType);

    LOGD("Display adding ", channel->getName(), " for ", source->name);
    
    histograms.add(h);
    triggerSourceMap[source].add(h);
    spikeChannelMap[channel].add(h);

    int numRows = histograms.size() / numColumns + 1;

    totalHeight = (numRows + 1) * (histogramHeight + 10);

    addAndMakeVisible(h);
}

void OnlinePSTHDisplay::updateColourForSource(const TriggerSource* source)
{
    Array<Histogram*> h = triggerSourceMap[source];

    for (auto hist : h)
    {
        hist->setSourceColour(source->colour);
    }
}

void OnlinePSTHDisplay::setRowHeight(int height)
{
	histogramHeight = height;
	resized();
}

void OnlinePSTHDisplay::setNumColumns(int numColumns_)
{
    numColumns = numColumns_;
    resized();
}


void OnlinePSTHDisplay::setConditionOverlay(bool overlay_)
{

    //std::cout << "Set condition overlay: " << overlay_ << std::endl;
    overlayConditions = overlay_;
    resized();
}



void OnlinePSTHDisplay::setWindowSizeMs(int pre_ms, int post_ms_)
{
    
    post_ms = post_ms_;
    
    for (auto hist : histograms)
    {
        hist->setWindowSizeMs(pre_ms, post_ms);
    }
}

void OnlinePSTHDisplay::setBinSizeMs(int bin_size)
{
    for (auto hist : histograms)
    {
        hist->setBinSizeMs(bin_size);
    }
}



void OnlinePSTHDisplay::setPlotType(int plotType_)
{
    
    plotType = plotType_;
    
    for (auto hist : histograms)
    {
        hist->setPlotType(plotType);
    }
}

void OnlinePSTHDisplay::pushEvent(const TriggerSource* source, uint16 streamId, int64 sample_number)
{
    
    for (auto hist : triggerSourceMap[source])
    {
        if (hist->streamId == streamId)
            hist->addEvent(sample_number);
    }
    
}

void OnlinePSTHDisplay::pushSpike(const SpikeChannel* channel, int64 sample_number, int sortedId)
{

    for (auto hist : spikeChannelMap[channel])
    {
        hist->addSpike(sample_number, sortedId);
    }
}


int OnlinePSTHDisplay::getDesiredHeight()
{
    return totalHeight;
}

void OnlinePSTHDisplay::clear()
{
    for (auto hist : histograms)
    {
        hist->clear();
    }
}

DynamicObject OnlinePSTHDisplay::getInfo()
{

    DynamicObject output;

    Array<var> histogram_info;
        
    for (auto hist : histograms)
    {
        DynamicObject::Ptr hist_info = hist->getInfo().clone();
        
        histogram_info.add(hist_info.get());
    }

    output.setProperty(Identifier("histograms"), histogram_info);

    return output;
}
