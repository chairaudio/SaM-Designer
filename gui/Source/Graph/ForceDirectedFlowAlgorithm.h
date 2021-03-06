/*
  ==============================================================================

    ForceDirectedFlowAlgorithm.h
    Created: 13 Nov 2012 6:48:50pm
    Author:  Peter Vasil
    Source: http://processingjs.nihongoresources.com/graphs/

  ==============================================================================

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

*/

#ifndef __FORCEDIRECTEDFLOWALGORITHM_H_F765C202__
#define __FORCEDIRECTEDFLOWALGORITHM_H_F765C202__

#include "DirectedGraph.h"
#include "FlowAlgorithm.h"

namespace synthamodeler
{

class Node;

class ForceDirectedFlowAlgorithm : public FlowAlgorithm
{
public:
    ForceDirectedFlowAlgorithm();
    virtual ~ForceDirectedFlowAlgorithm();

    void initParameters();

    bool reflow(DirectedGraph& g, const Rectangle<int>& area,
                ObjController& objController, float deltaTime);

private:

    void applyForces(Point<float>& totalEnergy, tNodesAndEdges& group,
                     int width, int height, ObjController& objController);

    float mass;
    float charge;
    float k;
    float damp;
    float stopEnergy;
};


}


#endif  // __FORCEDIRECTEDFLOWALGORITHM_H_F765C202__
