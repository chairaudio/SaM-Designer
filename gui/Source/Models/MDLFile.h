/*
  ==============================================================================

    MDLFile.h
    Created: 11 Apr 2012 3:18:35pm
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

#ifndef __MDLFILE_H_70428F9D__
#define __MDLFILE_H_70428F9D__

#include "JuceHeader.h"

namespace synthamodeler
{
/**
 * The MDLFile class is the document which conatins an the contents of an mdl
 * file. It mangaes also the handling of the file like open, close, save, saveAs
 * and keeps track of changes. Every MDLFile has an UndoManager and the data
 * ValueTree.
 */
class MDLFile : public FileBasedDocument,
				public ValueTree::Listener
{
public:
	MDLFile();
	MDLFile(const File& file);
	~MDLFile();

	void newMDL();
	void close();

	const String getFilePath() const;
    const String getNameWithStatus();
	const String getName() { return getDocumentTitle(); }

	bool perform (UndoableAction* const action, const String& actionName);

    UndoManager& getUndoMgr() throw ()
    {
        return undoMgr;
    }

    const ValueTree& getMDLRoot() const
    {
        return mdlRoot;
    }
    ValueTree& getMDLRoot()
    {
        return mdlRoot;
    }

    void valueTreePropertyChanged(ValueTree& tree, const Identifier& property);
    void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded);
    void valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved);
    void valueTreeChildOrderChanged(ValueTree& parentTree, int oldIndex, int newIndex);
    void valueTreeParentChanged(ValueTree& tree);

    void mdlChanged();

    static const char* mdlFileExtension;

    bool isEmpty();
    bool isUntiled();

    String toString() const;

    bool checkIfChecksumChanged();

protected:
    String getDocumentTitle();
    Result loadDocument(const File& file);
    Result saveDocument(const File& file);
    File getLastDocumentOpened();
    void setLastDocumentOpened(const File& file);

private:

    void initMDL();
    void destroyMDL();

    ValueTree mdlRoot;
    
    static File lastDocumentOpened;

    UndoManager undoMgr;

    bool isUntitledFile;

    MD5 md5;

    friend class MDLParser;
    friend class MDLWriter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MDLFile);

};
}

#endif  // __MDLFILE_H_70428F9D__
