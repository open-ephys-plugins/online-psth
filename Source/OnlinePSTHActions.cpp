/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2024 Open Ephys

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

#include <stdio.h>

#include "OnlinePSTHActions.h"

AddTriggerConditions::AddTriggerConditions(OnlinePSTH *processor_,
                                           Array<int> lines,
                                           TriggerType type_) : ProcessorAction("AddTriggerConditions"),
                                                                psthProcessor(processor_),
                                                                triggerLines(lines),
                                                                type(type_)
{
    triggerSources.clear();
}

AddTriggerConditions::~AddTriggerConditions()
{
}

bool AddTriggerConditions::perform()
{
    for (int i = 0; i < triggerLines.size(); i++)
    {
        TriggerSource *source = psthProcessor->addTriggerSource(triggerLines[i], type);
        triggerSources.add(source);
    }

    psthProcessor->registerUndoableAction(psthProcessor->getNodeId(), this);
    CoreServices::sendStatusMessage("Added " + String(triggerLines.size()) + " trigger condition(s)");
    psthProcessor->getEditor()->updateSettings();

    return true;
}

bool AddTriggerConditions::undo()
{
    if (triggerLines.size() > 0)
    {
        psthProcessor->removeTriggerSources(triggerSources);
        psthProcessor->getEditor()->updateSettings();
        CoreServices::sendStatusMessage("Removed " + String(triggerLines.size()) + " trigger condition(s)");
    }

    return true;
}

void AddTriggerConditions::restoreOwner(GenericProcessor *owner)
{
    LOGD("RESTORING OWNER FOR: AddTriggerConditions");
    psthProcessor = (OnlinePSTH *)owner;
}

RemoveTriggerConditions::RemoveTriggerConditions(OnlinePSTH *processor_,
                                                 Array<TriggerSource *> triggerSourcesToRemove_) : ProcessorAction("RemoveTriggerConditions"),
                                                                                                   psthProcessor(processor_),
                                                                                                   triggerSourcesToRemove(triggerSourcesToRemove_)
{
    settings = std::make_unique<XmlElement>("TRIGGER_SOURCES");

    for (auto source : triggerSourcesToRemove)
    {
        XmlElement *sourceXml = settings->createNewChildElement("SOURCE");
        sourceXml->setAttribute("name", source->name);
        sourceXml->setAttribute("line", source->line);
        sourceXml->setAttribute("type", source->type);
        sourceXml->setAttribute("colour", source->colour.toString());
    }
}

RemoveTriggerConditions::~RemoveTriggerConditions()
{
}

void RemoveTriggerConditions::restoreOwner(GenericProcessor *processor)
{
    psthProcessor = (OnlinePSTH *)processor;
}

bool RemoveTriggerConditions::perform()
{
    if (triggerSourcesToRemove.size() > 0)
    {
        psthProcessor->removeTriggerSources(triggerSourcesToRemove);

        psthProcessor->registerUndoableAction(psthProcessor->getNodeId(), this);
        psthProcessor->getEditor()->updateSettings();
        CoreServices::sendStatusMessage("Removed " + String(triggerSourcesToRemove.size()) + " trigger condition(s)");
    }

    return true;
}

bool RemoveTriggerConditions::undo()
{
    triggerSourcesToRemove.clear();
    for (auto *sourceXml : settings->getChildIterator())
    {
        String savedName = sourceXml->getStringAttribute("name");
        int savedLine = sourceXml->getIntAttribute("line", 0);
        int savedType = sourceXml->getIntAttribute("type", TTL_TRIGGER);
        String savedColour = sourceXml->getStringAttribute("colour", "");

        TriggerSource *source = psthProcessor->addTriggerSource(savedLine, (TriggerType)savedType);

        if (savedName.isNotEmpty())
            source->name = savedName;

        if (savedColour.length() > 0)
            source->colour = Colour::fromString(savedColour);

        triggerSourcesToRemove.add(source);
    }

    CoreServices::sendStatusMessage("Added " + String(triggerSourcesToRemove.size()) + " trigger condition(s)");
    psthProcessor->getEditor()->updateSettings();
    return true;
}
