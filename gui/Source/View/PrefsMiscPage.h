/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.2.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"

namespace synthamodeler
{
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MiscPage  : public Component,
                  public FilenameComponentListener,
                  public Button::Listener
{
public:
    //==============================================================================
    MiscPage ();
    ~MiscPage();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void filenameComponentChanged(FilenameComponent* fileComponentThatHasChanged);
    void readValues();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Label> laDatDir;
    ScopedPointer<FilenameComponent> fcDataDir;
    ScopedPointer<Label> laFaustDir;
    ScopedPointer<FilenameComponent> fcFaustDir;
    ScopedPointer<Label> laExternalEditor;
    ScopedPointer<FilenameComponent> fcExternalEditor;
    ScopedPointer<ToggleButton> tbAutoCorrect;
    ScopedPointer<ToggleButton> tbRunSAMBeforeExternal;
    ScopedPointer<ToggleButton> tbConfirmBeforeGeneration;
    ScopedPointer<ToggleButton> tbUseMDLX;
    ScopedPointer<ToggleButton> tbLogging;
    ScopedPointer<ToggleButton> tbRedrawWhenNoPos;
    ScopedPointer<ToggleButton> tbReopenLastModels;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MiscPage)
};

//[EndFile] You can add extra defines here...
}
//[/EndFile]
