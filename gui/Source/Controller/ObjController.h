/*
  ==============================================================================

    ObjController.h
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

#ifndef __OBJCONTROLLER_H_A98EC6A3__
#define __OBJCONTROLLER_H_A98EC6A3__

#include "JuceHeader.h"

namespace synthamodeler
{
class MDLController;
class BaseObjectComponent;
class ObjectComponent;
class ObjectsHolder;
class LinkComponent;
class Connector;
class SelectableObject;
class IdManager;
class CommentComponent;
class DirectedGraph;
class GraphReflowController;

typedef OwnedArray<ObjectComponent> tObjects;
typedef OwnedArray<LinkComponent> tLinks;
typedef OwnedArray<Connector> tAudioConnections;
typedef OwnedArray<CommentComponent> tComments;
typedef OwnedArray<Connector> tConnectors;

/**
 * The ObjController controlls all ObjectComponents.
 */
class ObjController
{
public:
	ObjController(MDLController& owner_);
	~ObjController();

	/**
	 * Performs the action with the UndoManger.
	 *
	 * @param action		the action to perform
	 * @param actionName	the action name
	 * @return
	 */
	bool perform (UndoableAction* const action, const String& actionName);

	/**
	 * Adds and object to the patch.
	 *
	 * @param holder 		the holder conponent which consists the objects.
	 * @param objId			object id (i.e. mass or link)
	 * @param posX			initial x position of the object
	 * @param posY			initial y position of the object
	 */
	ObjectComponent* addObject(ObjectsHolder* holder, ValueTree objValues, int index, bool undoable);

    void addNewObject(ObjectsHolder* holder, ValueTree objValues);
    
	/**
	 * Removes selected objects from the patch
	 *
	 * @param holder		the holder component which consists the objects.
	 */
	void removeSelectedObjects(ObjectsHolder& holder);

    void removeObject(ObjectComponent* objComp, bool undoable, ObjectsHolder* holder);

    LinkComponent* addLink(ObjectsHolder* holder, ValueTree linkValues, int index, bool undoable);
    
    void addNewLinkIfPossible(ObjectsHolder* holder, ValueTree linkValues);
    
    void addNewLink(ObjectsHolder* holder, ValueTree linkValues);
    
    Connector* addConnector(ObjectsHolder* holder,
                            BaseObjectComponent* objComp,
                            ObjectComponent* audioOutComp,
                            ValueTree source,
                            int index,
                            bool undoable);
    
    void addNewConnector(ObjectsHolder* holder, const Identifier& targetType);
    
    void removeLink(LinkComponent* linkComp, bool undoable, ObjectsHolder* holder);
    
    void removeAudioConnection(Connector* aocComp,
                               bool undoable,
                               ObjectsHolder* holder);


    CommentComponent* addComment(ObjectsHolder* holder, ValueTree commentValues,
                                 int index, bool undoable);
    void addNewComment(ObjectsHolder* holder, ValueTree commentValues);

    void removeComment(CommentComponent* commentComp, bool undoable,
                       ObjectsHolder* holder);

	/**
	 * Loads the object components of a patch when a mdl file is opened.
	 * @param holder
	 */
	void loadComponents(ObjectsHolder& holder);

	/**
	 * Marks all object as selected or deselects.
	 *
	 * @param shouldBeSelected		true for selected, false for deselected
	 */
	void selectAll(bool shouldBeSelected);

    void selectObjectsIfContainsText(const String& selectionText);
  
//    SelectedItemSet <ObjectComponent*>& getSelectedObjects() throw()
//    { 
//        return selectedObjects; 
//    }
//    SelectedItemSet <LinkComponent*>& getSelectedLinks() throw()
//    {
//        return selectedLinks;
//    }
//    SelectedItemSet <AudioOutConnector*>& getSelectedAudioConnections() throw()
//    {
//        return selectedAudioConnections;
//    }
    SelectedItemSet <SelectableObject*>& getSelectedObjects() throw()
    {
        return sObjects;
    }

    void startDragging();
    void dragSelectedComps (int dxFromDragStart, int dyFromDragStart);
    void endDragging();
    
