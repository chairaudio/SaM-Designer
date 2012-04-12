/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic outline for a simple desktop window.

    Author: Peter Vasil

  ==============================================================================
*/

#ifndef __MAINWINDOW_H_3C77CBD5__
#define __MAINWINDOW_H_3C77CBD5__

#include "../JuceLibraryCode/JuceHeader.h"
#include "DebugWindow.h"
#include "AppController.h"
#include "MDLController.h"
#include "ObjController.h"

//==============================================================================
class MainAppWindow   : public DocumentWindow
{
public:
    //==============================================================================
    MainAppWindow();
    ~MainAppWindow();

    void closeButtonPressed();

    // the command manager object used to dispatch command events
    ApplicationCommandManager commandManager;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainAppWindow)

    ScopedPointer<DebugWindow> debugWindow;
    ScopedPointer<AppController> appController;
    ScopedPointer<MDLController> mdlController;
    ScopedPointer<ObjController> objController;
};


#endif  // __MAINWINDOW_H_3C77CBD5__
