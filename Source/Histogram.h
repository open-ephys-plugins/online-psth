/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2013 Open Ephys

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

#ifndef Histogram_H__
#define Histogram_H__

#include <VisualizerWindowHeaders.h>

#include <vector>

class TriggerSource;
class OnlinePSTHDisplay;

/**
 
    Displays the actual PSTH
 
 */
class Histogram :
    public Component,
    public Timer,
    public ComboBox::Listener
{
public:
    
    /** Constructor */
    Histogram(OnlinePSTHDisplay*, const SpikeChannel* channel, const TriggerSource* triggerSource);
    
    /** Destructor */
    ~Histogram() { }
    
    /** Draws the histogram */
    void paint(Graphics& g);
    
    /** Called when histogram is resized */
    void resized();
    
    /** Adds a spike time */
    void addSpike(int64 sample_number, int sortedId);
    
    /** Adds an event time*/
    void addEvent(int64 sample_number);
    
    /** Clears the display*/
    void clear();
    
    /** Sets the =window size*/
    void setWindowSizeMs(int pre_ms, int post_ms);
    
    /** Sets the pre-event window size*/
    void setBinSizeMs(int ms);
    
    /** Sets the plot type (histogram, raster, raster + histogram) */
    void setPlotType(int plotType);

    /** Sets the plot colour */
    void setSourceColour(Colour colour);

    /** Sets the condition name */
    void setSourceName(String name);

    /** Sets the unit ID */
    void setUnitId(int unitId);

    /** Sets the max count (used in condition overlay mode) */
    void setMaxCount(int unitId, int count);

    /** Sets background draw state */
    void drawBackground(bool);

    /** Sets overlay mode */
    void setOverlayMode(bool);

    /** Sets overlay index */
    void setOverlayIndex(int index);

    /** Listens for mouse movements */
    void mouseMove(const MouseEvent& event);
    
    /** Listens for mouse movements */
    void mouseExit(const MouseEvent& event);

    /** Listens for ComboBox callbacks */
    void comboBoxChanged(ComboBox* comboBox) override;
    
    /** Called by OnlinePSTHDisplay after event window closes */
    void update();
    
    /** Stream ID for this histogram */
    uint16 streamId;

    /** Spike channel for this histogram */
    const SpikeChannel* spikeChannel;

    /** Return info about this histogram */
    DynamicObject getInfo();

private:
    
    /** Updates histogram after event window closes*/
    void timerCallback();
    
    /** Recomputes temporal offsets */
    void recompute();
    
    /** Recomputes bin counts */
    void recount(bool full=true);
    
    std::unique_ptr<Label> infoLabel;
    std::unique_ptr<Label> channelLabel;
    std::unique_ptr<Label> conditionLabel;
    std::unique_ptr<Label> hoverLabel;
    std::unique_ptr<ComboBox> unitSelector;
    
    Array<int64> newSpikeSampleNumbers;
    Array<int> newSpikeSortedIds;
    
    int64 latestEventSampleNumber;
    
    Array<int> uniqueSortedIds;
    
    Array<double> binEdges;
    
    bool plotHistogram = true;
    bool plotRaster = false;
    bool plotLine = false;
    
    int maxSortedId = 0;
    int maxRasterTrials = 30;
    
    Array<double> relativeTimes;
    Array<int> relativeTimeTrialIndices;
    Array<int> relativeTimeSortedIds;
    Colour baseColour;
    
    Array<Array<int>> counts;

    const TriggerSource* source;
    OnlinePSTHDisplay* display;
    
    int pre_ms;
    int post_ms;
    int bin_size_ms;

    float histogramWidth;
    float histogramHeight;

    bool shouldDrawBackground = true;

    int overlayIndex = 0;
    bool overlayMode = false;
    
    Array<int> maxCounts;
    
    int hoverBin = -1;
    
    bool waitingForWindowToClose;
    
    float numTrials = 0;

    int currentUnitId = 0;
    
    const double sample_rate;
    
};



#endif  // Histogram_H__
