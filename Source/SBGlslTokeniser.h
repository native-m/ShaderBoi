/*
  ==============================================================================

    SBGlslTokeniser.h
    Created: 16 Jan 2019 2:53:34pm
    Author:  USER

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class SBGlslTokeniser : public CodeTokeniser
{
public:
	SBGlslTokeniser();
	~SBGlslTokeniser();

    int readNextToken(CodeDocument::Iterator& source) override;
    CodeEditorComponent::ColourScheme getDefaultColourScheme() override;

	enum TokenType
	{
		tokenType_error = 0,
		tokenType_comment,
		tokenType_keyword,
		tokenType_operator,
		tokenType_identifier,
		tokenType_integer,
		tokenType_float,
		tokenType_string,
		tokenType_bracket,
		tokenType_punctuation,
		tokenType_preprocessor,
		tokenType_intrinsic
	};
};