/*
  ==============================================================================

    SBShaderView.h
    Created: 14 Jan 2019 8:10:27pm
    Author:  USER

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GLView.h"

class SBCodeEditor;

//==============================================================================
/*
*/
class SBShaderView    : public Component, private Timer
{
public:
    SBShaderView(const String& initShader, SBCodeEditor* codeEditor);
    ~SBShaderView();

    void paint (Graphics&) override;
    void resized() override;
	GLView* getShaderViewer();

private:
	void timerCallback();

private:
	std::unique_ptr<GLView> m_pView;
	SBTimer m_Timer;
	double m_fFpsElapsedTime, m_fCurrentFps;
	std::unique_ptr<Path> m_pPlayShape, m_pPauseShape, m_pRewindShape;
	std::unique_ptr<ShapeButton> m_pPlayBtn, m_pRewindBtn;
	bool m_bPaused;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SBShaderView)
};
