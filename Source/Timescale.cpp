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

#include "Timescale.h"


void Timescale::paint(Graphics& g)
{
    const float histogramWidth = getWidth() - 40;
    
    float zeroLoc = float(pre_ms) / float(pre_ms + post_ms) * histogramWidth;
    
    g.setColour(Colours::white);
    //g.drawLine(0, getHeight()-3, histogramWidth, getHeight()-3, 3.0);
    g.drawLine(zeroLoc, 0, zeroLoc, getHeight(), 2.0);
    
    float window_size_ms = float(pre_ms + post_ms);
    float stepSize;
    
    if (window_size_ms == 20.0f)
        stepSize = 1.0f;
    else if (window_size_ms > 20.0f && window_size_ms <= 50.0f)
        stepSize = 5.0f;
    else if (window_size_ms > 50.0f && window_size_ms <= 100.0f)
        stepSize = 10.0f;
    else if (window_size_ms > 100.0f && window_size_ms <= 250.0f)
        stepSize = 25.0f;
    else if (window_size_ms > 250.0f && window_size_ms <= 500.0f)
        stepSize = 50.0f;
    else if (window_size_ms > 500.0f && window_size_ms <= 1000.0f)
        stepSize = 100.0f;
    else if (window_size_ms >= 1000.0f && window_size_ms < 2000.0f)
        stepSize = 250.0f;
    else
        stepSize = 500.0f;
    
    float tickDistance = (stepSize / window_size_ms) * histogramWidth;
    
    float tick = stepSize;
    float tickLoc = zeroLoc + tickDistance;
    
    while (tick < post_ms)
    {
        g.drawLine(tickLoc, getHeight(), tickLoc, getHeight()-8, 2.0);
        g.drawText(String(tick), tickLoc-50, getHeight()-25, 100, 15, Justification::centred);
        tick += stepSize;
        tickLoc += tickDistance;
        
    }
    
    tick = -stepSize;
    tickLoc = zeroLoc - tickDistance;
    
    while (tick > -pre_ms)
    {
        g.drawLine(tickLoc, getHeight(), tickLoc, getHeight()-8, 2.0);
        g.drawText(String(tick), tickLoc-54, getHeight()-25, 100, 15, Justification::centred);
        tick -= stepSize;
        tickLoc -= tickDistance;
        
    }
    
}


void Timescale::setWindowSizeMs(int pre, int post)
{
    pre_ms = pre;
    post_ms = post;
    
    repaint();
}
