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

#ifndef SpikeDetectorActions_h
#define SpikeDetectorActions_h

#include <ProcessorHeaders.h>

#include "OnlinePSTH.h"
#include "PopupConfigurationWindow.h"

/**
    Adds a spike channel to the spike detector,
    based on the description.

    Undo: removes the spike channel from the
    spike detector.
*/
class AddTriggerConditions : public ProcessorAction
{
public:
    /** Constructor*/
    AddTriggerConditions(OnlinePSTH *processor,
                         Array<int> triggerLines,
                         TriggerType type);

    /** Destructor */
    ~AddTriggerConditions();

    void restoreOwner(GenericProcessor *processor) override;

    /** Perform the action*/
    bool perform() override;

    /** Undo the action*/
    bool undo() override;

    std::unique_ptr<XmlElement> settings;

private:
    OnlinePSTH *psthProcessor;
    Array<int> triggerLines;
    TriggerType type;
    Array<TriggerSource *> triggerSources;
};

class RemoveTriggerConditions : public ProcessorAction
{
public:
    /** Constructor*/
    RemoveTriggerConditions(OnlinePSTH *processor,
                            Array<TriggerSource *> triggerSourcesToRemove);

    /** Destructor */
    ~RemoveTriggerConditions();

    void restoreOwner(GenericProcessor *processor) override;

    /** Perform the action*/
    bool perform() override;

    /** Undo the action*/
    bool undo() override;

    std::unique_ptr<XmlElement> settings;

private:
    OnlinePSTH *psthProcessor;
    Array<TriggerSource *> triggerSourcesToRemove;
};

#endif /* SpikeDetectorActions_h */
