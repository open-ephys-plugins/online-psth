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

class OnlinePSTHCanvas;

/**
 
User interface for OnlinePSTH

@see OnlinePSTH, OnlinePSTHCanvas
 */
class OnlinePSTHEditor : public VisualizerEditor
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
    
private:
    
    OnlinePSTHCanvas* canvas;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OnlinePSTHEditor);

};




#endif  // __OnlinePSTHEDITOR_H_F0BD2DD9__
