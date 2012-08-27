/*
  ==============================================================================

    ObjectsHolder.cpp
    Created: 11 Apr 2012 5:10:20pm
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
#include "../Models/MDLFile.h"
#include "../Models/ObjectFactory.h"
#include "ContentComp.h"
#include "ObjectComponent.h"
#include "LinkComponent.h"
#include "../Controller/ObjController.h"
#include "VariablesPanel.h"

#include "ObjectsHolder.h"
#include "AudioOutConnector.h"

ObjectsHolder::ObjectsHolder(ObjController& objController_)
: objController(objController_),
mdlFile(nullptr),
dragging(false)
{
    setSize(100, 100);
}

ObjectsHolder::~ObjectsHolder()
{
    if (mdlFile != nullptr)
    {
        mdlFile->removeChangeListener(this);
    }
}

void ObjectsHolder::paint(Graphics& g)
{
    g.fillAll(Colours::white);

    g.setColour(Colours::black);
}

void ObjectsHolder::resized()
{
    updateComponents();
}

void ObjectsHolder::changeListenerCallback(ChangeBroadcaster*)
{
    updateComponents();
}

void ObjectsHolder::updateComponents()
{
    // TODO Find faster solution for updating componenans
    int i;
    Array<LinkComponent*> links;
    Array<AudioOutConnector*> aocs;
    for (i = getNumChildComponents(); --i >= 0;)
    {
        ObjectComponent * const bobj = dynamic_cast<ObjectComponent*> (getChildComponent(i));

        if (bobj != nullptr)
            bobj->update();
        
        LinkComponent * const lobj = dynamic_cast<LinkComponent*> (getChildComponent(i));
        
        if (lobj != nullptr)
        {
            links.add(lobj);
        }
        AudioOutConnector * const aobj = dynamic_cast<AudioOutConnector*> (getChildComponent(i));
        
        if (aobj != nullptr)
        {
            aocs.add(aobj);
        }
    }
    for (i = 0; i < aocs.size(); ++i)
    {
        aocs[i]->update();
        aocs[i]->toBack();
    }
    for (i = 0; i < links.size(); ++i)
    {
        links[i]->update();
        links[i]->toBack();
    }

}

void ObjectsHolder::mouseDrag(const MouseEvent& e)
{
    lassoComp.toFront (false);
    lassoComp.dragLasso (e);
}

void ObjectsHolder::mouseUp(const MouseEvent& e)
{
    if (e.mouseWasClicked())
    {
    }
    
    if (e.mouseWasClicked() && ! e.mods.isAnyModifierKeyDown())
    {
        // object changed
        objController.getSelectedObjects().deselectAll();
        objController.getSelectedLinks().deselectAll();
        objController.getSelectedAudioConnections().deselectAll();
    }
    lassoComp.endLasso();
}

void ObjectsHolder::mouseDown(const MouseEvent& e)
{
    if (e.mods.isPopupMenu() && objController.getSelectedObjects().getNumSelected() == 2)
    {
        String startObj;
        String endObj;
        if (objController.getSelectedObjects().getNumSelected() == 2)
        {
            startObj = objController.getSelectedObjects().getItemArray()[0]->getData().getProperty(Ids::identifier).toString();
            endObj = objController.getSelectedObjects().getItemArray()[1]->getData().getProperty(Ids::identifier).toString();
            DBG(String("Link: ") + startObj + String(", ") + endObj);
            showLinkPopupMenu(startObj, endObj);
        }

    }
    else if (e.mods.isPopupMenu())
    {
        showContextMenu(e.getMouseDownPosition());
    }
    else
    {
        addAndMakeVisible(&lassoComp);
        lassoComp.beginLasso(e, this);
    }
}

void ObjectsHolder::setMDLFile(MDLFile* newMDLFile)
{
    if (newMDLFile != mdlFile && newMDLFile != nullptr)
    {
        mdlFile = newMDLFile;
        mdlFile->addChangeListener(this);
        objController.loadComponents(this);
    }
}

bool ObjectsHolder::dispatchMenuItemClick(const ApplicationCommandTarget::InvocationInfo& info)
{
    Point<int> mp = getMouseXYRelative();

    if (mp.x < 0)
        mp.x = 0;
    else if (mp.x > getWidth())
        mp.x = getWidth();
    if (mp.y < 0)
        mp.y = 0;
    else if (mp.y > getHeight())
        mp.y = getHeight();

    String startObj;
    String endObj;
    if(objController.getSelectedObjects().getNumSelected() == 2)
    {
        startObj = objController.getSelectedObjects().getItemArray()[0]->getData().getProperty(Ids::identifier).toString();
        endObj = objController.getSelectedObjects().getItemArray()[1]->getData().getProperty(Ids::identifier).toString();
//        DBG(String("Link: ") + startObj + String(", ") + endObj);
    }
    switch (info.commandID)
    {
    case StandardApplicationCommandIDs::cut:
        objController.cut(this);
        break;
    case StandardApplicationCommandIDs::copy:
        objController.copySelectedToClipboard();
        break;
    case StandardApplicationCommandIDs::paste:
        objController.paste(this);
        break;
    case StandardApplicationCommandIDs::selectAll:
        objController.selectAll(true);
        break;
    case StandardApplicationCommandIDs::deselectAll:
        objController.selectAll(false);
        break;
    case StandardApplicationCommandIDs::del:
        deleteSelectedObjects();
        break;
    case CommandIDs::defineVariables:
        VariablesPanel::show(mdlFile->mdlRoot, &mdlFile->getUndoMgr());
        break;
    case CommandIDs::segmentedConnectors:
        // TODO: implement segmented connectors
        StoredSettings::getInstance()->setIsSegmentedConnectors(!StoredSettings::getInstance()->getIsSegmentedConnectors());
        break;
    case CommandIDs::reverseDirection:
        objController.reverseLinkDirection();
        break;

    case CommandIDs::insertMass:
        objController.addNewObject(this, ObjectFactory::createNewObjectTree(Ids::mass, mp.x, mp.y));
        break;
    case CommandIDs::insertGround:
        objController.addNewObject(this, ObjectFactory::createNewObjectTree(Ids::ground, mp.x, mp.y));
        break;
    case CommandIDs::insertResonator:
        objController.addNewObject(this, ObjectFactory::createNewObjectTree(Ids::resonator, mp.x, mp.y));
        break;
    case CommandIDs::insertPort:
        objController.addNewObject(this, ObjectFactory::createNewObjectTree(Ids::port, mp.x, mp.y));
        break;

    case CommandIDs::insertLink:
        
        objController.addNewLinkIfPossible(this, ObjectFactory::createNewLinkObjectTree(Ids::link, startObj, endObj));
        break;
    case CommandIDs::insertTouch:
        objController.addNewLinkIfPossible(this, ObjectFactory::createNewLinkObjectTree(Ids::touch, startObj, endObj));
        break;
    case CommandIDs::insertPluck:
        objController.addNewLinkIfPossible(this, ObjectFactory::createNewLinkObjectTree(Ids::pluck, startObj, endObj));
        break;

    case CommandIDs::insertAudioOutput:
        objController.addNewObject(this, ObjectFactory::createNewObjectTree(Ids::audioout, mp.x, mp.y));
        break;
    case CommandIDs::insertAudioConnection:
        objController.addNewAudioConnection(this);
        break;
    case CommandIDs::insertWaveguide:
        objController.addNewObject(this, ObjectFactory::createNewObjectTree(Ids::waveguide, mp.x, mp.y));
        break;
    case CommandIDs::insertTermination:
        objController.addNewObject(this, ObjectFactory::createNewObjectTree(Ids::termination, mp.x, mp.y));
        break;
    default:
        return false;
    }
    objController.getUndoManager()->beginNewTransaction();
    return true;
}

void ObjectsHolder::showContextMenu(const Point<int> mPos)
{
    PopupMenu m;
    m.addSectionHeader("Insert...");
    m.addItem(1, "mass");
    m.addItem(2, "ground");
    m.addItem(3, "resonator");
    m.addItem(4, "port");
    m.addSeparator();
    m.addItem(5, "audioout");
    m.addSeparator();
    m.addItem(6, "waveguide");
    m.addItem(7, "termination");

    const int r = m.show();

    if (r == 1)
    {
        objController.addNewObject(this, ObjectFactory::createNewObjectTree(Ids::mass, mPos.x, mPos.y));
    }
    else if (r == 2)
    {
        objController.addNewObject(this, ObjectFactory::createNewObjectTree(Ids::ground, mPos.x, mPos.y));
    }
    else if (r == 3)
    {
        objController.addNewObject(this, ObjectFactory::createNewObjectTree(Ids::resonator, mPos.x, mPos.y));
    }
    else if (r == 4)
    {
        objController.addNewObject(this, ObjectFactory::createNewObjectTree(Ids::port, mPos.x, mPos.y));
    }
    else if (r == 5)
    {
        objController.addNewObject(this, ObjectFactory::createNewObjectTree(Ids::audioout, mPos.x, mPos.y));
    }
    else if (r == 6)
    {
        objController.addNewObject(this, ObjectFactory::createNewObjectTree(Ids::waveguide, mPos.x, mPos.y));
    }
    else if (r == 7)
    {
        objController.addNewObject(this, ObjectFactory::createNewObjectTree(Ids::termination, mPos.x, mPos.y));
    }
}

void ObjectsHolder::showLinkPopupMenu(String so, String eo)
{
	PopupMenu m;
	m.addItem (1, "Add link");
	m.addItem (2, "Add touch");
	m.addItem (3, "Add pluck");
    m.addSeparator();
    m.addItem (4, "Add audio connection");
	const int r = m.show();

	if (r == 1)
	{
		DBG("Add link");
        objController.addNewLinkIfPossible(this, ObjectFactory::createNewLinkObjectTree(Ids::link, so, eo));
		return;
	}
	else if (r == 2)
	{
		DBG("Add touch");
        objController.addNewLinkIfPossible(this, ObjectFactory::createNewLinkObjectTree(Ids::touch, so, eo));
	}
	else if (r == 3)
	{
		DBG("Add pluck");
        objController.addNewLinkIfPossible(this, ObjectFactory::createNewLinkObjectTree(Ids::pluck, so, eo));
	}
    else if (r == 4)
    {
        DBG("Add audio connection");
        objController.addNewAudioConnection(this);
    }
}
void ObjectsHolder::editObjectProperties(BaseObjectComponent* oc)
{
    objController.editObjectProperties(oc, &mdlFile->getUndoMgr());
}

void ObjectsHolder::findLassoItemsInArea (Array <ObjectComponent*>& results, const Rectangle<int>& lasso)
{
    for (int i = 0; i < getNumChildComponents(); ++i)
    {
        ObjectComponent* const e = dynamic_cast <ObjectComponent*> (getChildComponent (i));

        if (e != 0 && e->getBounds().intersects (lasso))
        {
            e->setSelected(true);
            results.add (e);
        }
    }
}

SelectedItemSet <ObjectComponent*>& ObjectsHolder::getLassoSelection()
{
    return objController.getSelectedObjects();
}


//const Rectangle<int> ObjectsHolder::getComponentArea() const
//{
////    if (document.isFixedSize())
////    {
////        return Rectangle<int> ((getWidth() - document.getInitialWidth()) / 2,
////                               (getHeight() - document.getInitialHeight()) / 2,
////                               document.getInitialWidth(),
////                               document.getInitialHeight());
////    }
////    else
////    {
////        return Rectangle<int> (editorEdgeGap, editorEdgeGap,
////                               getWidth() - editorEdgeGap * 2,
////                               getHeight() - editorEdgeGap * 2);
////    }
//}

void ObjectsHolder::deleteSelectedObjects()
{
    objController.removeSelectedObjects(this);
    objController.removeSelectedLinks(this);
    objController.removeSelectedAudioConnections(this);
}