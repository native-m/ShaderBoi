/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SBShaderView.h"
#include "SBCodeEditor.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
	CodeDocument m_CodeDoc;
	std::unique_ptr<SBCodeEditor> m_pCodeEditor;
	std::unique_ptr<SBShaderView> m_pShaderView;
	StretchableLayoutManager m_PanelManager;
	StretchableLayoutResizerBar m_PanelResizer { &m_PanelManager, 1, true };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};