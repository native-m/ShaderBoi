/*
  ==============================================================================

    GLView.h
    Created: 13 Jan 2019 7:12:05pm
    Author:  USER

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <mutex>
#include "SBTimer.h"

struct MainCanvas;
class SBCodeEditor;

//==============================================================================
/*
*/
class GLView : public OpenGLAppComponent, private AsyncUpdater
{
public:
    GLView(const String& shader, SBCodeEditor* codeEditor);
    ~GLView();

    void initialise() override;
    void shutdown() override;
    void render() override;
    void paint (Graphics&) override;
    void resized() override;
	void paused(bool state);

	void lock();
	void unlock();
	void setShaderCode(int index, const String& newShader);
	void compileShader();
	const String& getLastErrorMessage();

private:
	void handleAsyncUpdate() override;

private:

	std::unique_ptr<OpenGLShaderProgram> m_pShader;
	std::unique_ptr<MainCanvas> m_pMainCanvas;
	SBCodeEditor* m_pCodeEditor;
	String m_strShader;
	String m_strLastError;
	bool m_bPaused;
	bool m_bWait;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GLView)
};
