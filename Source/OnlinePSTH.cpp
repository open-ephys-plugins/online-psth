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

#include "OnlinePSTH.h"

#include <stdio.h>

#include "OnlinePSTHCanvas.h"
#include "OnlinePSTHEditor.h"


OnlinePSTH::OnlinePSTH()
    : GenericProcessor("Online PSTH"),
      canvas(nullptr)
{

    addIntParameter(Parameter::GLOBAL_SCOPE,
                    "pre_ms",
                    "Size of the PSTH window in ms",
                    500, 10, 1000);
    
    addIntParameter(Parameter::GLOBAL_SCOPE,
                    "post_ms",
                    "Size of the PSTH window in ms",
                    500, 10, 1000);
    
    addIntParameter(Parameter::GLOBAL_SCOPE,
                    "bin_size",
                    "Size of the PSTH bins in ms",
                    10, 1, 100);
    
    addIntParameter(Parameter::GLOBAL_SCOPE,
                    "trigger_line",
                    "The input TTL line of the current trigger source",
                    0, 0, 255);

    addIntParameter(Parameter::GLOBAL_SCOPE,
        "trigger_type",
        "The type of the current trigger source",
        1, 1, 3);
    
}

AudioProcessorEditor* OnlinePSTH::createEditor()
{
    editor = std::make_unique<OnlinePSTHEditor> (this);
    return editor.get();
}


void OnlinePSTH::parameterValueChanged(Parameter* param)
{
   if (param->getName().equalsIgnoreCase("pre_ms"))
    {
        if (canvas != nullptr)
            canvas->setWindowSizeMs((int) param->getValue(),
                                    (int) getParameter("post_ms")->getValue());
    }
    else if (param->getName().equalsIgnoreCase("post_ms"))
    {
        if (canvas != nullptr)
            canvas->setWindowSizeMs((int) getParameter("pre_ms")->getValue(),
                                    (int) getParameter("post_ms")->getValue());
    }
    else if (param->getName().equalsIgnoreCase("bin_size"))
    {
        if (canvas != nullptr)
            canvas->setBinSizeMs((int) param->getValue());
   }
    else if (param->getName().equalsIgnoreCase("trigger_line"))
   {
       if (currentTriggerSource != nullptr)
       {
		   currentTriggerSource->line = (int)param->getValue();
       }
    }
    else if (param->getName().equalsIgnoreCase("trigger_type"))
   {
       if (currentTriggerSource != nullptr)
       {
           currentTriggerSource->type = (TriggerType)(int)param->getValue();

           if (currentTriggerSource->type == TTL_TRIGGER)
               currentTriggerSource->canTrigger = true;
           else
               currentTriggerSource->canTrigger = false;
       }
       
   }

}

int OnlinePSTH::getPreWindowSizeMs()
{
    return (int) getParameter("pre_ms")->getValue();
}

int OnlinePSTH::getPostWindowSizeMs()
{
    return (int) getParameter("post_ms")->getValue();
}


int OnlinePSTH::getBinSizeMs()
{
    return (int) getParameter("bin_size")->getValue();
}

Array<TriggerSource*> OnlinePSTH::getTriggerSources()
{
    Array<TriggerSource*> sources;

    for (auto source : triggerSources)
    {
        sources.add(source);
    }

    return sources;
}

TriggerSource* OnlinePSTH::addTriggerSource(int line, TriggerType type)
{
	String name = "Condition " + String(nextConditionIndex++);
    
	TriggerSource* source = new TriggerSource(this, name, line, type);
    source->colour = TriggerSource::getColourForLine(triggerSources.size());
	triggerSources.add(source);

    //LOGD("Adding ", name);

	return source;
}

void OnlinePSTH::removeTriggerSources(Array<TriggerSource*> sources)
{
	for (auto source : sources)
	{
		triggerSources.removeObject(source);
	}
}


String OnlinePSTH::ensureUniqueName(String name)
{

    // std::cout << "Candidate name: " << name << std::endl;

    bool matchingName = true;

    int append = 0;

    String nameToCheck = name;

    while (matchingName)
    {
        if (append > 0)
            nameToCheck = name + " (" + String(append) + ")";

        matchingName = false;

        for (auto source : triggerSources)
        {
            if (source->name.equalsIgnoreCase(nameToCheck))
            {
                matchingName = true;
                append += 1;
                break;
            }
        }
    }

    // std::cout << "New name: " << nameToCheck;

    return nameToCheck;
}

void OnlinePSTH::setTriggerSourceName(TriggerSource* source, String name, bool updateEditor)
{
    source->name = name;

    if (updateEditor)
    {
        OnlinePSTHEditor* editor = (OnlinePSTHEditor*)getEditor();

        editor->updateConditionName(source);
    }
}

void OnlinePSTH::setTriggerSourceLine(TriggerSource* source, int line, bool updateEditor)
{

    currentTriggerSource = source;
    
    getParameter("trigger_line")->setNextValue(line);

}

void OnlinePSTH::setTriggerSourceColour(TriggerSource* source, Colour colour, bool updateEditor)
{
    source->colour = colour;

    if (updateEditor)
    {
        OnlinePSTHEditor* editor = (OnlinePSTHEditor*)getEditor();

        editor->updateColours(source);
    }
	
}


