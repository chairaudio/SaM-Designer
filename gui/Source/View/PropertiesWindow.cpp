/*
  ==============================================================================

    PropertiesWindow.cpp
    Created: 23 Jul 2013 10:12:43pm
    Author:  Peter Vasil

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

#include "../Application/CommonHeaders.h"
#include "../Application/MainWindow.h"

#include "ContentComp.h"
#include "ObjectsHolder.h"
#include "../Controller/ObjController.h"
#include "../Graph/Node.h"
#include "BaseObjectComponent.h"
#include "ObjectPropertiesComponent.h"
#include "SelectableObject.h"
#include "ObjectComponent.h"
#include "LinkComponent.h"

#include "PropertiesWindow.h"
#include "AudioOutConnector.h"

using namespace synthamodeler;

class EmptyComponent : public Component
{
public:
    EmptyComponent() : Component() {}
    EmptyComponent(const String& compName) : Component(compName) {}
    void paint(Graphics& g)
    {
        g.drawText("No object selected", getWidth()/2 - 75, getHeight()/2 - 25,
                   150, 50, Justification::centred,false);
    }
};

PropertiesWindow::PropertiesWindow()
:
DocumentWindow("Properties", Colour::greyLevel (0.92f), DocumentWindow::closeButton, true)
{
    Desktop::getInstance().addFocusChangeListener(this);

    setSize(400, 300);

    // restore the last size and position from our settings file..
	restoreWindowStateFromString (StoredSettings::getInstance()->getProps()
	                                    .getValue ("lastPropertiesWindowPos"));

    setContentOwned(new EmptyComponent(), false);
    setResizable(true, true);
}

PropertiesWindow::~PropertiesWindow()
{
    StoredSettings::getInstance()->getProps()
        .setValue ("lastPropertiesWindowPos", getWindowStateAsString());

    clearContentComponent();
}

void PropertiesWindow::makeVisible(bool shouldBeVisible)
{
	setVisible(shouldBeVisible);
    if (shouldBeVisible)
    {
        toFront(true);
        if (currentSelection)
        {
            updateProperties();
        }
    }
}

void PropertiesWindow::globalFocusChanged (Component* focusedComponent)
{
    if(ContentComp* cc = dynamic_cast<ContentComp*>(focusedComponent))
    {
        currentContentComp = cc;
        currentSelection = &cc->getHolderComponent()->getObjController().getSelectedObjects();
        DBG("focus changed");
    }

}

void PropertiesWindow::changeListenerCallback(ChangeBroadcaster* /*source*/)
{
    if(! isVisible())
        return;

    if(currentSelection)
    {
        DBG("Selection changed");
        updateProperties();
    }
}
void PropertiesWindow::closeButtonPressed()
{
    setVisible(false);
}

bool PropertiesWindow::keyPressed(const KeyPress& kp)
{
    if(kp == KeyPress::escapeKey)
    {
        closeButtonPressed();
        return true;
    }
    return false;
}

void PropertiesWindow::updateProperties()
{
    if(currentSelection->getNumSelected() > 0)
    {
        Array<ValueTree> datas;
        StringArray audioSourceIds;
        const Array<SelectableObject*>& selectedItems = currentSelection->getItemArray();
        Identifier selectedId;
        for (int i = 0; i < selectedItems.size(); ++i)
        {
            if(BaseObjectComponent* boc = dynamic_cast<BaseObjectComponent*>(selectedItems[i]))
            {
                // get first id type. This is the type for a multiple selection
                if(datas.size() == 0)
                    selectedId = boc->getData().getType();

                if(selectedId == boc->getData().getType())
                    datas.add(boc->getData());
            }
            else if(AudioOutConnector* aoc = dynamic_cast<AudioOutConnector*>(selectedItems[i]))
            {
                if (datas.size() == 0)
                    selectedId = Ids::audioconnector;

                String sourceId;
                if (ObjectComponent * const oc = dynamic_cast<ObjectComponent*> (aoc->getSourceObject()))
                    sourceId = oc->getData()[Ids::identifier].toString();
                else if (LinkComponent * const lc = dynamic_cast<LinkComponent*> (aoc->getSourceObject()))
                    sourceId = lc->getData()[Ids::identifier].toString();

                audioSourceIds.add(sourceId);
                datas.add(aoc->getAudioObject()->getData());
            }
        }

        if(datas.size() == 0)
            return;
        
        Component* comp;
        ObjController* objCtrl = &currentContentComp->getHolderComponent()->getObjController();
        UndoManager* undoManager_ = objCtrl->getUndoManager();

        if (selectedId == Ids::mass)
        {
            comp = new MassPropertiesComponent(objCtrl, datas, undoManager_);
        }
        else if (selectedId == Ids::port)
        {
            comp = new PortPropertiesComponent(objCtrl, datas, undoManager_);
        }
        else if (selectedId == Ids::resonators)
        {
            comp = new ResonatorPropertiesComponent(objCtrl, datas, undoManager_);
        }
        else if (selectedId == Ids::ground)
        {
            comp = new GroundPropertiesComponent(objCtrl, datas, undoManager_);
        }
        else if (selectedId == Ids::link
            || selectedId == Ids::touch
            || selectedId == Ids::pluck
            || selectedId == Ids::pulsetouch)
        {
            comp = new LinkPropertiesComponent(objCtrl, datas, undoManager_);
        }
        else if (selectedId == Ids::waveguide)
        {
            comp = new WaveguidePropertiesComponent(objCtrl, datas, undoManager_);
        }
        else if (selectedId == Ids::junction)
        {
            comp = new JunctionPropertiesComponent(objCtrl, datas, undoManager_);
        }
        else if (selectedId == Ids::termination)
        {
            comp = new TerminationPropertiesComponent(objCtrl, datas, undoManager_);
        }
        else if (selectedId == Ids::audioout)
        {
            comp = new AudiooutPropertiesComponent(objCtrl, datas, undoManager_);
        }
        else if (selectedId == Ids::audioconnector)
        {
            comp = new GainComponent(audioSourceIds, datas, undoManager_);
        }
        else
        {
            comp = new EmptyComponent();
            setName("Properties");
        }
        setContentOwned(comp, false);
    }
    else
    {
        setContentOwned(new EmptyComponent(), false);
        setName("Properties");
    }
}

void PropertiesWindow::mdlChanged()
{
    if(isVisible())
        if (ObjectPropertiesComponent * opc = dynamic_cast<ObjectPropertiesComponent*> (getContentComponent()))
            opc->readValues();
}
//==============================================================================
void PropertiesWindow::valueTreePropertyChanged (ValueTree& /*tree*/,
                                                 const Identifier& /*property*/)
{
    mdlChanged();
}

void PropertiesWindow::valueTreeChildAdded (ValueTree& /*parentTree*/,
                                            ValueTree& /*childWhichHasBeenAdded*/)
{
	mdlChanged();
}

void PropertiesWindow::valueTreeChildRemoved (ValueTree& /*parentTree*/,
                                              ValueTree& /*childWhichHasBeenRemoved*/)
{
	mdlChanged();
}

void PropertiesWindow::valueTreeChildOrderChanged (ValueTree& /*parentTree*/)
{
	mdlChanged();
}

void PropertiesWindow::valueTreeParentChanged (ValueTree& /*tree*/)
{
    mdlChanged();
}

//==============================================================================