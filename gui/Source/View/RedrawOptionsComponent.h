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
class RedrawOptionsPanel;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class RedrawOptionsComponent  : public Component,
                                public Slider::Listener,
                                public Button::Listener
{
public:
    //==============================================================================
    RedrawOptionsComponent (RedrawOptionsPanel* rop_);
    ~RedrawOptionsComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void buttonClicked (Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    void readOptions();
    void saveOptions();

    RedrawOptionsPanel* rop;
    PropertiesFile& pf;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Label> laAlpha;
    ScopedPointer<Slider> slAlpha;
    ScopedPointer<Label> laBeta;
    ScopedPointer<Slider> slBeta;
    ScopedPointer<Label> laK;
    ScopedPointer<Slider> slK;
    ScopedPointer<Label> laDamp;
    ScopedPointer<Slider> slDamp;
    ScopedPointer<Label> laEnergy;
    ScopedPointer<Slider> slEnergy;
    ScopedPointer<Label> laTimestep;
    ScopedPointer<Slider> slTimestep;
    ScopedPointer<TextButton> btOk;
    ScopedPointer<TextButton> btCancel;
    ScopedPointer<ToggleButton> btToggleRandomize;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RedrawOptionsComponent)
};

//[EndFile] You can add extra defines here...
}
//[/EndFile]
