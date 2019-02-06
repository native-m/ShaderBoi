/*
  ==============================================================================

    SBSubCodeEditor.cpp
    Created: 16 Jan 2019 12:02:24am
    Author:  USER

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SBSubCodeEditor.h"

//==============================================================================

SBSubCodeEditor::SBSubCodeEditor(CodeDocument& doc, CodeTokeniser* pTokeniser)
	: CodeEditorComponent(doc, pTokeniser)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
}

SBSubCodeEditor::~SBSubCodeEditor()
{
}

void SBSubCodeEditor::handleReturnKey()
{
	CodeEditorComponent::handleReturnKey();

	CodeDocument::Position pos(getCaretPos());

	String blockIndent, lastLineIndent;
	SBSubCodeEditorFunctions::getIndentForCurrentBlock(pos, getTabString(getTabSize()), blockIndent, lastLineIndent);

	const String remainderOfBrokenLine(pos.getLineText());
	const int numLeadingWSChar = SBSubCodeEditorFunctions::getLeadingWhitespace(remainderOfBrokenLine).length();

	if (numLeadingWSChar > 0)
		getDocument().deleteSection(pos, pos.movedBy(numLeadingWSChar));

	if (remainderOfBrokenLine.trimStart().startsWithChar('}'))
	{
		CodeDocument::Position lastPos(getCaretPos().movedBy(-1));
		insertTextAtCaret(blockIndent);
		setHighlightedRegion(Range<int>(lastPos.getPosition(), lastPos.getPosition()));
		handleReturnKey();
	}
	else
		insertTextAtCaret(lastLineIndent);

	const String previousLine(pos.movedByLines(-1).getLineText());
	const String trimmedPreviousLine(previousLine.trim());

	if ((trimmedPreviousLine.startsWith("if ")
		|| trimmedPreviousLine.startsWith("if(")
		|| trimmedPreviousLine.startsWith("for ")
		|| trimmedPreviousLine.startsWith("for(")
		|| trimmedPreviousLine.startsWith("while ")
		|| trimmedPreviousLine.startsWith("while("))
		&& trimmedPreviousLine.endsWithChar(')'))
	{
		insertTabAtCaret();
	}
}

void SBSubCodeEditor::insertTextAtCaret(const String & newText)
{
	if (getHighlightedRegion().isEmpty())
	{
		const CodeDocument::Position pos(getCaretPos());

		if ((newText == "{" || newText == "}")
			&& pos.getLineNumber() > 0
			&& pos.getLineText().trim().isEmpty())
		{
			moveCaretToStartOfLine(true);

			String blockIndent, lastLineIndent;
			if (SBSubCodeEditorFunctions::getIndentForCurrentBlock(pos, getTabString(getTabSize()), blockIndent, lastLineIndent))
			{
				CodeEditorComponent::insertTextAtCaret(blockIndent);

				if (newText == "{")
				{
					insertTabAtCaret();
				}
			}
		}
	}

	CodeEditorComponent::insertTextAtCaret(newText);

	if (newText == "{")
	{
		int pos = getCaretPos().getPosition();
		CodeEditorComponent::insertTextAtCaret("}");
		CodeEditorComponent::setHighlightedRegion(Range<int>(pos, pos));
	}
}

String SBSubCodeEditorFunctions::getLeadingWhitespace(String line)
{
	line = line.removeCharacters("\r\n");
	const String::CharPointerType endOfLeadingWS(line.getCharPointer().findEndOfWhitespace());
	return String(line.getCharPointer(), endOfLeadingWS);
}

int SBSubCodeEditorFunctions::getBraceCount(String::CharPointerType line)
{
	int braces = 0;

	while (1)
	{
		const juce_wchar c = line.getAndAdvance();

		if (c == 0) break;
		else if (c == '{') braces++;
		else if (c == '}') braces--;
		else if (c == '/')
		{
			if (*line == '/')
				break;
		}
		else if (c == '"' || c == '\'')
		{
			while (!(line.isEmpty() || line.getAndAdvance() == c))
			{
			}
		}
	}

	return braces;
}

bool SBSubCodeEditorFunctions::getIndentForCurrentBlock(CodeDocument::Position pos, const String & tab, String & blockIndent, String & lastLineIndent)
{
	int braceCount = 0;
	bool indentFound = false;

	while (pos.getLineNumber() > 0)
	{
		pos = pos.movedByLines(-1);

		const String line(pos.getLineText());
		const String trimmedLine(line.trimStart());

		braceCount += getBraceCount(trimmedLine.getCharPointer());

		if (braceCount > 0)
		{
			blockIndent = getLeadingWhitespace(line);

			if (!indentFound)
				lastLineIndent = blockIndent + tab;

			return true;
		}

		if ((!indentFound) && trimmedLine.isNotEmpty())
		{
			indentFound = true;
			lastLineIndent = getLeadingWhitespace(line);
		}
	}

	return false;
}
