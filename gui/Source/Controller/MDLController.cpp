/*
  ==============================================================================

    MDLController.cpp
    Created: 12 Apr 2012 11:49:49pm
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
#include "../Models/MDLFile.h"
#include "../Models/SAMCmd.h"

#include "MDLController.h"

using namespace synthamodeler;

MDLController::MDLController(MainAppWindow& mainAppWindow_)
: mainAppWindow(mainAppWindow_),
  currentMdl(nullptr)
{
	samCmd = new SAMCmd();
}

MDLController::~MDLController()
{
	currentMdl = nullptr;
	samCmd = nullptr;
}

void MDLController::newFile()
{
	FileBasedDocument::SaveResult sr = currentMdl->saveIfNeededAndUserAgrees();
	if (sr != FileBasedDocument::userCancelledSave)
	{
		currentMdl->newMDL();
	}
}
void MDLController::open()
{
	FileBasedDocument::SaveResult sr = currentMdl->saveIfNeededAndUserAgrees();
	if (sr != FileBasedDocument::userCancelledSave)
	{
		bool loadOk = currentMdl->loadFromUserSpecifiedFile(true);

		if(loadOk)
		{
			StoredSettings::getInstance()->recentFiles.addFile(currentMdl->getFile());
		}
//		else
//		{
//			DBG("Something went wrong loading the mdl file.");
//		}
	}
}

void MDLController::openFromFile(const File& mdlFile)
{
	FileBasedDocument::SaveResult sr = currentMdl->saveIfNeededAndUserAgrees();
	if (sr != FileBasedDocument::userCancelledSave)
	{
		currentMdl->loadFrom(mdlFile, true);
	}
}
void MDLController::save()
{
    if(currentMdl->changedOutside())
    {

    }
    else
    {
        if (currentMdl->save(true, true) != FileBasedDocument::savedOk)
        {
            SAM_LOG("Something went wrong saving the mdl file.");
        }
    }
}
void MDLController::saveAs()
{
	if(currentMdl->saveAsInteractive(true) != FileBasedDocument::savedOk)
	{
		SAM_LOG("Something went wrong saving the mdl file.");
	}
//    else
//    {
//        close();
//        openFromFile(currentMdl->getFile());
//    }
}

void MDLController::close()
{
	FileBasedDocument::SaveResult sr = currentMdl->saveIfNeededAndUserAgrees();
	if(sr != FileBasedDocument::userCancelledSave)
	{
		currentMdl->close();
	}
}

bool MDLController::saveAsXml()
{
    bool saveOk = false;
    FileChooser fc("Select XML file to save...",
                   File::getSpecialLocation(File::userHomeDirectory),
                   "*.xml");

    if (fc.browseForFileToSave(true))
    {
        File xmlFile (fc.getResult());
        String mdlXmlStr = currentMdl->toString();

        saveOk = Utils::writeStringToFile(mdlXmlStr, xmlFile);
    }
    return saveOk;
}

const String MDLController::generateFaust()
{
    bool r = true;
    if (StoredSettings::getInstance()->getIsExportConfirm())
        r = Alerts::confirmExport("Really export faust");

    if (StoredSettings::getInstance()->getIsUsingBuiltinSAMCompiler())
    {
        if(r)
        {
            String outPath;
            outPath << StoredSettings::getInstance()->getDataDir();
            outPath << "/";
            outPath << currentMdl->getFile().getFileNameWithoutExtension();
            outPath << ".dsp";

            return samCmd->generateFaustCodeBuiltin(currentMdl->mdlRoot, outPath);
        }
    }
    else
    {
        if (!samCmd->isPerlAvailable())
        {
            Alerts::missingPerl();
            return "Missing Perl";
        }
        if (!samCmd->isSAMpreprocessorCmdAvailable())
        {
            Alerts::missingSAMpreprocessor();
            return "Missing SAM-preprocessor";
        }
        if (!samCmd->isSynthAModelerCmdAvailable())
        {
            Alerts::missingSAM();
            return "Missing Synth-A-Modeler";
        }

        if (r)
        {
            if (currentMdl->save(true, true) != FileBasedDocument::savedOk)
                return "Canceled";

            String inPath = currentMdl->getFilePath();
            File in(inPath);
            String outFileName = in.getFileNameWithoutExtension();
            outFileName << ".dsp";

            String dataDir = StoredSettings::getInstance()->getDataDir();
            String outPath = dataDir;
            outPath << "/" << outFileName;

            // if current MDL file is not in data dir make a temp copy in data dir
            File inDataDir(dataDir + "/" + in.getFileName());
            bool saveInDataDir = false;
            if (in != inDataDir)
            {
                saveInDataDir = true;
                currentMdl->getFile().copyFileTo(inDataDir);
                inPath = inDataDir.getFullPathName();
            }
            String processText = samCmd->generateFaustCode(inPath, outPath);
            if (StoredSettings::getInstance()->getOpenFaustExport())
                Utils::openFileNative(outPath);

            // delete temp MDL file
            if (saveInDataDir)
            {
                if (!inDataDir.deleteFile())
                {
                    DBG("Deleting temp file failed!");
                }
            }
            return processText;
        }
    }
    return String::empty;
}

const String MDLController::generateExternal()
{
	if(currentMdl->getName().compare("Untitled") == 0)
		return "No mdl file\n\n";

	if(! samCmd->isFaustAvailable())
	{
		Alerts::missingFaust();
		return "Missing faust executable";
	}

	bool r = true;
	if(StoredSettings::getInstance()->getIsExportConfirm())
		r = Alerts::confirmExport("Really export faust");

	if (r)
	{
        if(currentMdl->save(true, true) != FileBasedDocument::savedOk)
            return "Canceled";

        String outStr;
        if(StoredSettings::getInstance()->getRunSAMBeforeExternal())
            outStr << generateFaust();

        // if current MDL file is not in data dir make a temp copy in data dir
        String dataDir = StoredSettings::getInstance()->getDataDir();
        File inDataDir(dataDir + "/" + currentMdl->getFile().getFileName());
        bool saveInDataDir = false;
        if (currentMdl->getFile() != inDataDir)
        {
            saveInDataDir = true;
            currentMdl->getFile().copyFileTo(inDataDir);
        }

        outStr << samCmd->generateExternal(inDataDir.getFullPathName());

        // delete temp MDL file
        if(saveInDataDir)
        {
            if(! inDataDir.deleteFile())
            {
                DBG("Deleting temp file failed!");
            }
        }
        
        return outStr;
	}
	return String::empty;
}

const String MDLController::getMDLName()
{
	return currentMdl->getName();
}

UndoManager* MDLController::getUndoManager()
{
	if(currentMdl == nullptr)
		return nullptr;

	return &currentMdl->getUndoMgr();

}

bool MDLController::perform (UndoableAction* const action, const String& actionName)
{
	if(getUndoManager() != nullptr)
	{
		getUndoManager()->beginNewTransaction(actionName);
		return getUndoManager()->perform(action, actionName);
	}
	else
	{
		return false;
	}
}

ValueTree MDLController::getMDLTree()
{
	if(currentMdl == nullptr)
		return ValueTree::invalid;

	return currentMdl->mdlRoot;
}

bool MDLController::mdlCheckAndSaveIfNeeded()
{
	FileBasedDocument::SaveResult sr = currentMdl->saveIfNeededAndUserAgrees();
	if(sr == FileBasedDocument::userCancelledSave)
		return false;
	else
		return true;

}

MDLFile* MDLController::getMDLFile() const
{
	return currentMdl.get();
}

void MDLController::setMDLFile(MDLFile* mdlFile)
{
	currentMdl = mdlFile;
	if(currentMdl != nullptr)
	{
		StoredSettings::getInstance()->recentFiles.addFile(currentMdl->getFile());
		StoredSettings::getInstance()->flush();
	}
}

void MDLController::changed()
{
    currentMdl->changed();
    mainAppWindow.updateTitle();
}

ObjectsHolder* MDLController::getHolderComponent()
{
    return mainAppWindow.getHolderComponent();
}

static const char* fileTypesToDelete[] = {".dsp", ".mdx", ".dsp.xml", ".cpp"};

void MDLController::cleanDataDir()
{
    File f = getMDLFile()->getFile();

    String dataDir = StoredSettings::getInstance()->getDataDir();
    String mdlName = f.getFileNameWithoutExtension();

    String outStrOk;
    String outStrError;

    for (int i = 0; i < 4; ++i)
    {
        String filePathToDelete = dataDir+"/"+mdlName+fileTypesToDelete[i];
        File ftd(filePathToDelete);
        if(! ftd.existsAsFile())
            continue;
        if(ftd.moveToTrash())
            outStrOk << filePathToDelete << "\n";
        else
            outStrError << filePathToDelete << "\n";
    }
    if(outStrOk.isEmpty() && outStrError.isEmpty())
    {
        SAM_CONSOLE("MSG: ", "No files were deleted.", false);
        return;
    }
    if(outStrOk.isNotEmpty())
        SAM_CONSOLE("Delete OK:\n", outStrOk, false);
    if(outStrError.isNotEmpty())
        SAM_CONSOLE("Error: ", "Could not delete\n" + outStrError, false);
}

void MDLController::cleanDataDirAll()
{
    StringArray filePathsToDelete;
    for (int i = 0; i < 4; ++i)
    {
        DirectoryIterator iter(StoredSettings::getInstance()->getDataDir(),
                               false, "*"+String(fileTypesToDelete[i]));
        while (iter.next())
        {
            filePathsToDelete.add(iter.getFile().getFullPathName());
        }
    }
    String outStrOk;
    String outStrError;
    for (int j = 0; j < filePathsToDelete.size(); ++j)
    {
        File f(filePathsToDelete[j]);
        if(! f.existsAsFile())
            continue;
        if (f.moveToTrash())
            outStrOk << filePathsToDelete[j] << "\n";
        else
            outStrError << filePathsToDelete[j] << "\n";
    }
    if(outStrOk.isEmpty() && outStrError.isEmpty())
    {
        SAM_CONSOLE("MSG: ", "No files were deleted.", false);
        return;
    }
    if(outStrOk.isNotEmpty())
        SAM_CONSOLE("Delete OK:\n", outStrOk, false);
    if(outStrError.isNotEmpty())
        SAM_CONSOLE("Error: ", "Could not delete\n" + outStrError, false);
}