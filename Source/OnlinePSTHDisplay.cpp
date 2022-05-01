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
    histogramMap.clear();
    setBounds(0, 0, getWidth(), 0);
}


void OnlinePSTHDisplay::resized()
{
    totalHeight = 0;
    
    for (auto hist : histograms)
    {
        hist->setBounds(10, totalHeight, getWidth()-20, histogramHeight);
        totalHeight += histogramHeight + borderSize;
        
        //std::cout << "Histogram bounds: 0, " << totalHeight << ", " << getWidth() << ", " << histogramHeight << std::endl;
    }
}


void OnlinePSTHDisplay::addSpikeChannel(const SpikeChannel* channel)
{
    Histogram* h = new Histogram(channel);
    
    histograms.add(h);
    histogramMap[channel] = h;
    
    totalHeight += histogramHeight + borderSize;
    
    //std::cout << "Adding histogram for " << channel->getName() << std::endl;

    addAndMakeVisible(h);
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

void OnlinePSTHDisplay::pushEvent(uint16 streamId, int64 sample_number)
{
    
    for (auto hist : histograms)
    {
        if (hist->streamId == streamId)
            hist->addEvent(sample_number);
    }
    
}

void OnlinePSTHDisplay::pushSpike(const SpikeChannel* channel, int64 sample_number, int sortedId)
{
    histogramMap[channel]->addSpike(sample_number, sortedId);
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

