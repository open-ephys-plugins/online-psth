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
class OnlinePSTH;

/**
 
User interface for OnlinePSTH

@see OnlinePSTH, OnlinePSTHCanvas
 */

class OnlinePSTHEditor : public VisualizerEditor,
    public Label::Listener,
    public ComboBox::Listener,
    public Button::Listener,
    public Slider::Listener
{
    
public:
    
    /** Constructor */
    OnlinePSTHEditor(GenericProcessor* parentNode);

    /** Destructor */
    virtual ~OnlinePSTHEditor();

    /** Creates the visualizer */
    Visualizer* createNewCanvas() override;

    /** Respond to button events */
    void buttonClicked(Button* button) override;

    /** Label events */
    void labelTextChanged(Label* label) override;

    /** ComboBox event listener */
    void comboBoxChanged(ComboBox* comboBox) override;

    /** Slider event listener */
    void sliderValueChanged(Slider* slider) override;

    /** Called when signal chain is updated */
    void updateSettings() override;
    void setTrigger(int val);
    void setBin(int val);
    void setWindow(int val);

    OnlinePSTHCanvas* canvas;
    
private:
    struct EventSources
    {
        unsigned int eventIndex;
        unsigned int channel;
    };

    std::vector<EventSources> eventSourceArray;
    
    OnlinePSTH* processor;
    ScopedPointer<ComboBox> triggerChannel;
    ScopedPointer<Label> binSize, windowSize, channelLabel, binLabel, windowLabel;
    Font font;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OnlinePSTHEditor);

};




#endif  // __OnlinePSTHEDITOR_H_F0BD2DD9__
