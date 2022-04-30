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

/**
 
    Displays the actual PSTH
 
 */
class Histogram : public Component
{
public:
    
    /** Constructor */
    Histogram(const String& name,
              const String& streamName,
              uint16 streamId,
              double sample_rate);
    
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
    
    /** Listens for mouse movements */
    void mouseMove(const MouseEvent& event);
    
    /** Stream ID for this histogram */
    uint16 streamId;

private:
    
    /** Recomputes temporal offsets */
    void recompute();
    
    /** Recomputes bin counts */
    void recount();
    
    ScopedPointer<Label> infoLabel;
    
    Array<int64> spikeSampleNumbers;
    Array<int64> eventSampleNumbers;
    
    Array<int> spikeSortedIds;
    Array<int> uniqueSortedIds;
    
    Array<double> binEdges;
    
    int maxSortedId = 0;
    
    Array<double> relativeTimes;
    Array<int> relativeTimeSortedIds;
    Array<Colour> colours;
    
    Array<Array<int>> counts;
    
    int pre_ms;
    int post_ms;
    int bin_size_ms;
    
    int maxCount;
    
    const double sample_rate;
    
};



#endif  // Histogram_H__
