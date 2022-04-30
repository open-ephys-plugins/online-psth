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
    /*g.setColour(Colours::snow);
    int histogramLen = getWidth()-230;
    int vertLineLen = 20;
    int textStart = vertLineLen+5;
    g.drawHorizontalLine(0, 30, histogramLen+30);

    g.drawVerticalLine(30, 0, vertLineLen);
    g.drawText(String(-1000.0*float(windowSize/2)/float(sampleRate)) + " ms", 0, textStart, 60, 10, Justification::centred);
    
    g.drawVerticalLine(histogramLen/4+30, 0, vertLineLen);
    g.drawText(String(-1000.0*float(windowSize/2)/2.0/float(sampleRate)) + " ms", histogramLen/4, textStart, 60, 10, Justification::centred);
    
    g.drawVerticalLine(histogramLen/2+30, 0, vertLineLen);
    g.drawText(" 0 ms",histogramLen/2, textStart, 60, 10, Justification::centred);
    
    g.drawVerticalLine(3*histogramLen/4+30, 0, vertLineLen);
    g.drawText(String(1000.0*float(windowSize/2)/2.0/float(sampleRate)) + " ms", 3*histogramLen/4, textStart, 60, 10, Justification::centred);
    
    g.drawVerticalLine(histogramLen+30, 0, vertLineLen);
    g.drawText(String(1000.0*float(windowSize/2)/float(sampleRate)) + " ms", histogramLen, textStart, 60, 10, Justification::centred);
    
    g.drawText(String(1000*float(bin*binSize)/float(sampleRate)) + "-" + String(1000*(float(bin+1)*binSize)/float(sampleRate)) + "ms, Spikes: " + String(data),histogramLen+30, 5, getWidth()-(histogramLen+30), getHeight(), Justification::right);*/
}


void Timescale::setWindowSizeMs(int pre, int post)
{
    pre_ms = pre;
    post_ms = post;
    
    repaint();
}
