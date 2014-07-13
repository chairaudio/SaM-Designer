/*
  ==============================================================================

    ObjectsHelper.h
    Created: 3 Feb 2014 12:37:51am
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

#ifndef OBJECTSHELPER_H_INCLUDED
#define OBJECTSHELPER_H_INCLUDED


namespace synthamodeler
{

class ObjController;
class DirectedGraph;
class ObjectComponent;
class LinkComponent;
class CommentComponent;
class MDLFile;

class ObjectsHelper
{
public:

    static bool changeObjectNameInLink(const ObjController& objController,
                                       const String& oldName,
                                       const String& newName,
                                       UndoManager* undoManager);

    static void changeObjectNameInAudioSources(const ObjController& objController,
                                               const String& oldName,
                                               const String& newName,
                                               UndoManager* undoManager);

    static void tidyUpObjects(ObjController& objController);

    static void makeGraph(const ObjController& objController, DirectedGraph& graph);

    static String fixParameterValueIfNeeded(const String& paramVal);

    static String getGainForSourceId(const ValueTree& sources, const String& sourceId);
    static void setGainForSourceId(const ValueTree& sources, const String& sourceId,
                                   const String& gainVal, UndoManager* undoManager);

    static BaseObjectComponent* getBaseObjectFromSource(const ObjController& objController,
                                                        const ValueTree& source);

    static const Identifier& getObjectGroup(const Identifier& ident);

    static bool containsStringInValueTree(const ValueTree& valTree,
                                          const String& stringToSearch);

    static const Array<Identifier>& getResonatorParamsIds();
    static const Array<Identifier>& getAllObjectIds();

    template<typename BaseCompType>
    static ObjectComponent* getObject(BaseCompType* baseCompType);

    template<typename BaseCompType>
    static LinkComponent* getLink(BaseCompType* baseCompType);

    template<typename BaseCompType>
    static CommentComponent* getComment(BaseCompType* baseCompType);

    static void getSelectedObjectComponents(ObjController& objController,
                                            Array<ObjectComponent*>& selectedObjs);

    static void loadComponents(ObjController& objController, ObjectsHolder& objHolder,
                               const ValueTree& mdlRoot, int& numObjects, int& numZeroPos);

private:

    template<typename ObjType>
    static ObjType* addComp(ObjController& objController, ObjectsHolder& objHolder,
                            ObjType* const obj);

};

}


#endif  // OBJECTSHELPER_H_INCLUDED