    void moveSelectedComps (int dxFromMoveStart, int dyFromMoveStart);
    UndoManager& getUndoManager();

    ObjectComponent* getObject(int index) const throw() { return objects[index]; }
    ObjectComponent* getObjectUnchecked(int index) const throw() { return objects.getUnchecked(index); }
    int indexOfObject (ObjectComponent* e) const throw() { return objects.indexOf (e); }
    int getNumObjects() const { return objects.size(); }
    void addObjectComp(ObjectComponent* o) { objects.add(o); }
    tObjects& getObjects() { return objects; }

    LinkComponent* getLink(int index) const throw() { return links[index]; }
    LinkComponent* getLinkUnchecked(int index) const throw() { return links.getUnchecked(index); }
    int indexOfLink (LinkComponent* e) const throw() { return links.indexOf (e); }
    int getNumLinks() const { return links.size(); }
    void addLinkComp(LinkComponent* l) { links.add(l); }
    tLinks& getLinks() { return links; }
    const tLinks& getLinks() const { return links; }

    Connector* getAudioConnector(int index) const throw() 
    { return connections[index]; }
    int indexOfAudioConnector (Connector* e) const throw()
    {
        return connections.indexOf(e);
    }
    int getNumAudioConnections() const
    {
        return connections.size();
    }
    void addAudioConnectionComp(Connector* a) { connections.add(a); }
    tConnectors& getConnectors() { return connections; }
    const tConnectors& getConnectors() const { return connections; }

    CommentComponent* getComment(int index) const throw() { return comments[index]; }
    CommentComponent* getCommentUnchecked(int index) const throw() { return comments.getUnchecked(index); }
    int indexOfComment (CommentComponent* e) const throw() { return comments.indexOf (e); }
    int getNumComment() const { return comments.size(); }
    void addCommentComp(CommentComponent* c) { comments.add(c); }

    void addComp(ObjectComponent* o) { objects.add(o); }
    void addComp(CommentComponent* c) { comments.add(c); }
    void addComp(LinkComponent* l) { links.add(l); }

    void changed();
    
    ObjectComponent* getObjectForId(const String& idString) const;
    LinkComponent* getLinkForId(const String& idString) const;
    CommentComponent* getCommentForId(const String& idString) const;
    
    void reverseLinkDirection();
    
    bool checkIfIdExists(const Identifier& objId, const String& idStr) const;
    bool renameId(const Identifier& objId, const String& oldId,
                  const String& newId, UndoManager* undoManager_);
    
    String getNewNameForObject(const Identifier& objId);

    IdManager& getIdManager() { return *idMgr; }
    const IdManager& getIdManager() const { return *idMgr; }

    void setLinksSegmented(bool isSegmented);

    void destroy();

    void setAudioConnectionVisibility(bool shouldBeVisible);

    void setIsReflowing(bool bReflowing) { isReflowing = bReflowing; }
    bool getIsReflowing() const { return isReflowing; }

    template<typename T>
    void setAsFromtmostLink(T& t);

    void startReflow(ObjectsHolder& objHolder, int cmdId);
    void stopReflow();

    void addSelectedObjectsListener(ChangeListener* listener);

private:
    
    bool checkIfLinkExitsts(const ValueTree& linkTree) const;
    bool checkIfConnectionExitsts(const ValueTree& source,
                                  const ValueTree& target) const;
    Array<int> checkIfObjectHasLinks(const ValueTree& objTree) const;
    Array<int> checkIfObjectHasAudioConnections(const ValueTree& objTree) const;
    MDLController& owner;
	OwnedArray<ObjectComponent> objects;
    OwnedArray<LinkComponent> links;
    OwnedArray<Connector> connections;
    OwnedArray<CommentComponent> comments;
    SelectedItemSet<SelectableObject*> sObjects;
    ScopedPointer<IdManager> idMgr;
    int timesPasted;
    bool isReflowing;

    ScopedPointer<GraphReflowController> reflowController;

    WeakReference<ObjController>::Master masterReference;
    friend class WeakReference<ObjController>;
};
}

#endif  // __OBJCONTROLLER_H_A98EC6A3__
