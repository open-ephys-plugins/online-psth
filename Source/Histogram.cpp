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

#include "Histogram.h"

Histogram::Histogram(const String& name, const String& streamName, uint16 streamId_, double sample_rate_)
    : sample_rate(sample_rate_), streamId(streamId_)
{
    infoLabel = new Label("info label");
    infoLabel->setText(name + "\n" + streamName, dontSendNotification);
    addAndMakeVisible(infoLabel);
    
    colours.add(Colour(255, 224, 93));
    colours.add(Colour(255, 178, 99));
    colours.add(Colour(255, 109, 161));
    colours.add(Colour(246, 102, 255));
    colours.add(Colour(175, 98, 255));
    colours.add(Colour(90, 241, 233));
    colours.add(Colour(109, 175, 136));
    colours.add(Colour(255, 224, 93));
    colours.add(Colour(160, 237, 181));
    
}

void Histogram::resized()
{
    infoLabel->setBounds(getWidth() - 40, 0, 40, getHeight());
}

void Histogram::clear()
{
    spikeSampleNumbers.clear();
    spikeSortedIds.clear();
    relativeTimes.clear();
    relativeTimeSortedIds.clear();
    
    for (auto c : counts)
        c.clear();
}

void Histogram::addSpike(int64 sample_number, int sortedId)
{
    spikeSampleNumbers.add(sample_number);
    spikeSortedIds.add(sortedId);
    
    int sortedIdIndex = uniqueSortedIds.indexOf(sortedId);
    
    if (sortedIdIndex < 0)
    {
        sortedIdIndex = uniqueSortedIds.size();
        uniqueSortedIds.add(sortedId);
        counts.add(Array<int>());
    }
    
    for (auto eventSampleNumber : eventSampleNumbers)
    {
        double offsetMs = double(eventSampleNumber - sample_number) / sample_rate;
        
        if (offsetMs > -pre_ms && offsetMs < post_ms)
        {
            relativeTimes.add(offsetMs);
            relativeTimeSortedIds.add(sortedId);
        }
    }
}

void Histogram::addEvent(int64 sample_number)
{
    eventSampleNumbers.add(sample_number);
}

void Histogram::setWindowSizeMs(int pre, int post)
{
    pre_ms = pre;
    post_ms = post;
    
    recompute();
}


void Histogram::recompute()
{
    relativeTimes.clear();
    
    int index = -1;
    
    for (auto spikeSampleNumber : spikeSampleNumbers)
    {
        index++;
        
        for (auto eventSampleNumber : eventSampleNumbers)
        {
            double offsetMs = double(eventSampleNumber - spikeSampleNumber) / sample_rate;
            
            if (offsetMs > -pre_ms && offsetMs < post_ms)
            {
                relativeTimes.add(offsetMs);
                relativeTimeSortedIds.add(spikeSortedIds[index]);
            }
        }
    }
    
    setBinSizeMs(bin_size_ms);
    
}

void Histogram::setBinSizeMs(int ms)
{
    bin_size_ms = ms;
    
    binEdges.clear();
    
    double bin_edge = (double) -pre_ms;
    
    while (bin_edge < post_ms)
    {
        binEdges.add(bin_edge);
        bin_edge += (double) bin_size_ms;
    }
    
    binEdges.add(post_ms);
    
    recount();
}

void Histogram::recount()
{
    
    const int nBins = binEdges.size() - 1;
    
    for (auto c : counts)
    {
        c.clear();
        c.insertMultiple(0, 0, nBins);
    }
    
    int maxCount = 0;
    
    for (int i = 0; i < relativeTimes.size(); i++)
    {
        for (int j = 0; j < nBins; j++)
        {
            if (relativeTimes[i] > binEdges[j] && relativeTimes[i] < binEdges[j+1])
            {
                int sortedIdIndex = uniqueSortedIds.indexOf(relativeTimeSortedIds[i]);
                int lastCount = counts[sortedIdIndex][j];
                int newCount = lastCount + 1;
                
                maxCount = jmax(newCount, maxCount);
                
                counts[sortedIdIndex].set(j, newCount);
            }
                
        }
    }
}

void Histogram::paint(Graphics& g)
{
    
    const int nBins = binEdges.size() - 1;
    
    float binWidth = getWidth() / float(nBins);
    
    for (int sortedIdIndex = 0; sortedIdIndex < uniqueSortedIds.size(); sortedIdIndex++)
    {
        if (sortedIdIndex == 0)
            g.setColour(Colours::lightgrey);
        else
            g.setColour(colours[(sortedIdIndex - 1) % colours.size() ]);
        
        for (int i = 0; i < nBins; i++)
        {
            float x = binWidth * i;
            float relativeHeight = float(counts[sortedIdIndex][i]) / float(maxCount);
            float height = relativeHeight * getHeight();
            float y = getHeight() - height;
            g.drawRect(x, y, binWidth, height);
        }
    }
    
    float zeroLoc = pre_ms / (pre_ms + post_ms) * getWidth();
    
    g.setColour(Colours::white);
    g.drawLine(zeroLoc, 0, zeroLoc, getHeight());
}


void Histogram::mouseMove(const MouseEvent &event)
{
    std::cout << event.getScreenX() << std::endl;
}

