/*
  ==============================================================================

    ObjController.cpp
    Created: 13 Apr 2012 12:06:25am
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
#include "../View/SelectableObject.h"
#include "../Models/ObjectActions.h"
#include "../Models/MDLFile.h"
#include "../Models/ObjectFactory.h"
#include "../View/ObjectComponent.h"
#include "../View/LinkComponent.h"
#include "../View/AudioOutConnector.h"
#include "MDLController.h"
#include "../Utilities/IdManager.h"
#include "../View/CommentComponent.h"
//#include "../Graph/FlowAlgorithm.h"
//#include "../Graph/ForceDirectedFlowAlgorithm.h"
#include "../Graph/DirectedGraph.h"
#include "../Utilities/ObjectsHelper.h"

#include "ObjController.h"

using namespace synthamodeler;

ObjController::ObjController(MDLController& owner_)
: owner(owner_),
  timesPasted(0)
{
    idMgr = new IdManager();
    sObjects.addChangeListener(propertiesWindow);
}

ObjController::~ObjController()
{
    audioConnections.clear(true);
    links.clear(true);
    objects.clear(true);
    comments.clear(true);
    idMgr = nullptr;
    sObjects.removeChangeListener(propertiesWindow);
    masterReference.clear();
}

bool ObjController::perform(UndoableAction * const action, const String& actionName)
{
    return owner.perform(action, actionName);
}

ObjectComponent* ObjController::addObject(ObjectsHolder* holder, ValueTree objValues, int index, bool undoable)
{
    if(undoable)
    {
        AddObjectAction* action = new AddObjectAction(this, objValues, holder);
        owner.getUndoManager().perform(action, "Add new Object");

        return objects[action->indexAdded];
    }
    else
    {
        const Identifier& groupName = ObjectsHelper::getObjectGroup(objValues.getType().toString());
        ValueTree mdl = owner.getMDLTree();
        ValueTree subTree = mdl.getOrCreateChildWithName(groupName, nullptr);

		subTree.addChild(objValues,-1, nullptr);
        idMgr->addId(objValues.getType(), objValues[Ids::identifier].toString(), nullptr);

        ObjectComponent* objComp = new ObjectComponent(*this, objValues);
        objects.insert(index, objComp);

        holder->addAndMakeVisible(objComp);
        changed();
        return objComp;
    }
    return 0;
}

void ObjController::addNewObject(ObjectsHolder* holder, ValueTree objValues)
{

    ObjectComponent* oc = addObject(holder, objValues, -1, true);
    sObjects.selectOnly(oc);

}

LinkComponent* ObjController::addLink(ObjectsHolder* holder, ValueTree linkValues, int index, bool undoable)
{
    if(undoable)
    {
        AddLinkAction* action = new AddLinkAction(this, linkValues, holder);
        owner.getUndoManager().perform(action, "Add new Link");

        return links[action->indexAdded];
    }
    else
    {
        const Identifier& gruopName = ObjectsHelper::getObjectGroup(linkValues.getType());
        ValueTree mdl = owner.getMDLTree();
        ValueTree subTree = mdl.getOrCreateChildWithName(gruopName, nullptr);
		subTree.addChild(linkValues,-1, nullptr);
        idMgr->addId(linkValues.getType(), linkValues[Ids::identifier].toString(), nullptr);

        LinkComponent* linkComp = new LinkComponent(*this, linkValues);
        links.insert(index, linkComp);

		holder->addAndMakeVisible(linkComp);
        linkComp->toBack();
        changed();
        return linkComp;
    }
    return 0;
}

void ObjController::addNewLink(ObjectsHolder* holder, ValueTree linkValues)
{
    addLink(holder, linkValues, -1, true);
}
bool ObjController::checkIfLinkExitsts(ValueTree linkTree)
{
    for (int i = 0; i < links.size(); i++)
    {
        if(links.getUnchecked(i)->sameStartEnd(linkTree))
        {
            return true;
        }
    }
    return false;

}

bool ObjController::checkIfAudioConnectionExitsts(ValueTree source,
                                                  ValueTree audioOut)
{
    for (int i = 0; i < audioConnections.size(); i++)
    {
        ValueTree sourceComp = audioConnections.getUnchecked(i)->getSourceObject()->getData();
        ValueTree aoComp = audioConnections.getUnchecked(i)->getAudioObject()->getData();
        if(sourceComp[Ids::identifier] == source[Ids::identifier]
            && aoComp[Ids::identifier] == audioOut[Ids::identifier])
        {
            return true;
        }
    }
    return false;
}
void ObjController::addNewLinkIfPossible(ObjectsHolder* holder, ValueTree linkValues)
{
    if(sObjects.getNumSelected() == 2)
    {
        ObjectComponent* oc1 = dynamic_cast<ObjectComponent*>(sObjects.getSelectedItem(0));
        ObjectComponent* oc2 = dynamic_cast<ObjectComponent*>(sObjects.getSelectedItem(1));

        if(oc1 != nullptr && oc2 != nullptr
            && oc1->canBeConnected(linkValues.getType())
            && oc2->canBeConnected(linkValues.getType())
            && (! checkIfLinkExitsts(linkValues)))
        {
            addLink(holder, linkValues, -1, true);
        }
        else
        {
            String msg = "Cannot connect ";
            msg << oc1->getData().getType().toString();
            msg << " and ";
            msg << oc2->getData().getType().toString();
            msg << " with ";
            msg << linkValues.getType().toString();
            msg << ".";
            SAM_CONSOLE(msg, PostLevel::ERROR);
        }
    }
    else
    {
        SAM_CONSOLE("Please select 2 Objects", PostLevel::ERROR);
    }
}

AudioOutConnector* ObjController::addAudioConnection(ObjectsHolder* holder,
                                                     BaseObjectComponent* objComp,
                                                     ObjectComponent* audioOutComp,
                                                     ValueTree source,
                                                     int index,
                                                     bool undoable)
{
    if(undoable)
    {
        AddAudioConnectionAction* action = new AddAudioConnectionAction(this,
                                                                        objComp,
                                                                        source,
                                                                        audioOutComp,
                                                                        holder);
        owner.getUndoManager().perform(action, "Add new audio connection");

        return audioConnections[action->indexAdded];
    }
    else
    {
        AudioOutConnector* aoc = new AudioOutConnector(*this, objComp, audioOutComp);
        ValueTree sources = aoc->getAudioObject()->getData().getChildWithName(Ids::sources);
        if(! sources.getChildWithProperty(Ids::value, source[Ids::value]).isValid())
            sources.addChild(source, -1, nullptr);
        audioConnections.insert(index, aoc);
        holder->addAndMakeVisible(aoc);
        return aoc;
    }
    return nullptr;
}

static ValueTree createAudioSourceTree(const String& srcName, const String& srcVal)
{
    ValueTree src(Ids::audiosource);
    String aSrc;
    aSrc << srcName;
    aSrc << srcVal;
    src.setProperty(Ids::value, aSrc, nullptr);
    return src;
}
void ObjController::addNewAudioConnection(ObjectsHolder* holder)
{
    if(sObjects.getNumSelected() == 2)
    {
        if(! StoredSettings::getInstance()->getShowAudioConnections())
        {
            StoredSettings::getInstance()->setShowAudioConnections(true);
            setAudioConnectionVisibility(true);
        }

        ObjectComponent* oc1 = dynamic_cast<ObjectComponent*>(sObjects.getSelectedItem(0));
        LinkComponent* lc1 = dynamic_cast<LinkComponent*>(sObjects.getSelectedItem(0));
        ObjectComponent* oc2 = dynamic_cast<ObjectComponent*>(sObjects.getSelectedItem(1));
        LinkComponent* lc2 = dynamic_cast<LinkComponent*>(sObjects.getSelectedItem(1));

        if(oc1 != nullptr && oc2 != nullptr)
        {
            if (oc1->getData().getType() == Ids::audioout
                && oc2->getData().getType() != Ids::audioout)
            {
                if(! checkIfAudioConnectionExitsts(oc2->getData(), oc1->getData()))
                {
                    ValueTree src = createAudioSourceTree(oc2->getData()[Ids::identifier].toString(),
                                                          "*1.0");
//                    ValueTree sources = oc1->getData().getOrCreateChildWithName(Ids::sources)
                    addAudioConnection(holder, oc2, oc1, src, -1, true);
                }
            }
            else if (oc1->getData().getType() != Ids::audioout
                && oc2->getData().getType() == Ids::audioout)
            {
                if(! checkIfAudioConnectionExitsts(oc1->getData(), oc2->getData()))
                {
                    ValueTree src = createAudioSourceTree(oc1->getData()[Ids::identifier].toString(),
                                                          "*1.0");
                    addAudioConnection(holder, oc1, oc2, src, -1, true);
                }
            }
            else
            {
                SAM_CONSOLE("Cannot create audio connection", PostLevel::ERROR);
            }
        }
        else if(oc1 == nullptr && oc2 != nullptr && lc1 != nullptr
            && lc1->getData().getType() != Ids::waveguide)
        {
            if( ! checkIfAudioConnectionExitsts(lc1->getData(), oc2->getData()))
            {
                ValueTree src = createAudioSourceTree(lc1->getData()[Ids::identifier].toString(),
                                                          "*1.0");
                addAudioConnection(holder, lc1, oc2, src, -1, true);
            }
        }
        else if(oc2 == nullptr && oc1 != nullptr && lc2 != nullptr
            && lc2->getData().getType() != Ids::waveguide)
        {
            if( ! checkIfAudioConnectionExitsts(lc2->getData(), oc1->getData()))
            {
                ValueTree src = createAudioSourceTree(lc2->getData()[Ids::identifier].toString(),
                                                      "*1.0");
                addAudioConnection(holder, lc2, oc1, src, -1, true);
            }
        }
        else
        {
            SAM_CONSOLE("Cannot create audio connection", PostLevel::ERROR);
        }
    }
}

CommentComponent* ObjController::addComment(ObjectsHolder* holder,
                                            ValueTree commentValues,
                                            int index, bool undoable)
{
    if(undoable)
    {
        AddCommentAction* action = new AddCommentAction(this, commentValues, holder);
        owner.getUndoManager().perform(action, "Add new Comment");

        return comments[action->indexAdded];
    }
    else
    {
        const Identifier& groupName = ObjectsHelper::getObjectGroup(commentValues.getType().toString());
        ValueTree mdl = owner.getMDLTree();
        ValueTree subTree = mdl.getOrCreateChildWithName(groupName, nullptr);

        subTree.addChild(commentValues,-1, nullptr);
        idMgr->addId(commentValues.getType(),
                     commentValues[Ids::identifier].toString(),
                     nullptr);

        CommentComponent* commentComp = new CommentComponent(*this, commentValues);
        comments.insert(index, commentComp);

        holder->addAndMakeVisible(commentComp);
        changed();
        return commentComp;
    }
}

void ObjController::addNewComment(ObjectsHolder* holder, ValueTree commentValues)
{
    CommentComponent* cc = addComment(holder, commentValues, -1, true);
    sObjects.selectOnly(cc);
}


void ObjController::removeObject(ObjectComponent* objComp, bool undoable, ObjectsHolder* holder)
{
    if (undoable)
    {
        owner.getUndoManager().perform(new RemoveObjectAction(holder, objComp, this));
    }
    else
    {
        sObjects.deselect(objComp);
        sObjects.changed(true);

        // Get link indices attached to this object and remove them.
        // TODO needs better solution
        Array<int> lIndices = checkIfObjectHasLinks(objComp->getData());
//        if(indices.size() > 0)
//            sObjects.deselectAll();
        for(int i = lIndices.size(); --i >= 0;)
        {
            removeLink(getLink(lIndices[i]), true, holder);
        }
        Array<int> aoIndices = checkIfObjectHasAudioConnections(objComp->getData());
        for(int i = aoIndices.size(); --i >= 0;)
        {
            // remove audioouts attached to this object only if this is not an audioout
            bool isAudioOut = objComp->getData().getType() == Ids::audioout;
            removeAudioConnection(getAudioConnector(aoIndices[i]), !isAudioOut, holder);
        }

        const Identifier& groupName = ObjectsHelper::getObjectGroup(objComp->getData().getType());
        ValueTree mdl = owner.getMDLTree();
        ValueTree subTree = mdl.getOrCreateChildWithName(groupName, nullptr);
        idMgr->removeId(objComp->getData().getType(),
                        objComp->getData()[Ids::identifier].toString(), nullptr);
        subTree.removeChild(objComp->getData(), nullptr);
        objects.removeObject(objComp);
    }

}

void ObjController::removeSelectedObjects(ObjectsHolder* holder)
{
    SelectedItemSet <SelectableObject*> temp;
    temp = sObjects;

    if (temp.getNumSelected() > 0)
    {
        sObjects.deselectAll();
        sObjects.changed(true);
        // first remove all selected links
        for (int i = temp.getNumSelected(); --i >= 0;)
        {
            if(LinkComponent* lc = dynamic_cast<LinkComponent*>(temp.getSelectedItem(i)))
            {
                temp.deselect(lc);
                removeLink(lc, true, holder);
            }
        }
        // then objects and remaining links connected to the objects
        for (int i = temp.getNumSelected(); --i >= 0;)
        {
            if(AudioOutConnector* aoc = dynamic_cast<AudioOutConnector*>(temp.getSelectedItem(i)))
            {
                temp.deselect(aoc);
                removeAudioConnection(aoc, true, holder);
                continue;
            }
        }
        for (int i = temp.getNumSelected(); --i >= 0;)
        {
            if(ObjectComponent* oc = dynamic_cast<ObjectComponent*>(temp.getSelectedItem(i)))
            {
                removeObject(oc, true, holder);
            }
            else if(CommentComponent* cc = dynamic_cast<CommentComponent*>(temp.getSelectedItem(i)))
            {
                removeComment(cc, true, holder);
            }
//            LinkComponent* lc = dynamic_cast<LinkComponent*>(temp.getSelectedItem(i));
//            if(lc != nullptr)
//            {
//                removeLink(lc, true, holder);
//                continue;
//            }

        }
    }
}

void ObjController::removeAudioConnection(AudioOutConnector* aocComp,
                                          bool undoable,
                                          ObjectsHolder* holder)
{
    if(undoable)
    {
        owner.getUndoManager().perform(new RemoveAudioConnectionAction(holder, aocComp, this));
    }
    else
    {
        sObjects.deselect(aocComp);
        sObjects.changed(true);
        ValueTree sources = aocComp->getAudioObject()->getData().getChildWithName(Ids::sources);
        ValueTree source;
        for (int i = 0; i < sources.getNumChildren(); ++i)
        {
            String val = sources.getChild(i)[Ids::value];
//            DBG(aocComp->getSourceObject()->getData()[Ids::identifier].toString());
            if(val.contains(aocComp->getSourceObject()->getData()[Ids::identifier].toString()))
            {
                source = sources.getChild(i);
                break;
            }
        }
        sources.removeChild(source, nullptr);
        audioConnections.removeObject(aocComp);
    }
}

void ObjController::removeLink(LinkComponent* linkComp, bool undoable, ObjectsHolder* holder)
{
    if (undoable)
    {
        owner.getUndoManager().perform(new RemoveLinkAction(holder, linkComp, this));
    }
    else
    {
        sObjects.deselect(linkComp);
        sObjects.changed(true);

        Array<int> aoIndices = checkIfObjectHasAudioConnections(linkComp->getData());
        for(int i = aoIndices.size(); --i >= 0;)
        {
            removeAudioConnection(getAudioConnector(aoIndices[i]), true, holder);
        }

        const Identifier& groupName = ObjectsHelper::getObjectGroup(linkComp->getData().getType());
        ValueTree mdl = owner.getMDLTree();
        ValueTree subTree = mdl.getOrCreateChildWithName(groupName, nullptr);
        idMgr->removeId(linkComp->getData().getType(),
                        linkComp->getData()[Ids::identifier].toString(), nullptr);
        subTree.removeChild(linkComp->getData(), nullptr);
        links.removeObject(linkComp);
    }
}

void ObjController::removeComment(CommentComponent* commentComp,
                                  bool undoable, ObjectsHolder* holder)
{
    if(undoable)
    {
        owner.getUndoManager().perform(new RemoveCommentAction(holder, commentComp, this));
    }
    else
    {
        sObjects.deselect(commentComp);
        sObjects.changed(true);

        const Identifier& groupName = ObjectsHelper::getObjectGroup(commentComp->getData().getType());
        ValueTree mdl = owner.getMDLTree();
        ValueTree subTree = mdl.getOrCreateChildWithName(groupName, nullptr);
        idMgr->removeId(commentComp->getData().getType(),
                        commentComp->getData()[Ids::identifier].toString(), nullptr);
        subTree.removeChild(commentComp->getData(), nullptr);
        comments.removeObject(commentComp);
    }
}

void ObjController::loadComponents(ObjectsHolder* holder)
{
    MDLFile* mf = owner.getMDLFile();
    ValueTree mdl = mf->getMDLRoot();
    int numObjects = 0;
    int numNodesZeroPos = 0;

    ValueTree massObjects = mdl.getChildWithName(Objects::masses);
    for (int i = 0; i < massObjects.getNumChildren(); i++)
    {
        ValueTree obj = massObjects.getChild(i);
        if(idMgr->addId(obj.getType(), obj[Ids::identifier].toString(), nullptr))
        {
            ObjectComponent* objComp = new ObjectComponent(*this, obj);
            objects.add(objComp);
            holder->addAndMakeVisible(objComp);
            SAM_LOG("Load " + obj.getType().toString() + " " + obj[Ids::identifier].toString());
            ++numObjects;
            if(float(obj[Ids::posX]) < 0.00001f
                && float(obj[Ids::posY]) < 0.00001f)
                ++numNodesZeroPos;
        }
        else
        {
            SAM_LOG("Couldn't add duplicate Object " + obj[Ids::identifier].toString());
        }
    }
    ValueTree termObjects = mdl.getChildWithName(Objects::terminations);
    for (int i = 0; i < termObjects.getNumChildren(); i++)
    {
        ValueTree obj = termObjects.getChild(i);
        if(idMgr->addId(obj.getType(), obj[Ids::identifier].toString(), nullptr))
        {
            ObjectComponent* objComp = new ObjectComponent(*this, obj);
            objects.add(objComp);
            holder->addAndMakeVisible(objComp);
            SAM_LOG("Load " + obj.getType().toString() + " " + obj[Ids::identifier].toString());
            ++numObjects;
            if(float(obj[Ids::posX]) < 0.00001f
                && float(obj[Ids::posY]) < 0.00001f)
                ++numNodesZeroPos;
        }
        else
        {
            SAM_LOG("Couldn't add duplicate Object " + obj[Ids::identifier].toString());
        }
    }
    ValueTree junctObjects = mdl.getChildWithName(Objects::junctions);
    for (int i = 0; i < junctObjects.getNumChildren(); i++)
    {
        ValueTree obj = junctObjects.getChild(i);
        if(idMgr->addId(obj.getType(), obj[Ids::identifier].toString(), nullptr))
        {
            ObjectComponent* objComp = new ObjectComponent(*this, obj);
            objects.add(objComp);
            holder->addAndMakeVisible(objComp);
            SAM_LOG("Load " + obj.getType().toString() + " " + obj[Ids::identifier].toString());
            ++numObjects;
            if(float(obj[Ids::posX]) < 0.00001f
                && float(obj[Ids::posY]) < 0.00001f)
                ++numNodesZeroPos;
        }
        else
        {
            SAM_LOG("Couldn't add duplicate Object " + obj[Ids::identifier].toString());
        }
    }
    ValueTree linkObjects = mdl.getChildWithName(Objects::links);
    for (int i = 0; i < linkObjects.getNumChildren(); i++)
    {
        ValueTree obj = linkObjects.getChild(i);
        if(idMgr->addId(obj.getType(), obj[Ids::identifier].toString(), nullptr))
        {
            LinkComponent* linkComp = new LinkComponent(*this, obj);
            links.add(linkComp);
            holder->addAndMakeVisible(linkComp);
            linkComp->toBack();
            SAM_LOG("Load " + obj.getType().toString() + " " + obj[Ids::identifier].toString());
        }
    }
    ValueTree waveguideObjects = mdl.getChildWithName(Objects::waveguides);
    for (int i = 0; i < waveguideObjects.getNumChildren(); i++)
    {
        ValueTree obj = waveguideObjects.getChild(i);
        if(idMgr->addId(obj.getType(), obj[Ids::identifier].toString(), nullptr))
        {
            LinkComponent* linkComp = new LinkComponent(*this, obj);
            links.add(linkComp);
            holder->addAndMakeVisible(linkComp);
            linkComp->toBack();
            SAM_LOG("Load " + obj.getType().toString() + " " + obj[Ids::identifier].toString());
        }
    }

    ValueTree audioObjects = mdl.getChildWithName(Objects::audioobjects);
    for (int i = 0; i < audioObjects.getNumChildren(); i++)
    {
        ValueTree obj = audioObjects.getChild(i);
        if(idMgr->addId(obj.getType(), obj[Ids::identifier].toString(), nullptr))
        {
            ObjectComponent* audioOutComp = new ObjectComponent(*this, obj);
            objects.add(audioOutComp);
            holder->addAndMakeVisible(audioOutComp);
            SAM_LOG("Load " + obj.getType().toString() + " " + obj[Ids::identifier].toString());
            ++numObjects;
            if(float(obj[Ids::posX]) < 0.00001f
                && float(obj[Ids::posY]) < 0.00001f)
                ++numNodesZeroPos;

            ValueTree aoSources = obj.getChildWithName(Ids::sources);
            for (int j = 0; j < aoSources.getNumChildren(); ++j)
            {
                ValueTree source = aoSources.getChild(j);
//                ObjectComponent* oc = getObjectForId(src);
//                LinkComponent* lc = getLinkForId(src);
                BaseObjectComponent* sourceComp = ObjectsHelper::getBaseObjectFromSource(this, source);
//                if(oc != nullptr)
//                    sourceComp = oc;
//                else if(lc != nullptr)
//                    sourceComp = lc;

                if( sourceComp != nullptr )
                {
                    AudioOutConnector* aoc = new AudioOutConnector(*this,
                                                                   sourceComp,
                                                                   audioOutComp);
                    audioConnections.add(aoc);
                    holder->addAndMakeVisible(aoc);
                    aoc->update();
                }
            }
        }
        else
        {
            SAM_LOG("Couldn't add duplicate Object " + obj[Ids::identifier].toString());
        }
    }

    ValueTree faustcodeblock = mdl.getChildWithName(Objects::faustcodeblock);
    for (int i = 0; i < faustcodeblock.getNumChildren(); i++)
    {
        ValueTree obj = faustcodeblock.getChild(i);
//        if(idMgr->addId(obj.getType(), obj[Ids::identifier].toString(), nullptr))
//        {
//            SAM_LOG("Load " + obj.getType().toString() + " " + obj[Ids::identifier].toString());
//        }
//        else
//        {
//            faustcodeblock.removeChild(obj, nullptr);
//        }
    }

    ValueTree commentsTree = mdl.getChildWithName(Objects::comments);
    for (int i = 0; i < commentsTree.getNumChildren(); ++i)
    {
        ValueTree comment = commentsTree.getChild(i);
        if(idMgr->addId(comment.getType(), comment[Ids::identifier].toString(), nullptr))
        {
            CommentComponent* cComp = new CommentComponent(*this, comment);
            comments.add(cComp);
            holder->addAndMakeVisible(cComp);
            cComp->update();
            SAM_LOG("Load " + comment.getType().toString() + " " + comment[Ids::identifier].toString());
            ++numObjects;
            if(float(comment[Ids::posX]) < 0.00001f
                && float(comment[Ids::posY]) < 0.00001f)
                ++numNodesZeroPos;
        }
    }

    setAudioConnectionVisibility(StoredSettings::getInstance()->getShowAudioConnections());

    if(StoredSettings::getInstance()->getShouldRedrawOnLoad())
        if(numNodesZeroPos >= numObjects || numNodesZeroPos > 1)
            holder->redrawObjects(CommandIDs::redrawForceDirected);
}

void ObjController::selectAll(bool shouldBeSelected)
{
    if(shouldBeSelected)
    {
        sObjects.deselectAll();
        for (int i = 0; i < objects.size(); ++i)
        {
            sObjects.addToSelection(objects.getUnchecked(i));
        }
        for (int j = 0; j < links.size(); ++j)
        {
            sObjects.addToSelection(links.getUnchecked(j));
        }
        for (int k = 0; k < audioConnections.size(); ++k)
        {
            sObjects.addToSelection(audioConnections.getUnchecked(k));
        }
        for (int l = 0; l < comments.size(); ++l)
        {
            sObjects.addToSelection(comments.getUnchecked(l));
        }
    }
    else
    {
        sObjects.deselectAll();
    }
}

void ObjController::selectObjectsIfContainText(const String& selectionText)
{
    sObjects.deselectAll();
    if(selectionText.isEmpty())
        return;

    for (int i = 0; i < objects.size(); ++i)
    {
        ObjectComponent* oc = objects.getUnchecked(i);
        if(ObjectsHelper::containsStringInValueTree(oc->getData(), selectionText, true))
            sObjects.addToSelection(oc);
    }
    for (int j = 0; j < links.size(); ++j)
    {
        LinkComponent* lc = links.getUnchecked(j);
        if(ObjectsHelper::containsStringInValueTree(lc->getData(), selectionText, true))
            sObjects.addToSelection(lc);
    }
}

void ObjController::startDragging()
{
    for (int i = 0; i < objects.size(); ++i)
    {
        ObjectComponent * const c = objects.getUnchecked(i);

        const Point<int>& r = c->getCenter();

        c->getProperties().set("xDragStart", r.getX());
        c->getProperties().set("yDragStart", r.getY());
    }
    for (int i = 0; i < comments.size(); ++i)
    {
        CommentComponent * const c = comments.getUnchecked(i);

        const Point<int>& r = c->getCenter();

        c->getProperties().set("xDragStart", r.getX());
        c->getProperties().set("yDragStart", r.getY());
    }

    owner.getUndoManager().beginNewTransaction();
}

void ObjController::dragSelectedComps(int dx, int dy)
{
    for (int i = 0; i < sObjects.getNumSelected(); ++i)
    {
        if(ObjectComponent * const c = dynamic_cast<ObjectComponent*>(sObjects.getSelectedItem(i)))
        {
           const int startX = c->getProperties() ["xDragStart"];
           const int startY = c->getProperties() ["yDragStart"];

            Point<int> r = c->getCenter();

            r.setXY(owner.getHolderComponent()->snapPosition(startX + dx),
                    owner.getHolderComponent()->snapPosition(startY + dy));

            c->setActualPosition(r);
        }
        else if(CommentComponent* const cc = dynamic_cast<CommentComponent*>(sObjects.getSelectedItem(i)))
        {
            const int startX = cc->getProperties() ["xDragStart"];
            const int startY = cc->getProperties() ["yDragStart"];

            Point<int> r(cc->getCenter());

            r.setXY(owner.getHolderComponent()->snapPosition(startX + dx),
                    owner.getHolderComponent()->snapPosition(startY + dy));

            cc->setActualPosition(r);
        }
    }
}

void ObjController::endDragging()
{
    for (int i = 0; i < sObjects.getNumSelected(); ++i)
    {
        if(ObjectComponent * const c = dynamic_cast<ObjectComponent*>(sObjects.getSelectedItem(i)))
        {
            c->setPosition(c->getActualPos(), true);
        }
        else if(CommentComponent* const cc = dynamic_cast<CommentComponent*>(sObjects.getSelectedItem(i)))
        {
            cc->setPosition(cc->getPosition(), true);
        }
    }

    changed();

    owner.getUndoManager().beginNewTransaction();
}

void ObjController::moveSelectedComps (int dxFromMoveStart, int dyFromMoveStart)
{
    startDragging();
    dragSelectedComps(dxFromMoveStart, dyFromMoveStart);
    endDragging();
}

UndoManager& ObjController::getUndoManager()
{
    return owner.getUndoManager();
}

void ObjController::changed()
{
    owner.changed();
}

ObjectComponent* ObjController::getObjectForId(const String& idString) const throw()
{
    for (int i = 0; i < objects.size(); i++)
    {
        ObjectComponent* elem = objects.getUnchecked(i);
        if(idString.compare(elem->getData().getProperty(Ids::identifier).toString()) == 0)
        {
            return elem;
        }
    }
    return nullptr;

}

LinkComponent* ObjController::getLinkForId(const String& idString) const throw()
{
    for (int i = 0; i < links.size(); i++)
    {
        LinkComponent* elem = links.getUnchecked(i);
        if(idString.compare(elem->getData().getProperty(Ids::identifier).toString()) == 0)
        {
            return elem;
        }
    }
    return nullptr;

}

CommentComponent* ObjController::getCommentForId(const String& idString) const throw()
{
    for (int i = 0; i < comments.size(); i++)
    {
        CommentComponent* elem = comments.getUnchecked(i);
        if(idString.compare(elem->getData().getProperty(Ids::identifier).toString()) == 0)
        {
            return elem;
        }
    }
    return nullptr;

}

void ObjController::reverseLinkDirection()
{
    owner.getUndoManager().beginNewTransaction();

    for (int i = 0; i < sObjects.getNumSelected(); ++i)
    {
        if(LinkComponent* lc = dynamic_cast<LinkComponent*>(sObjects.getSelectedItem(i)))
        {
            ReverseLinkDirectionAction* action = new ReverseLinkDirectionAction(lc,this);
            owner.getUndoManager().perform(action, "reverse link direction");
        }
    }
    changed();

    owner.getUndoManager().beginNewTransaction();
}

Array<int> ObjController::checkIfObjectHasLinks(ValueTree objTree)
{
    Array<int> linkIndices;
//    ValueTree objTree = objComp->getData();
    for (int i = 0; i < links.size(); i++)
    {
        ValueTree linkTree = links.getUnchecked(i)->getData();

        if(linkTree.getProperty(Ids::startVertex) == objTree.getProperty(Ids::identifier)
            || linkTree.getProperty(Ids::endVertex) == objTree.getProperty(Ids::identifier))
        {
            linkIndices.add(i);
        }
    }
    return linkIndices;
}

Array<int> ObjController::checkIfObjectHasAudioConnections(ValueTree objTree)
{
    Array<int> aoIndices;
    for (int i = 0; i < audioConnections.size(); ++i)
    {
        AudioOutConnector* aoc = audioConnections.getUnchecked(i);

        ValueTree aoDataSource = aoc->getSourceObject()->getData();
        ValueTree aoDataAudioObject = aoc->getAudioObject()->getData();
        if(aoDataSource[Ids::identifier] == objTree[Ids::identifier]
            || aoDataAudioObject[Ids::identifier] == objTree[Ids::identifier])
            aoIndices.add(i);
    }
    return aoIndices;
}

//==============================================================================

bool ObjController::checkIfIdExists(const Identifier& objId, const String& idStr)
{
    return idMgr->contains(objId, idStr);
}

bool ObjController::renameId(const Identifier& objId, const String& oldId,
                             const String& newId, UndoManager* undoManager_)
{
    return idMgr->renameId(objId, oldId, newId, undoManager_);
}

String ObjController::getNewNameForObject(const Identifier& objId)
{
    return idMgr->getNextId(objId);
}

void ObjController::setLinksSegmented(bool isSegmented)
{
    for (int i = 0; i < links.size(); ++i)
    {
        LinkComponent* const lc = links.getUnchecked(i);
        lc->setSegmented(isSegmented);
    }
    for (int i = 0; i < audioConnections.size(); ++i)
    {
        AudioOutConnector* const aoc = audioConnections.getUnchecked(i);
        aoc->setSegmented(isSegmented);
    }
}

void ObjController::destroy()
{
    links.clear();
    audioConnections.clear();
    objects.clear();
    sObjects.deselectAll();
    idMgr = nullptr;
    idMgr = new IdManager();

}

void ObjController::setAudioConnectionVisibility(bool shouldBeVisible)
{
    for (int i = 0; i < audioConnections.size(); ++i)
    {
        audioConnections[i]->setVisible(shouldBeVisible);
        audioConnections[i]->repaint();
    }
}
