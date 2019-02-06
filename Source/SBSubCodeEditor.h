/*
  ==============================================================================

    SBSubCodeEditor.h
    Created: 16 Jan 2019 12:02:24am
    Author:  USER

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SBGlslTokeniser.h"

//==============================================================================
/*
*/
struct SBSubCodeEditorFunctions
{
	static String getLeadingWhitespace(String line);
	static int getBraceCount(String::CharPointerType line);
	static bool getIndentForCurrentBlock(CodeDocument::Position pos, const String& tab, String& blockIndent, String& lastLineIndent);
};

class SBSubCodeEditor : public CodeEditorComponent
{
public:
    SBSubCodeEditor(CodeDocument& doc, CodeTokeniser* pTokeniser);
    ~SBSubCodeEditor();

	void handleReturnKey() override;
	void insertTextAtCaret(const String& newText) override;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SBSubCodeEditor)
};
