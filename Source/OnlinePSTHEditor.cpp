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

#include "OnlinePSTHEditor.h"

#include "OnlinePSTHCanvas.h"
#include "OnlinePSTH.h"

#include "PopupConfigurationWindow.h"

#include <stdio.h>

OnlinePSTHEditor::OnlinePSTHEditor(GenericProcessor* parentNode)
    : VisualizerEditor(parentNode, "PSTH", 225), 
      canvas(nullptr),
      currentConfigWindow(nullptr)

{
    addTextBoxParameterEditor("pre_ms", 20, 30);
    addTextBoxParameterEditor("post_ms", 20, 75);
    addTextBoxParameterEditor("bin_size", 125, 30);

    configureButton = std::make_unique<UtilityButton>("configure", titleFont);
    configureButton->addListener(this);
    configureButton->setRadius(3.0f);
    configureButton->setBounds(125, 85, 80, 30);
    addAndMakeVisible(configureButton.get());
}

Visualizer* OnlinePSTHEditor::createNewCanvas()
{

    OnlinePSTH* processor = (OnlinePSTH*) getProcessor();
    
    canvas = new OnlinePSTHCanvas();
    processor->canvas = canvas;
    
    updateSettings();
    
    return canvas;
}

void OnlinePSTHEditor::updateSettings()
{
    
    if (canvas == nullptr)
        return;
    
    canvas->prepareToUpdate();
    
    OnlinePSTH* processor = (OnlinePSTH*) getProcessor();

    for (int i = 0; i < processor->getTotalSpikeChannels(); i++)
    {
        const SpikeChannel* channel = processor->getSpikeChannel(i);
            
        for (auto source : processor->getTriggerSources())
        {
            if (channel->isValid())
            {
                canvas->addSpikeChannel(channel, source);
                //LOGD("Editor adding ", channel->getName(), " for ", source->name);
            }
                
        }
    }

    canvas->setWindowSizeMs(processor->getPreWindowSizeMs(),
                            processor->getPostWindowSizeMs());
    
    canvas->setBinSizeMs(processor->getBinSizeMs());
    
    canvas->resized();
    
}


void OnlinePSTHEditor::updateColours(TriggerSource* source)
{

    if (canvas == nullptr)
        return;
    
    OnlinePSTH* processor = (OnlinePSTH*)getProcessor();

    canvas->updateColourForSource(source);
}


void OnlinePSTHEditor::buttonClicked(Button* button)
{

    if (button == configureButton.get())
    {

        OnlinePSTH* processor = (OnlinePSTH*) getProcessor();

        Array<TriggerSource*> triggerLines = processor->getTriggerSources();
        std::cout << triggerLines.size() << " trigger sources found." << std::endl;

        currentConfigWindow = new PopupConfigurationWindow(this,
            triggerLines,
            acquisitionIsActive);

        CallOutBox& myBox
            = CallOutBox::launchAsynchronously(std::unique_ptr<Component>(currentConfigWindow),
                button->getScreenBounds(),
                nullptr);

        myBox.setDismissalMouseClicksAreAlwaysConsumed(true);

        return;
    }

}


void OnlinePSTHEditor::addTriggerSources(PopupConfigurationWindow* window, Array<int> lines, TriggerType type)
{
	OnlinePSTH* processor = (OnlinePSTH*)getProcessor();
	
	for (int i = 0; i < lines.size(); i++)
	{
		TriggerSource* source = processor->addTriggerSource(lines[i], type);
        
	}

    if (window != nullptr)
        window->update(processor->getTriggerSources());

    updateSettings();
}


void OnlinePSTHEditor::removeTriggerSources(PopupConfigurationWindow* window, Array<TriggerSource*> triggerSourcesToRemove)
{
    OnlinePSTH* processor = (OnlinePSTH*)getProcessor();

    processor->removeTriggerSources(triggerSourcesToRemove);

    if (window != nullptr)
        window->update(processor->getTriggerSources());

    updateSettings();
}