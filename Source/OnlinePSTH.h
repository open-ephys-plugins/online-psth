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

class OnlinePSTH;

enum TriggerType
{
	TTL_TRIGGER = 1,
	MSG_TRIGGER = 2,
	TTL_AND_MSG_TRIGGER = 3
};

/** 
	Represents one trigger source
*/
class TriggerSource
{
public:
    TriggerSource(OnlinePSTH* processor_, String name_, int line_, TriggerType type_) :
		processor(processor_), name(name_), line(line_), type(type_) {
    
        if (type == TTL_TRIGGER)
            canTrigger = true;
        else
            canTrigger = false;
    
    }
    
	String name;
	int line;
	TriggerType type;
    OnlinePSTH* processor;
    bool canTrigger;
};

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

    /** Returns an array of current trigger sources */
	Array<TriggerSource*> getTriggerSources();

    /** Adds a new trigger source */
    TriggerSource* addTriggerSource(int line, TriggerType type);

    /** Removes trigger sources */
	void removeTriggerSources(Array<TriggerSource*> sources);

    /** Checks whether the source name is unique*/
    String ensureUniqueName(String name);

    /** Sets trigger source name*/
    void setTriggerSourceName(TriggerSource* source, String name);

    /** Sets trigger source line */
    void setTriggerSourceLine(TriggerSource* source, int line);
    
    /** Sets trigger source type */
    void setTriggerSourceTriggerType(TriggerSource* source, TriggerType type);

    /** Saves trigger source parameters */
    void saveCustomParametersToXml(XmlElement* xml) override;

    /** Saves trigger source parameters */
    void loadCustomParametersFromXml(XmlElement* xml) override;
    
private:

    /** Responds to incoming broadcast messages */
    void handleBroadcastMessage(String message) override;
    
    /** Pushes incoming events to the canvas */
    void handleTTLEvent (TTLEventPtr event) override;

    /** Pushes incoming spikes to the canvas */
    void handleSpike(SpikePtr spike) override;

    OwnedArray<TriggerSource> triggerSources;

    int nextConditionIndex = 1;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OnlinePSTH);

};



#endif  // __OnlinePSTH_H_3F920F95__
