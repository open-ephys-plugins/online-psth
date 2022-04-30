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

#ifndef __OnlinePSTH_H_3F920F95__
#define __OnlinePSTH_H_3F920F95__

#include <ProcessorHeaders.h>

#include <vector>
#include <map>

class OnlinePSTHCanvas;

/**
    
    Aligns spike times with incoming TTL events to generate real-time peri-stimulus
    time histogram (PSTH) plots.

*/

class OnlinePSTH : public GenericProcessor
{
public:

    /** Constructor */
    OnlinePSTH();

    /** Destructor */
    ~OnlinePSTH() { }

    /** Creates the OnlinePSTHEditor. */
    AudioProcessorEditor* createEditor() override;
    
    /** Used to alter parameters of data acquisition. */
    void parameterValueChanged(Parameter* param) override;

    /** Calls checkForEvents */
    void process(AudioBuffer<float>& buffer) override;
    
    /** Returns the PSTH pre-event window size in ms */
    int getPreWindowSizeMs();
    
    /** Returns the PSTH post-event window size in ms */
    int getPostWindowSizeMs();
    
    /** Returns the PSTH bin size in ms*/
    int getBinSizeMs();
    
    /** Pointer to the display canvas */
    OnlinePSTHCanvas* canvas;

private:
    
    /** Pushes incoming events to the canvas */
    void handleTTLEvent (TTLEventPtr event) override;

    /** Pushes incoming spikes to the canvas */
    void handleSpike(SpikePtr spike) override;
    
    
    
    int triggerLine;
   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OnlinePSTH);

};



#endif  // __OnlinePSTH_H_3F920F95__
