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

#include "OnlinePSTH.h"

Histogram::Histogram(const SpikeChannel* channel, const TriggerSource* source_)
    : sample_rate(channel->getSampleRate()), spikeChannel(channel), source(source_), baseColour(source_->colour),
      streamId(channel->getStreamId()),
      waitingForWindowToClose(false),
      latestEventSampleNumber(0)
{
    
    pre_ms = 0;
    post_ms = 0;
    bin_size_ms = 10;
    
    infoLabel = new Label("info label");
    infoLabel->setJustificationType(Justification::topLeft);
    infoLabel->setText(channel->getName(), dontSendNotification);
    infoLabel->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(infoLabel);

    channelLabel = new Label("channel label");
    channelLabel->setFont(14);
    channelLabel->setJustificationType(Justification::topLeft);
    channelLabel->setColour(Label::textColourId, Colours::white);
    String channelString = "";

    for (auto ch : channel->getSourceChannels())
        channelString += ch->getName() + ", ";

    channelString = channelString.substring(0, channelString.length() - 2);
    channelLabel->setText(channelString, dontSendNotification);
    addAndMakeVisible(channelLabel);
    
    conditionLabel = new Label("condition label");
    conditionLabel->setFont(16);
    conditionLabel->setJustificationType(Justification::topLeft);
    conditionLabel->setText(source->name, dontSendNotification);
    conditionLabel->setColour(Label::textColourId, baseColour);
    addAndMakeVisible(conditionLabel);
    
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
    colours.add(Colour(160, 237, 181));

    clear();
    
}

void Histogram::resized()
{
    
    
    int labelOffset;
    const int width = getWidth();

    if (width < 320)
        labelOffset = 5;
    else if (width >= 320 && width < 700)
        labelOffset = width - 120;
    else
        labelOffset = width - 150;
        
   
    if (labelOffset == 5)
        histogramWidth = width - labelOffset;
    else
        histogramWidth = labelOffset - 10;
    
    histogramHeight = getHeight() - 10;
    
    infoLabel->setBounds(labelOffset, 10, 150, 30);
    
    if (getHeight() < 100)
    {
        conditionLabel->setBounds(labelOffset, 26, 150, 30);
        channelLabel->setVisible(false);
        hoverLabel->setVisible(false);
	}
	else
	{
		conditionLabel->setBounds(labelOffset, 45, 150, 15);
		channelLabel->setVisible(!overlayMode);
        channelLabel->setBounds(labelOffset, 26, 150, 30);

		hoverLabel->setVisible(!overlayMode);
        hoverLabel->setBounds(labelOffset, 66, 150, 45);
	}

    if (labelOffset == 5)
    {
		conditionLabel->setVisible(false);
		channelLabel->setVisible(false);
        hoverLabel->setBounds(width - 120, 10, 150, 45);
	}
	else
	{
        conditionLabel->setVisible(true);
        channelLabel->setVisible(!overlayMode);

        if (overlayMode)
        {
            conditionLabel->setBounds(labelOffset, 45 + 18 * overlayIndex, 150, 15);
        }


    }
    
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

        startTimer(1010); // collect all spikes within 1 s
        
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
        plotLine = false;
    } 
    else if (plotType == 2)
    {
        plotRaster = true;
        plotHistogram = false;
        plotLine = false;
    } 
    else if (plotType == 3) 
    {
        plotRaster = true;
        plotHistogram = true;
        plotLine = false;
    }
    else if (plotType == 4) 
    {
        plotRaster = false;
        plotHistogram = false;
        plotLine = true;
    }
    else if (plotType == 5) {
        plotRaster = true;
        plotHistogram = false;
        plotLine = true;
    }

    repaint();
}

void Histogram::setSourceColour(Colour colour)
{
    baseColour = colour;
    conditionLabel->setColour(Label::textColourId, baseColour);
    repaint();
}


void Histogram::drawBackground(bool shouldDraw)
{
    shouldDrawBackground = shouldDraw;

    infoLabel->setVisible(shouldDrawBackground);

}

void Histogram::setOverlayMode(bool shouldOverlay)
{

    

    overlayMode = shouldOverlay;

}



void Histogram::setOverlayIndex(int index)
{
   
    overlayIndex = index;

    resized();

}

