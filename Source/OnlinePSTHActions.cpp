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
    triggerNames.clear();
    triggerIndices.insertMultiple(0, -1, triggerLines.size());
}

AddTriggerConditions::~AddTriggerConditions()
{
}

bool AddTriggerConditions::perform()
{
    for (int i = 0; i < triggerLines.size(); i++)
    {
        TriggerSource *source = psthProcessor->addTriggerSource(triggerLines[i], type, triggerIndices[i]);
        triggerSources.add(source);
    }

    if (triggerNames.isEmpty())
    {
        auto allSources = psthProcessor->getTriggerSources();
        for (int i = 0; i < triggerSources.size(); i++)
        {
            triggerNames.add(triggerSources[i]->name);
            triggerIndices.set(i, allSources.indexOf(triggerSources[i]));
        }
    }
    else
    {
        for (int i = 0; i < triggerSources.size(); i++)
        {
            triggerSources[i]->name = triggerNames[i];
        }
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
        triggerSources.clear();

        for (int i = 0; i < triggerLines.size(); i++)
            psthProcessor->removeTriggerSource(triggerIndices[i]);

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

    auto allSources = psthProcessor->getTriggerSources();
    for (auto source : triggerSourcesToRemove)
    {
        XmlElement *sourceXml = settings->createNewChildElement("SOURCE");
        sourceXml->setAttribute("name", source->name);
        sourceXml->setAttribute("line", source->line);
        sourceXml->setAttribute("type", source->type);
        sourceXml->setAttribute("colour", source->colour.toString());
        sourceXml->setAttribute("index", allSources.indexOf(source));
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
        for (auto *sourceXml : settings->getChildIterator())
        {
            int indexToRemove = sourceXml->getIntAttribute("index", -1);
            psthProcessor->removeTriggerSource(indexToRemove);
        }

        psthProcessor->registerUndoableAction(psthProcessor->getNodeId(), this);
        psthProcessor->getEditor()->updateSettings();
        CoreServices::sendStatusMessage("Removed " + String(triggerSourcesToRemove.size()) + " trigger condition(s)");

        triggerSourcesToRemove.clear();
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
        int savedIndex = sourceXml->getIntAttribute("index", -1);

        TriggerSource *source = psthProcessor->addTriggerSource(savedLine, (TriggerType)savedType, savedIndex);

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

RenameTriggerSource::RenameTriggerSource(OnlinePSTH *processor_,
                                         TriggerSource *source_,
                                         const String &newName_) : ProcessorAction("RenameTriggerSource"),
                                                                   psthProcessor(processor_),
                                                                   triggerSourcesToRename(source_),
                                                                   newName(newName_)
{
    triggerIndex = psthProcessor->getTriggerSources().indexOf(triggerSourcesToRename);
    oldName = triggerSourcesToRename->name;
}

RenameTriggerSource::~RenameTriggerSource()
{
}

void RenameTriggerSource::restoreOwner(GenericProcessor *processor)
{
    psthProcessor = (OnlinePSTH *)processor;
}

bool RenameTriggerSource::perform()
{
    auto source = psthProcessor->getTriggerSources()[triggerIndex];
    if (source != nullptr)
    {
        psthProcessor->setTriggerSourceName(source, newName);
        psthProcessor->registerUndoableAction(psthProcessor->getNodeId(), this);
        CoreServices::sendStatusMessage("Renamed trigger condition from " + oldName + " to " + newName);
    }

    return true;
}

bool RenameTriggerSource::undo()
{
    auto source = psthProcessor->getTriggerSources()[triggerIndex];
    if (source != nullptr)
    {
        psthProcessor->setTriggerSourceName(source, oldName);
        CoreServices::sendStatusMessage("Renamed trigger condition from " + newName + " to " + oldName);
    }

    return true;
}

ChangeTriggerTTLLine::ChangeTriggerTTLLine(OnlinePSTH *processor_,
                                           TriggerSource *source_,
                                           const int newLine_) : ProcessorAction("ChangeTriggerTTLLine"),
                                                                 psthProcessor(processor_),
                                                                 triggerSource(source_),
                                                                 newLine(newLine_)
{
    triggerIndex = psthProcessor->getTriggerSources().indexOf(triggerSource);
    oldLine = triggerSource->line;
}

ChangeTriggerTTLLine::~ChangeTriggerTTLLine()
{
}

void ChangeTriggerTTLLine::restoreOwner(GenericProcessor *processor)
{
    psthProcessor = (OnlinePSTH *)processor;
}

bool ChangeTriggerTTLLine::perform()
{
    auto source = psthProcessor->getTriggerSources()[triggerIndex];
    if (source != nullptr)
    {
        psthProcessor->setTriggerSourceLine(source, newLine);
        psthProcessor->registerUndoableAction(psthProcessor->getNodeId(), this);
        CoreServices::sendStatusMessage("Changed trigger condition line from " + String(oldLine) + " to " + String(newLine));
    }

    return true;
}

bool ChangeTriggerTTLLine::undo()
{
    auto source = psthProcessor->getTriggerSources()[triggerIndex];
    if (source != nullptr)
    {
        psthProcessor->setTriggerSourceLine(source, oldLine);
        CoreServices::sendStatusMessage("Changed trigger condition line from " + String(newLine) + " to " + String(oldLine));
    }

    return true;
}

ChangeTriggerType::ChangeTriggerType(OnlinePSTH *processor_,
                                     TriggerSource *source_,
                                     TriggerType newType_) : ProcessorAction("ChangeTriggerType"),
                                                             psthProcessor(processor_),
                                                             triggerSource(source_),
                                                             newType(newType_)
{
    triggerIndex = psthProcessor->getTriggerSources().indexOf(triggerSource);
    oldType = triggerSource->type;
}

ChangeTriggerType::~ChangeTriggerType()
{
}

void ChangeTriggerType::restoreOwner(GenericProcessor *processor)
{
    psthProcessor = (OnlinePSTH *)processor;
}

bool ChangeTriggerType::perform()
{
    auto source = psthProcessor->getTriggerSources()[triggerIndex];
    if (source != nullptr)
    {
        psthProcessor->setTriggerSourceTriggerType(source, newType);
        psthProcessor->registerUndoableAction(psthProcessor->getNodeId(), this);
        CoreServices::sendStatusMessage("Changed trigger condition type from " + String(oldType) + " to " + String(newType));
    }

    return true;
}

bool ChangeTriggerType::undo()
{
    auto source = psthProcessor->getTriggerSources()[triggerIndex];
    if (source != nullptr)
    {
        psthProcessor->setTriggerSourceTriggerType(source, oldType);
        CoreServices::sendStatusMessage("Changed trigger condition line from " + String(newType) + " to " + String(oldType));
    }

    return true;
}