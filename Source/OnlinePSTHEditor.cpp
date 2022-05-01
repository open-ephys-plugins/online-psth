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

#include <stdio.h>

OnlinePSTHEditor::OnlinePSTHEditor(GenericProcessor* parentNode)
    : VisualizerEditor(parentNode, "PSTH", 225), canvas(nullptr)

{
    addTextBoxParameterEditor("pre_ms", 20, 30);
    addTextBoxParameterEditor("post_ms", 20, 75);
    addTextBoxParameterEditor("bin_size", 125, 30);
    addComboBoxParameterEditor("trigger", 125, 75);
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
        
        if (channel->isValid())
            canvas->addSpikeChannel(channel);
    }
    
    canvas->setWindowSizeMs(processor->getPreWindowSizeMs(),
                            processor->getPostWindowSizeMs());
    
    canvas->setBinSizeMs(processor->getBinSizeMs());
    
    canvas->resized();
    
}