void Histogram::update()
{

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
        
    numTrials++;
        
    if (numTrials == 1)
        recount(true);
    else
        recount(false);

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


void Histogram::recount(bool full)
{
    
    const int nBins = binEdges.size() - 1;
    
    if (full)
    {
        for (int i = 0; i < counts.size(); i++)
        {
            counts.getReference(i).clear();
            counts.getReference(i).insertMultiple(0, 0, nBins);
        }
    }

    maxCount = 1;

    for (int i = 0; i < relativeTimes.size(); i++)
    {

        if (relativeTimeTrialIndices[i] == (numTrials - 1) || full)
        {
            for (int j = 0; j < nBins; j++)
            {

                maxCount = jmax(counts[0][j], maxCount);

                if (relativeTimes[i] > binEdges[j] && relativeTimes[i] < binEdges[j + 1])
                {
                    int sortedIdIndex = uniqueSortedIds.indexOf(relativeTimeSortedIds[i]);
                    int lastCount = counts[sortedIdIndex][j];
                    int newCount = lastCount + 1;

                    if (sortedIdIndex == 0)
                        maxCount = jmax(newCount, maxCount);

                    counts.getReference(sortedIdIndex).set(j, newCount);

                    continue;
                }

            }
        }

    }
    
    repaint();
}

void Histogram::paint(Graphics& g)
{
    
    if (shouldDrawBackground)
      g.fillAll(Colour(30,30,40));
    
    const int nBins = binEdges.size() - 1;
    float binWidth = histogramWidth / float(nBins);
    
    if (plotHistogram)
    {
        for (int sortedId = 0; sortedId < maxSortedId + 1; sortedId++)
        {
            
            Colour plotColour;
            
            if (sortedId == 0)
                plotColour = baseColour;
            else
                plotColour = colours[(sortedId - 1) % colours.size()];
            
            if (plotRaster)
                plotColour = plotColour.withBrightness(0.45f);

            if (overlayMode)
                plotColour = plotColour.withAlpha(0.5f);
            
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

    if (plotLine)
    {
        for (int sortedId = 0; sortedId < maxSortedId + 1; sortedId++)
        {

            Colour plotColour;

            if (sortedId == 0)
                plotColour = baseColour;
            else
                plotColour = colours[(sortedId - 1) % colours.size()];

            const int sortedIdIndex = uniqueSortedIds.indexOf(sortedId);

            if (sortedIdIndex < 0)
                continue;

			g.setColour(plotColour);

            for (int i = 0; i < nBins - 1; i++)
            {
                
                float x1 = binWidth * i + binWidth / 2;
                float x2 = binWidth * (i + 1) + binWidth / 2;
                float relativeHeight1 = float(counts[sortedIdIndex][i]) / float(maxCount);
                float height1 = relativeHeight1 * histogramHeight;
                float y1 = 9 + histogramHeight - height1;
                float relativeHeight2 = float(counts[sortedIdIndex][i+1]) / float(maxCount);
                float height2 = relativeHeight2 * histogramHeight;
                float y2 = 9 + histogramHeight - height2;
                g.drawLine(x1, y1, x2, y2, 2.0f);

                if (hoverBin == i)
					g.fillEllipse(x1 - 3, y1 - 3, 6, 6);

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
                    
                    if (!overlayMode)
                        g.setColour(Colours::white.withAlpha(0.8f));
                    else
                        g.setColour(baseColour);
                    
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


DynamicObject Histogram::getInfo()
{
    DynamicObject info;

    info.setProperty(Identifier("electrode"),
        var(spikeChannel->getName()));
    info.setProperty(Identifier("condition"),
        var(source->name));
    info.setProperty(Identifier("color"),
        var(source->colour.toString()));
    info.setProperty(Identifier("trial_count"),
        var(int(numTrials)));
    
    Array<var> bin_edges;
    Array<var> spike_counts;

    for (int bin = 0; bin < binEdges.size() - 1; bin++)
    {

        bin_edges.add(binEdges[bin]);
        spike_counts.add(counts[0][bin]);
    }

    info.setProperty(Identifier("bin_edges"), bin_edges);
    info.setProperty(Identifier("spike_counts"), spike_counts);

	return info;
}