void OnlinePSTH::setTriggerSourceTriggerType(TriggerSource* source, TriggerType type, bool updateEditor)
{

    currentTriggerSource = source;

    getParameter("trigger_type")->setNextValue((int) type);
    
}

void OnlinePSTH::process(AudioBuffer<float>& buffer)
{
    checkForEvents(true);
}

void OnlinePSTH::handleBroadcastMessage(String message)
{
    LOGD("Online PSTH received ", message);

    for (auto source : triggerSources)
    {
        if (message.equalsIgnoreCase(source->name))
        {
            if (source->type == TTL_AND_MSG_TRIGGER)
            {
                source->canTrigger = true;
            }
            else if (source->type == MSG_TRIGGER)
            {
                if (canvas != nullptr)
                {
                    for (auto stream : getDataStreams())
                    {
                        const uint16 streamId = stream->getStreamId();
                        canvas->pushEvent(source, streamId, getFirstSampleNumberForBlock(streamId));
                    }
                }
            }
        }
    }
}

String OnlinePSTH::handleConfigMessage(String message)
{
    LOGD("Online PSTH received ", message);

    var parsedMessage = JSON::parse(message);
    
    if (!parsedMessage.isObject())
        return "Invalid JSON string";
    
    DynamicObject::Ptr jsonMessage = parsedMessage.getDynamicObject();
    
    if (jsonMessage == nullptr)
        return "Invalid JSON string";
    
    int condition_index;
    bool foundValue = getIntField(jsonMessage,
        "condition_index", // field name
        condition_index,   // value to set
        0,                 // minimum value
        triggerSources.size()); // maximum value

    LOGD(condition_index);

    if (!foundValue || condition_index >= triggerSources.size())
        return "Condition index out of bounds.";

    TriggerSource* source = triggerSources[condition_index];

    if (jsonMessage->hasProperty("name"))
    {
        setTriggerSourceName(source, jsonMessage->getProperty("name"), false);

        startTimer(100);
    }

    if (jsonMessage->hasProperty("ttl_line"))
    {
        int ttl_line;
        bool foundValue = getIntField(jsonMessage,
            "ttl_line", // field name
            ttl_line,   // value to set
            1,                 // minimum value
            256); // maximum value

        if (foundValue)
            setTriggerSourceLine(source, ttl_line - 1, false);
    }

    if (jsonMessage->hasProperty("trigger_type"))
    {
        int trigger_type;
        bool foundValue = getIntField(jsonMessage,
            "trigger_type", // field name
            trigger_type,   // value to set
            1,                 // minimum value
            3); // maximum value

        if (foundValue)
            setTriggerSourceTriggerType(source, (TriggerType) trigger_type, false);
    }
    
    return "Message received.";
}

bool OnlinePSTH::getIntField(DynamicObject::Ptr payload, 
        String name, 
        int& value, 
        int lowerBound, 
        int upperBound) 
{
    if (!payload->hasProperty(name) || !payload->getProperty(name).isInt())
        return false;
    int tempVal = payload->getProperty(name);
    if ((upperBound != INT_MIN && tempVal > upperBound) || (lowerBound != INT_MAX && tempVal < lowerBound))
        return false;
    value = tempVal;
    return true;
}

void OnlinePSTH::timerCallback()
{
    stopTimer();

    for (auto source : getTriggerSources())
    {
        OnlinePSTHEditor* editor = (OnlinePSTHEditor*)getEditor();

        editor->updateConditionName(source);
    }

}


void OnlinePSTH::handleTTLEvent(TTLEventPtr event)
{
    
    for (auto source : triggerSources)
    {
        if (event->getLine() == source->line && event->getState() && source->canTrigger)
        {
            if (canvas != nullptr)
                canvas->pushEvent(source, event->getStreamId(), event->getSampleNumber());

            if (source->type == TTL_AND_MSG_TRIGGER)
				source->canTrigger = false;
        }
    }
    
}

void OnlinePSTH::handleSpike(SpikePtr spike)
{
   if (canvas != nullptr)
       canvas->pushSpike(spike->getChannelInfo(), spike->getSampleNumber(), spike->getSortedId());
}



void OnlinePSTH::saveCustomParametersToXml(XmlElement* xml)
{
    
	for (auto source : triggerSources)
	{
		XmlElement* sourceXml = xml->createNewChildElement("TRIGGERSOURCE");
		sourceXml->setAttribute("name", source->name);
		sourceXml->setAttribute("line", source->line);
		sourceXml->setAttribute("type", source->type);
        sourceXml->setAttribute("colour", source->colour.toString());
	}

    
}


void OnlinePSTH::loadCustomParametersFromXml(XmlElement* xml)
{
    
	for (auto sourceXml : xml->getChildIterator())
	{
		if (sourceXml->hasTagName("TRIGGERSOURCE"))
		{
			TriggerSource* source = addTriggerSource(sourceXml->getIntAttribute("line", 0),
				(TriggerType)sourceXml->getIntAttribute("type", TTL_TRIGGER));
			
            source->name = sourceXml->getStringAttribute("name");
			String savedColour = sourceXml->getStringAttribute("colour", "");
            
            if (savedColour.length() > 0)
				source->colour = Colour::fromString(savedColour);
		}
	}
}