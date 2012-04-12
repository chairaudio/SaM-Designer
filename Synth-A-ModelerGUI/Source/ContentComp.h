/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  12 Apr 2012 3:26:55am

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_CONTENTCOMP_CONTENTCOMP_7E8CF65B__
#define __JUCER_HEADER_CONTENTCOMP_CONTENTCOMP_7E8CF65B__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "MainWindow.h"
#include "DebugWindow.h"
#include "ObjComp.h"
#include "MDLController.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class ContentComp  : public Component,
                     public MenuBarModel,
                     public ApplicationCommandTarget
{
public:
    //==============================================================================
    ContentComp (MainAppWindow& mainWindow_, AppController& appController_,
    		MDLController& mdlController_, ObjController& objController_);
    ~ContentComp();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    const StringArray getMenuBarNames();
    const PopupMenu getMenuForIndex (int menuIndex, const String&);
	void menuItemSelected(int menuItemID, int);
	ApplicationCommandTarget* getNextCommandTarget();
	void getAllCommands(Array<CommandID>& commands);
	void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result);
	bool perform(const InvocationInfo& info);
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();



    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	MainAppWindow& mainWindow;
	AppController& appController;
	MDLController& mdlController;
	ObjController& objController;
	ObjComp* objComp;
    //[/UserVariables]

    //==============================================================================


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    ContentComp (const ContentComp&);
    const ContentComp& operator= (const ContentComp&);
};


#endif   // __JUCER_HEADER_CONTENTCOMP_CONTENTCOMP_7E8CF65B__
