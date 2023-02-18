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

#ifndef OnlinePSTHDISPLAY_H_
#define OnlinePSTHDISPLAY_H_

#include <VisualizerWindowHeaders.h>

#include "Histogram.h"

#include <vector>

class TriggerSource;

/*
    
    Component that holds the histogram displays
 
 */
class OnlinePSTHDisplay : public Component
{
    
public:
    
    /** Constructor */
    OnlinePSTHDisplay();
    
    /** Destructor */
    ~OnlinePSTHDisplay() { }
    
    /** Renders the Visualizer on each animation callback cycle
        Called instead of Juce's "repaint()" to avoid redrawing underlying components
        if not necessary.*/
    void refresh();
    
    /** Called when component changes size*/
    void resized();

    /** Sets the overall window size*/
    void setWindowSizeMs(int pre_ms, int post_ms);
    
    /** Sets the bin size*/
    void setBinSizeMs(int bin_size);
    
    /** Sets the bin size*/
    void setPlotType(int plotType);
    
    /** Add an event to the queue */
    void pushEvent(const TriggerSource* source, uint16 streamId, int64 sample_number);
    
    /** Add a spike to the queue */
    void pushSpike(const SpikeChannel* channel, int64 sample_number, int sortedId);
    
    /** Adds a spike channel for a given trigger source */
    void addSpikeChannel(const SpikeChannel* channel, const TriggerSource* source);

    /** Changes source colour */
    void updateColourForSource(const TriggerSource* source);

    /** Changes source name */
    void updateConditionName(const TriggerSource* source);

    /** Changes num columns */
    void setNumColumns(int numColumns);

    /** Changes row height */
    void setRowHeight(int rowHeightPixels);

    /** Sets whether conditions are overlaid or not */
    void setConditionOverlay(bool);

    /** Sets selected unit in condition overlay mode */
    void setUnitForElectrode(const SpikeChannel* channel, int unitId);
    
    /** Prepare for update*/
    void prepareToUpdate();
    
    /** Returns the desired height for this component*/
    int getDesiredHeight();
    
    /** Clears the histograms */
    void clear();

    /** Returns histogram info */
    DynamicObject getInfo();

private:
    
    OwnedArray<Histogram> histograms;
    
	std::map<const TriggerSource*, Array<Histogram*>> triggerSourceMap;
    std::map<const SpikeChannel*, Array<Histogram*>> spikeChannelMap;
    
    int totalHeight = 0;
    int histogramHeight = 100;
    int borderSize = 10;
    int numColumns = 1;

    bool overlayConditions = false;
    
    int post_ms;
    int plotType = 1;
};


#endif  // OnlinePSTHDISPLAY_H_
