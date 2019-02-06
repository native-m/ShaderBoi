/*
  ==============================================================================

    SBShaderView.cpp
    Created: 14 Jan 2019 8:10:27pm
    Author:  USER

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SBShaderView.h"
#include "SBTimer.h"
#include "SBCodeEditor.h"

//==============================================================================
SBShaderView::SBShaderView(const String& initShader, SBCodeEditor* codeEditor)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
	Colour color(255, 255, 255);

	m_bPaused = false;

	m_pView.reset(new GLView(initShader, codeEditor));
	addAndMakeVisible(*m_pView);
	m_pView->setSize(getWidth(), getHeight() - 20);

	m_pPlayShape.reset(new Path());
	m_pPlayShape->addTriangle(Point<float>(4.0f, 0.0f), Point<float>(16.0f, 10.0f), Point<float>(4.0f, 20.0f));

	m_pPauseShape.reset(new Path());
	m_pPauseShape->addRectangle(Rectangle<float>(1.0f, 1.0f, 4.0f, 10.0f));
	m_pPauseShape->addRectangle(Rectangle<float>(8.0f, 1.0f, 4.0f, 10.0f));

	m_pRewindShape.reset(new Path());
	m_pRewindShape->addTriangle(Point<float>(0.5f, 0.0f), Point<float>(0.0f, 0.5f), Point<float>(0.5f, 1.0f));
	m_pRewindShape->addTriangle(Point<float>(1.0f, 0.0f), Point<float>(0.5f, 0.5f), Point<float>(1.0f, 1.0f));

	m_pPlayBtn.reset(new ShapeButton("Play pause button", color, color.darker(0.2f), color.darker(0.5f)));
	m_pPlayBtn->setShape(*m_pPauseShape.get(), false, true, false);
	m_pPlayBtn->setSize(18, 18);
	m_pPlayBtn->onClick = [this] {
		m_bPaused = !m_bPaused;
		if(!m_bPaused)
			m_pPlayBtn->setShape(*m_pPauseShape.get(), false, true, false);
		else
			m_pPlayBtn->setShape(*m_pPlayShape.get(), false, true, false);
		SBTimer::g_pTimerInstance->reset();
		m_pView->paused(m_bPaused);
	};
	addAndMakeVisible(*m_pPlayBtn);

	m_pRewindBtn.reset(new ShapeButton("Rewind button", color, color.darker(0.2f), color.darker(0.5f)));
	m_pRewindBtn->setShape(*m_pRewindShape, false, true, false);
	m_pRewindBtn->setSize(25, 18);
	m_pRewindBtn->onClick = [this] {
		SBTimer::g_pTimerInstance->fCounter = 0;
	};
	addAndMakeVisible(*m_pRewindBtn);

	m_fFpsElapsedTime = 0.0;
	m_fCurrentFps = 0.0;
	startTimerHz(60);
}

SBShaderView::~SBShaderView()
{
	stopTimer();
}

void SBShaderView::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId).darker());   // clear the background

    g.setColour(Colours::white);
    g.setFont(14.0f);
    g.drawText(
		"Time: " + String(SBTimer::g_pTimerInstance->fCounter),
		Rectangle<int>(4, m_pView->getHeight() + 3, 100, 14),
		Justification::topLeft,
		true);

	m_fFpsElapsedTime = m_fFpsElapsedTime + m_Timer.fFrameTime;

	// Update FPS every 5 fps
	if(m_fFpsElapsedTime > 0.2)
	{
		if(SBTimer::g_pTimerInstance)
			m_fCurrentFps = SBTimer::g_pTimerInstance->getFps();
		m_fFpsElapsedTime = 0.0;
	}

	g.drawText(
		"FPS: " + String(m_fCurrentFps),
		Rectangle<int>(104, m_pView->getHeight() + 3, 100, 14),
		Justification::topLeft,
		true);

	m_Timer.update(m_Timer.getTime());
}

void SBShaderView::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
	m_pView->setSize(getWidth(), getHeight() - 20);
	m_pPlayBtn->setTopLeftPosition(220, getHeight() - 19);
	m_pRewindBtn->setTopLeftPosition(250, getHeight() - 19);
}

GLView * SBShaderView::getShaderViewer()
{
	return m_pView.get();
}

void SBShaderView::timerCallback()
{
	repaint();
}
