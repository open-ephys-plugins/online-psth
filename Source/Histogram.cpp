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

Histogram::Histogram(const SpikeChannel* channel)
    : sample_rate(channel->getSampleRate()),
      streamId(channel->getStreamId()),
      waitingForWindowToClose(false),
      latestEventSampleNumber(0)
{
    
    pre_ms = 0;
    post_ms = 0;
    bin_size_ms = 10;
    
    infoLabel = new Label("info label");
    infoLabel->setJustificationType(Justification::topLeft);
    infoLabel->setText(channel->getName() + "\n" + channel->getStreamName(), dontSendNotification);
    infoLabel->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(infoLabel);
    
    channelLabel = new Label("channel label");
    channelLabel->setFont(14);
    channelLabel->setJustificationType(Justification::topLeft);
    channelLabel->setColour(Label::textColourId, Colours::white);
    String channelString = "";
    
    for (auto ch : channel->getSourceChannels())
        channelString += ch->getName() + ", ";
    
    channelString = channelString.substring(0, channelString.length()-2);
    channelLabel->setText(channelString, dontSendNotification);
    addAndMakeVisible(channelLabel);
    
    hoverLabel = new Label("hover label");
    hoverLabel->setJustificationType(Justification::topLeft);
    hoverLabel->setFont(12);
    hoverLabel->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(hoverLabel);
    
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
    infoLabel->setBounds(getWidth() - 150, 10, 150, 30);
    channelLabel->setBounds(getWidth() - 150, 45, 150, 15);
    hoverLabel->setBounds(getWidth() - 150, 66, 150, 45);
}

void Histogram::clear()
{
    relativeTimes.clear();
    relativeTimeSortedIds.clear();
    relativeTimeTrialIndices.clear();
    
    numTrials = 0;

    recount();
}

void Histogram::addSpike(int64 sample_number, int sortedId)
{
    
    const ScopedLock lock(mutex);
    
    newSpikeSampleNumbers.add(sample_number);
    newSpikeSortedIds.add(sortedId);
    
    int sortedIdIndex = uniqueSortedIds.indexOf(sortedId);
    
    if (sortedIdIndex < 0)
    {
        sortedIdIndex = uniqueSortedIds.size();
        uniqueSortedIds.add(sortedId);
        counts.add(Array<int>());
        maxSortedId = jmax(sortedId, maxSortedId);
    }
}

void Histogram::addEvent(int64 sample_number)
{
    if (!waitingForWindowToClose)
    {
        latestEventSampleNumber = sample_number;

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

void Histogram::setPlotType(int plotType)
{
    if (plotType == 1)
    {
        plotRaster = false;
        plotHistogram = true;
    } else if (plotType == 2)
    {
        plotRaster = true;
        plotHistogram = false;
    } else {
        plotRaster = true;
        plotHistogram = true;
    }
    
    repaint();
}

void Histogram::update()
{
    //std::cout << "Updating!" << std::endl;
    
    {
        const ScopedLock lock(mutex);
        
        //std::cout << newSpikeSampleNumbers.size() << " new spikes." << std::endl;
        
        int index = 0;
        
        for (auto sample_number : newSpikeSampleNumbers)
        {
            double offsetMs = double(sample_number - latestEventSampleNumber) / sample_rate * 1000;
            
            if (offsetMs > -1000 && offsetMs < 1000)
            {
                relativeTimes.add(offsetMs);
                relativeTimeSortedIds.add(newSpikeSortedIds[index]);
                relativeTimeTrialIndices.add(int(numTrials));
            }
            
            index++;
        }
        
        newSpikeSampleNumbers.clear();
        newSpikeSortedIds.clear();
        
        numTrials++;
        
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
        
    }
    
    binEdges.add(post_ms);
    
    recount();
}


void Histogram::recount()
{
    
    const int nBins = binEdges.size() - 1;
    
    for (int i = 0; i < counts.size(); i++)
    {
        counts.getReference(i).clear();
        counts.getReference(i).insertMultiple(0, 0, nBins);
    }
    
    maxCount = 1;

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
                
                counts.getReference(sortedIdIndex).set(j, newCount);

                continue;
            }
                
        }
    }
    
    repaint();
}

