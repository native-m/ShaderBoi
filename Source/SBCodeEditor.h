/*
  ==============================================================================

    SBCodeEditor.h
    Created: 16 Jan 2019 12:19:04am
    Author:  USER

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SBSubCodeEditor.h"

class GLView;
struct SBCodeEditorContainer;

//==============================================================================
/*
*/
class SBCodeEditor : public Component
{
public:
    SBCodeEditor();
    ~SBCodeEditor();

	void paint(Graphics& g) override;
    void resized() override;

	void setShaderViewer(GLView* view);

	CodeDocument& getErrorMessageDocumentFromEditor(int index);
	SBCodeEditorContainer* getCodeEditorContainer(int index);
	int getCodeEditorContainerCount();
	GLView* getViewer();

private:
	std::unique_ptr<TabbedComponent> m_pCodeTabs;
	std::unique_ptr<TextButton> m_pNewBtn;
	std::unique_ptr<TextButton> m_pOpenBtn;
	std::unique_ptr<TextButton> m_pSaveBtn;
	std::unique_ptr<TextButton> m_pCompileBtn;
	GLView* m_pShaderViewer;
	CriticalSection m_ObjectLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SBCodeEditor)
};
