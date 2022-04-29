/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2013 Open Ephys

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

#ifndef OnlinePSTHCANVAS_H_
#define OnlinePSTHCANVAS_H_

#include <VisualizerWindowHeaders.h>
#include "OnlinePSTH.h"

#include <vector>

/**

 Displays spikes aligned to TTL events
 
 @see OnlinePSTH, OnlinePSTHEditor
 
*/

class OnlinePSTHCanvasHolder;
class OnlinePSTHDisplay;
class Timescale;
class GraphUnit;
class LabelDisplay;
class HistoGraph;
class StatDisplay;


/** 

    Visualizer for spike histograms

*/
class OnlinePSTHCanvas : 
    public Visualizer, 
    public Button::Listener
{
public:
    OnlinePSTHCanvas(OnlinePSTH* n);
    ~OnlinePSTHCanvas();
    
    void paint(Graphics& g);
    void repaintDisplay();
    void refresh();
    void beginAnimation();
    void endAnimation();
    void refreshState();
    void setParameter(int, float) {}
    void setParameter(int, int, int, float) {}
    void update();
    void resized();
    bool keyPressed(const KeyPress& key);
    void buttonClicked(Button* button);
    void startRecording() { }; // unused
    void stopRecording() { }; // unused
    void setBin(int bin_);
    void setBinSize(int binSize_);
    void setData(int data_);
    OnlinePSTH* processor;

private:

    Array<uint64*> histoData;
    std::vector<std::vector<float>> minMaxMean;
    void removeUnitOrBox();
    ScopedPointer<Viewport> viewport;
    ScopedPointer<OnlinePSTHDisplay> display;
    ScopedPointer<UtilityButton> clearHisto;
    int scrollBarThickness;
    int border = 20;
    int triggerChannel = -1;
    unsigned long spikeBufferSize = 0;
    unsigned long ttlBufferSize = 0;
    int bin = 0;
    int binSize = 0;
    int data = 0;
    OnlinePSTHCanvasHolder* holder;
    ScopedPointer<Timescale> scale;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OnlinePSTHCanvas);

};

//---------------------------

class OnlinePSTHDisplay : public Component
{
    
public:
    OnlinePSTHDisplay(OnlinePSTHCanvas* c, Viewport* v, OnlinePSTH* p);
    ~OnlinePSTHDisplay();
    void visibleAreaChanged (const Rectangle<int>& newVisibleArea);
    
    /** Callback method that is called when the viewed component is added, removed or swapped. */
    void viewedComponentChanged (Component* newComponent);
    void resized();
    void paint(Graphics &g);
    void refresh();
    int getNumGraphs();
private:

    OnlinePSTH* processor;
    OnlinePSTHCanvas* canvas;
    Viewport* viewport;
    std::vector<GraphUnit*> graphs;
    juce::Colour channelColours[16];
    Array<uint64 *> histoData;
    Array<float *> minMaxMean;
    int border = 20;
};

//---------------------------

class Timescale : public Component
{
public:
    Timescale(int windowSize_, uint64 sampleRate_, int data = 0, int bin = 0 ,int binSize = 0 );
    ~Timescale();
    void paint(Graphics& g);
    void resized();
    void update(int windowSize, uint64 sR);
    void setBin(int bin_);
    void setData(int data_);
    void setBinSize(int binSize_);
private:
    int windowSize;
    uint64 sampleRate;
    int bin = 0;
    int binSize = 0;
    int data = 0;
    int lowerBin = 0;
    int upperBin = 0;
    
};


//---------------------------


class GraphUnit : public Component
{
public:
    GraphUnit(OnlinePSTH* processor_,OnlinePSTHCanvas* canvas_,juce::Colour color_, String name_, float * stats_, uint64 * data_);
    ~GraphUnit();
    void paint(Graphics& g);
    void resized();
private:
    LabelDisplay* LD;
    HistoGraph* HG;
    StatDisplay* SD;
    Colour color;
};

//---------------------------

class LabelDisplay : public Component
{
public:
    LabelDisplay(Colour color,String n);
    ~LabelDisplay();
    void paint(Graphics& g);
    void resized();
private:
    Colour color;
    String name;
};

//---------------------------

class HistoGraph : public Component
{
    
public:
    HistoGraph(OnlinePSTH* processor_,OnlinePSTHCanvas* canvas_,juce::Colour color_, uint64 bins_, float max_, uint64 * histoData_);
    ~HistoGraph();
    
    void paint(Graphics& g);
    void resized();
    
    void select();
    void deselect();
    void clear();
    void mouseMove(const MouseEvent &event);
    
    
private:
    uint64 bins = 0;
    Colour color;
    float max;
    uint64 const * histoData;
    int valueY=0;
    OnlinePSTH* processor;
    OnlinePSTHCanvas* canvas;
    
};

//---------------------------

class StatDisplay : public Component
{
public:
    StatDisplay(OnlinePSTH* display_, juce::Colour c, float * s);
    ~StatDisplay();
    void paint(Graphics& g);
    void resized();
private:
    OnlinePSTH* processor;
    Colour color;
    float const * stats;
    
};


#endif  // OnlinePSTHCANVAS_H_
