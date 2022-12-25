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
                    "trigger",
                    "The input TTL line to trigger on",
                    1, 1, 16);
    
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
	triggerSources.add(source);
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

void OnlinePSTH::setTriggerSourceName(TriggerSource* source, String name)
{
    source->name = name;
}

void OnlinePSTH::setTriggerSourceLine(TriggerSource* source, int line)
{
    source->line = line;
}

void OnlinePSTH::setTriggerSourceTriggerType(TriggerSource* source, TriggerType type)
{
    source->type = type;
}

void OnlinePSTH::process(AudioBuffer<float>& buffer)
{
    checkForEvents(true);
}

void OnlinePSTH::handleTTLEvent(TTLEventPtr event)
{
    
    for (auto source : triggerSources)
    {
        if (event->getLine() == source->line - 1 && event->getState())
        {
            if (canvas != nullptr)
                canvas->pushEvent(source, event->getStreamId(), event->getSampleNumber());
        }
    }
    
}

void OnlinePSTH::handleSpike(SpikePtr spike)
{
   if (canvas != nullptr)
       canvas->pushSpike(spike->getChannelInfo(), spike->getSampleNumber(), spike->getSortedId());
}