void Histogram::paint(Graphics& g)
{
    
    g.fillAll(Colour(30,30,40));
    
    const int nBins = binEdges.size() - 1;
    const float histogramWidth = getWidth() - 170;
    const float histogramHeight = getHeight() - 10;
    
    float binWidth = histogramWidth / float(nBins);
    
    if (plotHistogram)
    {
        for (int sortedId = 0; sortedId < maxSortedId + 1; sortedId++)
        {
            
            Colour plotColour;
            
            if (sortedId == 0)
                plotColour = Colours::greenyellow;
            else
                plotColour = colours[(sortedId - 1) % colours.size()];
            
            if (plotRaster)
                plotColour = plotColour.withBrightness(0.35f);
            
            const int sortedIdIndex = uniqueSortedIds.indexOf(sortedId);
            
            if (sortedIdIndex < 0)
                continue;
            
            for (int i = 0; i < nBins; i++)
            {
                if (hoverBin == i)
                    g.setColour(plotColour.withAlpha(0.85f));
                else
                    g.setColour(plotColour);
                
                float x = binWidth * i;
                float relativeHeight = float(counts[sortedIdIndex][i]) / float(maxCount);
                float height = relativeHeight * histogramHeight;
                float y = 10 + histogramHeight - height;
                g.fillRect(x, y, binWidth+1, height);

            }
        }
    }
    
    if (plotRaster)
    {
        int firstTrial = numTrials - maxRasterTrials;
        
        if (firstTrial < 0)
            firstTrial = 0;
        
        for (int index = 0; index < relativeTimes.size(); index++)
        {
            if (relativeTimeTrialIndices[index] >= firstTrial)
            {
                if (relativeTimes[index] > -pre_ms && relativeTimes[index] < post_ms)
                {
                    const float yPos = float(relativeTimeTrialIndices[index] - firstTrial) / float(maxRasterTrials) * (histogramHeight+10);
                    const float xPos = (relativeTimes[index] + float(pre_ms)) / float(pre_ms + post_ms) * histogramWidth;
                    const int sortedId = relativeTimeSortedIds[index];
                    
                    if (sortedId == 0)
                        g.setColour(Colours::white.withAlpha(0.8f));
                    else
                        g.setColour(colours[(sortedId - 1) % colours.size()]);
                    
                    g.fillEllipse(xPos, yPos, 2, 2);
                }
            }
        }
    }
    
    
    float zeroLoc = float(pre_ms) / float(pre_ms + post_ms) * histogramWidth;
    
    g.setColour(Colours::white);
    g.drawLine(zeroLoc, 0, zeroLoc, getHeight(), 2.0);
}


void Histogram::mouseMove(const MouseEvent &event)
{
    
    const int histogramWidth = getWidth() - 170;
    
    if (event.getPosition().x < histogramWidth)
    {
        const int nBins = binEdges.size() - 1;
        float binWidth = histogramWidth / float(nBins);
        
        hoverBin = (int) (float(event.getPosition().x) / binWidth);
        
        float firing_rate;
        
        if (numTrials > 0)
            firing_rate = float(counts[0][hoverBin] / numTrials) / (float(bin_size_ms) / 1000.0f) ;
        else
            firing_rate = 0;
        
        String firingRateString = String(firing_rate, 2) + " Hz";
        String binString = "[" + String(binEdges[hoverBin]) +
        "," + String(binEdges[hoverBin+1]) + "] ms";
        
        hoverLabel->setText(firingRateString + "\n" + binString, dontSendNotification);

        repaint();
    }
    
}

void Histogram::mouseExit(const MouseEvent &event)
{
    hoverLabel->setText("", dontSendNotification);
    hoverBin = -1;
    repaint();
}


void Histogram::timerCallback()
{
    stopTimer();
    
    waitingForWindowToClose = false;
    
    update();
}
