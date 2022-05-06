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

#ifndef OnlinePSTHCANVAS_H_
#define OnlinePSTHCANVAS_H_

#include <VisualizerWindowHeaders.h>

#include "OnlinePSTHDisplay.h"
#include "Timescale.h"


/**

    Visualizer for spike histograms

*/
class OnlinePSTHCanvas :
    public Visualizer,
    public Button::Listener,
    public ComboBox::Listener
{
public:
    
 
    /** Constructor */
    OnlinePSTHCanvas();
    
    /** Destructor */
    ~OnlinePSTHCanvas() { }
    
    /** Respond to button clicks */
    void buttonClicked(Button* button) override;
    
    /** Respond to button clicks */
    void comboBoxChanged(ComboBox* comboBox) override;
    
    /** Renders the Visualizer on each animation callback cycle
        Called instead of Juce's "repaint()" to avoid redrawing underlying components
        if not necessary.*/
    void refresh() { }

    /** Called when the Visualizer's tab becomes visible after being hidden .*/
    void refreshState();
    
    /** Called when the Visualizer is first created, and optionally when
        the parameters of the underlying processor are changed. */
    void update() { }
    
    /** Called when the component changes size */
    void resized();
    
    /** Renders component background */
    void paint(Graphics& g);

    /** Sets the overall window size*/
    void setWindowSizeMs(int pre_ms, int post_ms);
    
    /** Sets the bin size*/
    void setBinSizeMs(int bin_size);
    
    /** Add an event to the queue */
    void pushEvent(uint16 streamId, int64 sample_number);
    
    /** Add a spike to the queue */
    void pushSpike(const SpikeChannel* channel, int64 sample_number, int sortedId);
    
    /** Adds a spike channel */
    void addSpikeChannel(const SpikeChannel* channel);
    
    /** Prepare for update*/
    void prepareToUpdate();
    
    /** Save plot type*/
    void saveCustomParametersToXml(XmlElement* xml) override;
    
    /** Load plot type*/
    void loadCustomParametersFromXml(XmlElement* xml) override;

private:
    
    int pre_ms;
    int post_ms;
    
    ScopedPointer<UtilityButton> clearButton;
    ScopedPointer<ComboBox> plotTypeSelector;
    
    ScopedPointer<Viewport> viewport;
    
    ScopedPointer<Timescale> scale;
    ScopedPointer<OnlinePSTHDisplay> display;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OnlinePSTHCanvas);

};

#endif  // OnlinePSTHCANVAS_H_
