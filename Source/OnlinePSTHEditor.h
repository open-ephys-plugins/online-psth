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

#ifndef __OnlinePSTHEDITOR_H_F0BD2DD9__
#define __OnlinePSTHEDITOR_H_F0BD2DD9__

#include <VisualizerEditorHeaders.h>

#include "OnlinePSTH.h"

class PopupConfigurationWindow;

class OnlinePSTHCanvas;

/**
 
User interface for OnlinePSTH

@see OnlinePSTH, OnlinePSTHCanvas
 */
class OnlinePSTHEditor : public VisualizerEditor,
                         public Button::Listener
{
public:
    
    /** Constructor */
    OnlinePSTHEditor(GenericProcessor* parentNode);

    /** Destructor */
    ~OnlinePSTHEditor() { }

    /** Creates the visualizer */
    Visualizer* createNewCanvas() override;

    /** Called when signal chain is updated */
    void updateSettings() override;

    /** Called when configure button is clicked */
    void buttonClicked(Button* button) override;

    /** Adds triggers with a given type */
	void addTriggerSources(PopupConfigurationWindow* window, Array<int> lines, TriggerType type);

    /** Removes triggers based on an array of pointers to trigger objects*/
    void removeTriggerSources(PopupConfigurationWindow* window, Array<TriggerSource*> triggerSourcesToRemove);
    
private:

    std::unique_ptr<UtilityButton> configureButton;
    
    OnlinePSTHCanvas* canvas;

    PopupConfigurationWindow* currentConfigWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OnlinePSTHEditor);

};




#endif  // __OnlinePSTHEDITOR_H_F0BD2DD9__
