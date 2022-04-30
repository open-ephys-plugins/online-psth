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
    : sample_rate(sample_rate_),
      streamId(streamId_),
      waitingForWindowToClose(false),
      latestEventSampleNumber(0)
{
    
    pre_ms = 0;
    post_ms = 0;
    bin_size_ms = 10;
    
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
    infoLabel->setBounds(getWidth() - 150, 0, 150, getHeight());
}

void Histogram::clear()
{
    newSpikeSampleNumbers.clear();
    newSpikeSortedIds.clear();
    relativeTimes.clear();
    relativeTimeSortedIds.clear();
    
    for (auto c : counts)
        c.clear();
}

void Histogram::addSpike(int64 sample_number, int sortedId)
{
    
    const ScopedLock lock(mutex);
    
    newSpikeSampleNumbers.add(sample_number);
    newSpikeSortedIds.add(sortedId);
    
    int sortedIdIndex = uniqueSortedIds.indexOf(sortedId);
    
    if (sortedIdIndex < 0)
    {
        std::cout << "Adding new sortedId: " << sortedId << std::endl;
        sortedIdIndex = uniqueSortedIds.size();
        uniqueSortedIds.add(sortedId);
        counts.add(Array<int>());
    }
}

void Histogram::addEvent(int64 sample_number)
{
    if (!waitingForWindowToClose)
    {
        latestEventSampleNumber = sample_number;
        
        std::cout << "Added " << latestEventSampleNumber << std::endl;
        
        startTimer(1010);
        
        waitingForWindowToClose = true;
    }
    
}

void Histogram::setWindowSizeMs(int pre, int post)
{
    pre_ms = pre;
    post_ms = post;
    
    setBinSizeMs(bin_size_ms);
    
    recount();
    
    
}

void Histogram::update()
{
    std::cout << "Updating!" << std::endl;
    
    {
        const ScopedLock lock(mutex);
        
        std::cout << newSpikeSampleNumbers.size() << " new spikes." << std::endl;
        
        int index = 0;
        
        for (auto sample_number : newSpikeSampleNumbers)
        {
            double offsetMs = double(sample_number - latestEventSampleNumber) / sample_rate * 1000;
            
           // if (index < 10)
           // {
           //     std::cout << latestEventSampleNumber << " " << sample_number << " " << offsetMs << std::endl;
           // }
                
            
            if (offsetMs > -1000 && offsetMs < 1000)
            {
                //if (index < 10)
                 //   std::cout << offsetMs << std::endl;
                
                relativeTimes.add(offsetMs);
                relativeTimeSortedIds.add(newSpikeSortedIds[index]);
            }
            
            index++;
        }
        
        newSpikeSampleNumbers.clear();
        newSpikeSortedIds.clear();
    }
    
    recount();

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
        //std::cout << bin_edge << std::endl;
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
    
    maxCount = 1;
    
    std::cout << "Recount!" << std::endl;
    std::cout << "Num relative times: " << relativeTimes.size() << std::endl;
    
    for (int i = 0; i < relativeTimes.size(); i++)
    {
        for (int j = 0; j < nBins; j++)
        {
            
            if (relativeTimes[i] > binEdges[j] && relativeTimes[i] < binEdges[j+1])
            {
                //std::cout << "FOUND!" << std::endl;
                
                int sortedIdIndex = uniqueSortedIds.indexOf(relativeTimeSortedIds[i]);
                int lastCount = counts[sortedIdIndex][j];
                int newCount = lastCount + 1;
                
                maxCount = jmax(newCount, maxCount);
                
                counts.getReference(sortedIdIndex).set(j, newCount);
                
                //std::cout << "Count = " << newCount << std::endl;
                
                continue;
            }
                
        }
    }
    
    repaint();
}

void Histogram::paint(Graphics& g)
{
    
    g.fillAll(Colours::greenyellow);
    
    const int nBins = binEdges.size() - 1;
    
    float binWidth = getWidth() / float(nBins);
    
    for (int sortedIdIndex = 0; sortedIdIndex < uniqueSortedIds.size(); sortedIdIndex++)
    {
        if (uniqueSortedIds[sortedIdIndex] == 0)
            g.setColour(Colours::black);
        else
            g.setColour(colours[(uniqueSortedIds[sortedIdIndex] - 1) % colours.size() ]);
        
        for (int i = 0; i < nBins; i++)
        {
            float x = binWidth * i;
            float relativeHeight = float(counts[sortedIdIndex][i]) / float(maxCount);
            float height = relativeHeight * getHeight();
            float y = getHeight() - height;
            g.fillRect(x, y, binWidth, height);
            
            //if (i < 10)
            //    std::cout << x << " " << y << " " << binWidth << " " << height << std::endl;
        }
    }
    
    float zeroLoc = float(pre_ms) / float(pre_ms + post_ms) * getWidth();
    
    g.setColour(Colours::white);
    g.drawLine(zeroLoc, 0, zeroLoc, getHeight());
}


void Histogram::mouseMove(const MouseEvent &event)
{
    //std::cout << event.getScreenX() << std::endl;
}


void Histogram::timerCallback()
{
    
    stopTimer();
    
    waitingForWindowToClose = false;
    
    update();
    
}
