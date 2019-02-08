/*
  ==============================================================================

    SBGlslTokeniser.cpp
    Created: 16 Jan 2019 2:53:34pm
    Author:  USER

  ==============================================================================
*/

#include "SBGlslTokeniser.h"

static const char* const keywords2[] = {
	"do", "if", "in",
	nullptr,
	nullptr
};

static const char* const keywords3[] = {
	"for", "out", "int",
	nullptr,
	"sin", "cos", "tan", "pow", "exp", "log", "abs",
	"mod", "min", "max", "mix", "fma", "dot",
	"any", "all", "not", nullptr
};

static const char* const keywords4[] = {
	"flat", "case", "else", "void", "bool", "true", "vec2",
	"vec3", "vec4", "uint", "mat2", "mat3", "mat4",
	"lowp",
	nullptr,
	"asin", "acos", "atan", "sinh", "cosh", "tanh", "exp2",
	"log2", "sqrt", "sign", "ceil", "modf", "step",
	nullptr
};

static const char* const keywords5[] = {
	"const", "patch", "break", "while", "inout", "false", "float",
	"ivec2", "ivec3", "ivec4", "bvec2", "bvec3", "bvec4",
	"uvec2", "uvec3", "uvec4", "dvec2", "dvec3", "dvec4",
	"dmat2", "dmat3", "dmat4", "highp",
	nullptr,
	"asinh", "acosh", "atanh", "floor", "trunc", "round", "fract",
	"clamp", "isnan", "isinf", "frexp", "ldexp", "cross",
	"equal", nullptr
};

static const char* const keywords6[] = {
	"buffer", "shared", "layout", "smooth", "sample", "switch", "double",
	"return", "mat2x2", "mat2x3", "mat2x4", "mat3x2", "mat3x3",
	"mat3x4", "mat4x2", "mat4x3", "mat4x4", "struct",
	nullptr,
	"length", nullptr
};

static const char* const keywords7[] = {
	"uniform", "varying", "precise", "default", "discard", "dmat2x2", "dmat2x3",
	"dmat2x4", "dmat3x2", "dmat3x3", "dmat3x4", "dmat4x2", "dmat4x3",
	"dmat4x4", "mediump", "image1D", "image2D", "image3D",
	nullptr,
	"radians", "degrees", "reflect", "refract", "inverse", "findLSB", "findMSB",
	"texture", nullptr
};

static const char* const keywordsOther[] = {
	"attribute", "coherent", "volatile", "restrict", "readonly", "writeonly", "atomic_uint",
	"centroid", "noperspective", "invariant", "continue", "subroutine", "precision",
	"sampler1D", "sampler1DShadow", "sampler1DArray", "sampler1DArrayShadow", "isampler1D", "isampler1DArray",
	"usampler1D", "usampler1DArray", "sampler2D", "sampler2DShadow", "sampler2DArray", "sampler2DArrayShadow",
	"isampler2D", "isampler2DArray", "usampler2D", "usampler2DArray", "sampler2DRect", "sampler2DRectShadow",
	"isampler2DRect", "usampler2DRect", "sampler2DMS", "isampler2DMS", "usampler2DMS", "sampler2DMSArray",
	"isampler2DMSArray", "usampler2DMSArray", "sampler3D", "isampler3D", "usampler3D", "samplerCube",
	"samplerCubeShadow", "isamplerCube", "usamplerCube", "samplerCubeArray", "samplerCubeArrayShadow", "isamplerCubeArray",
	"usamplerCubeArray", "samplerBuffer", "isamplerBuffer", "usamplerBuffer", "iimage1D", "uimage1D",
	"image1DArray", "iimage1DArray", "uimage1DArray", "iimage2D", "uimage2D", "image2DArray",
	"iimage2DArray", "uimage2DArray", "image2DRect", "iimage2DRect", "uimage2DRect", "image2DMS",
	"iimage2DMS", "uimage2DMS", "image2DMSArray", "iimage2DMSArray", "uimage2DMSArray", "iimage3D",
	"uimage3D", "imageCube", "iimageCube", "uimageCube", "imageCubeArray", "iimageCubeArray",
	"uimageCubeArray", "imageBuffer", "iimageBuffer", "uimageBuffer",
	nullptr,
	"inversesqrt", "roundEven", "smoothstep", "floatBitsToInt", "floatBitsToUint", "intBitsToFloat", "uintBitsToFloat",
	"packUnorm2x16", "packSnorm2x16", "packUnorm4x8", "packSnorm4x8", "unpackUnorm2x16", "unpackSnorm2x16",
	"unpackUnorm4x8", "unpackSnorm4x8", "packHalf2x16", "unpackHalf2x16", "packDouble2x32", "unpackDouble2x32",
	"distance", "normalize", "ftransform", "faceforward", "matrixCompMult", "outerProduct",
	"transform", "determinant", "lessThan", "lessThanEqual", "greaterThan", "greaterThanEqual",
	"notEqual", "uaddCarry", "usubBorrow", "umulExtended", "imulExtended", "bitfieldExtract",
	"bitfieldInsert", "bitfieldReverse", "bitCount", "textureSize", "textureQueryLod", "textureQueryLevels",
	"textureProj", "textureLod", "textureOffset", "texelFetch", "texelFetchOffset", "textureProjOffset",
	"textureLodOffset", "textureProjLod", "textureProjLodOffset", "textureGrad", "textureGradOffset", "textureProjGrad",
	"textureProjGradOffset", "textureGather", "textureGatherOffset", "textureGatherOffsets", "texture1D", "texture2D",
	"texture3D", "shadow1D", "shadow2D", "shadow1DProj", "shadow2DProj", "shadow1DLod",
	"shadow2DLod", "shadow1DProjLod", "shadow2DProjLod", "atomicCounterIncrement", "atomicCounterDecrement", "atomicCounter",
	"atomicCounterAdd", "atomicCounterSubtract", "atomicCounterMin", "atomicCounterMax", "atomicCounterAnd", "atomicCounterOr",
	"atomicCounterXor", "atomicCounterExchange", "atomicCounterCompSwap", "atomicAdd", "atomicMin", "atomicMax",
	"atomicAnd", "atomicOr", "atomicXor", "atomicExchange", "atomicCompSwap", nullptr
};




struct SBGlslTokeniserFunction
{
	static void skipPreprocessor(CodeDocument::Iterator& source);
	static int isReservedKeyword(String::CharPointerType token, const int tokenLength);
	static bool isHexLiteral(CodeDocument::Iterator& source);
	static bool isIntLiteral(CodeDocument::Iterator& source);
	static bool isFloatLiteral(CodeDocument::Iterator& source);
	static int parseNumber(CodeDocument::Iterator& source);
	static int parseIdentifier(CodeDocument::Iterator& source);
};

SBGlslTokeniser::SBGlslTokeniser()
{
}

SBGlslTokeniser::~SBGlslTokeniser()
{
}

int SBGlslTokeniser::readNextToken(CodeDocument::Iterator & source)
{
	source.skipWhitespace();
	auto c = source.peekNextChar();

	switch (c)
	{
	case 0:
		break;

	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	case '.':
	{
		int type = SBGlslTokeniserFunction::parseNumber(source);
		if (type == tokenType_error)
		{
			source.skip();
			if (c == '.')
				return tokenType_punctuation;
		}

		return type;
	}

	case ',':
	case ';':
	case ':':
		source.skip();
		return tokenType_punctuation;

	case '(': case ')':
	case '{': case '}':
	case '[': case ']':
		source.skip();
		return tokenType_bracket;

	case '"':
	case '\'':
	{
		juce_wchar cc = 0;
		juce_wchar quote = source.nextChar();

		while (1)
		{
			cc = source.nextChar();

			if (c == quote || c == 0)
				break;

			if (cc == '\\')
				source.skip();
		}

		return tokenType_string;
	}

	case '+':
	{
		int type;
		juce_wchar cc = 0;

		source.skip();
		type = SBGlslTokeniserFunction::parseNumber(source);

		if(type == tokenType_error)
		{
			cc = source.peekNextChar();

			if (cc == '+' || cc == '=')
				source.skip();

			return tokenType_operator;
		}

		return type;
	}

	case '-':
	{
		int type;
		juce_wchar cc = 0;

		source.skip();
		type = SBGlslTokeniserFunction::parseNumber(source);

		if (type == tokenType_error)
		{
			cc = source.peekNextChar();

			if (cc == '-' || cc == '=')
				source.skip();

			return tokenType_operator;
		}

		return type;
	}

	case '*': case '%':
	case '=': case '!':
	{
		juce_wchar cc = 0;

		source.skip();

		cc = source.peekNextChar();
		if (cc == '=')
			source.skip();

		return tokenType_operator;
	}

	case '/':
	{
		juce_wchar cc = 0;
		source.skip();
		cc = source.peekNextChar();

		if (cc == '/')
		{
			source.skipToEndOfLine();
			return tokenType_comment;
		}

		if (cc == '*')
		{
			bool star = false;
			source.skip();

			while (1)
			{
				juce_wchar ccc = source.nextChar();

				if (ccc == 0 || (ccc == '/' && star))
					break;

				star = (ccc == '*');
			}

			return tokenType_comment;
		}

		if (cc == '=')
			source.skip();

		return tokenType_operator;
	}

	case '?':
	case '~':
		source.skip();
		return tokenType_operator;

	case '<': case '>':
	case '|': case '&': case '^':
	{
		juce_wchar cc = 0;

		source.skip();

		cc = source.peekNextChar();
		if (cc == c || cc == '=')
			source.skip();

		return tokenType_operator;
	}

	case '#':
		SBGlslTokeniserFunction::skipPreprocessor(source);
		return tokenType_preprocessor;

	default:
		if (iswalpha((wint_t)c) || c == '_' || c == '@')
			return SBGlslTokeniserFunction::parseIdentifier(source);

		source.skip();
		break;
	}

	return tokenType_error;
}

CodeEditorComponent::ColourScheme SBGlslTokeniser::getDefaultColourScheme()
{
	CodeEditorComponent::ColourScheme cs;

	struct Type
	{
		const char* name;
		juce::uint32 colour;
	};

	const Type types[] =
	{
		{ "Error",					0xffe60000 },
		{ "Comment",				0xff72d20c },
		{ "Keyword",				/*0xffee6f6f*/ Colour(86, 156, 214).getARGB() },
		{ "Operator",				/*0xffc4eb19*/ Colour(180, 180, 180).getARGB() },
		{ "Identifier",				0xffcfcfcf },
		{ "Integer",				0xff42c8c4 },
		{ "Float",					/*0xff885500*/ Colour(214, 157, 133).getARGB() },
		{ "String",					0xffbc45dd },
		{ "Bracket",				0xffcfcfcf },
		{ "Punctuation",			0xffcfbeff },
		{ "Preprocessor Text",		0xfff8f631 },
		{ "Intrinsic Functions",	Colour(184, 215, 163).getARGB() }
	};

	for (auto& t : types)
		cs.set(t.name, Colour(t.colour));

	return cs;
}

void SBGlslTokeniserFunction::skipPreprocessor(CodeDocument::Iterator & source)
{
	bool backslash = false;

	while (1)
	{
		juce_wchar cc = source.peekNextChar();

		if (cc == '"')
		{
			juce_wchar ccc;
			juce_wchar quote = source.nextChar();

			while (1)
			{
				ccc = source.nextChar();

				if (cc == quote || cc == 0)
					break;

				if (ccc == '\\')
					source.skip();
			}

			continue;
		}

		if (cc == '/')
		{
			juce_wchar ccc;
			CodeDocument::Iterator next(source);
			next.skip();
			ccc = next.peekNextChar();

			if (ccc == '/' || ccc == '*')
				return;
		}

		if (cc == 0)
			break;

		if (cc == '\n' || cc == '\r')
		{
			source.skipToEndOfLine();
			if (backslash)
				skipPreprocessor(source);
			break;
		}

		if (cc == '\\')
			backslash = true;

		source.skip();
	}
}

int SBGlslTokeniserFunction::isReservedKeyword(String::CharPointerType token, const int tokenLength)
{
	const char* const* k;
	int i = 0;

	switch (tokenLength)
	{
	case 2: k = keywords2; break;
	case 3: k = keywords3; break;
	case 4: k = keywords4; break;
	case 5: k = keywords5; break;
	case 6: k = keywords6; break;
	case 7: k = keywords7; break;
	default:
		if (tokenLength < 2 || tokenLength > 22)
			return false;

		k = keywordsOther;
		break;
	}

	for (; k[i] != nullptr; i++)
	{
		if (token.compare(CharPointer_ASCII(k[i])) == 0)
			return 1;
	}

	i++;
	for (; k[i] != nullptr; i++)
	{
		if (token.compare(CharPointer_ASCII(k[i])) == 0)
			return 2;
	}

	return 0;
}

bool SBGlslTokeniserFunction::isHexLiteral(CodeDocument::Iterator & source)
{
	juce_wchar c = source.peekNextChar();
	int numDigits = 0;

	if (c == '+' || c == '-')
		source.skip();

	c = source.peekNextChar();
	if (c == '0')
	{
		source.skip();
		c = source.peekNextChar();
		if (c == 'x' || c == 'X')
			source.skip();
		else return false;
	}

	c = source.peekNextChar();
	while ((c >= '0' && c <= '9')
		|| (c >= 'A' && c <= 'F')
		|| (c >= 'a' && c <= 'f'))
	{
		source.skip();
		numDigits++;
		c = source.peekNextChar();
	}

	if (numDigits == 0)
		return false;

	if (c == 'u' || c == 'U')
		source.skip();

	return true;
}

bool SBGlslTokeniserFunction::isIntLiteral(CodeDocument::Iterator & source)
{
	juce_wchar c = source.peekNextChar();
	int numDigits = 0;

	if (c == '-' || c == '+')
		source.skip();

	c = source.peekNextChar();
	while ((c >= '0' && c <= '9'))
	{
		source.skip();
		numDigits++;
		c = source.peekNextChar();
	}

	if (numDigits == 0)
		return false;

	if (c == 'u' || c == 'U')
		source.skip();

	return true;
}

bool SBGlslTokeniserFunction::isFloatLiteral(CodeDocument::Iterator & source)
{
	juce_wchar c = source.peekNextChar();
	int numDigits = 0;
	bool hasPoint = false;
	bool hasExp = false;

	if (c == '+' || c == '-')
		source.skip();

	c = source.peekNextChar();
	while((c >= '0' && c <= '9'))
	{
		source.skip();
		numDigits++;
		c = source.peekNextChar();
	}

	if (c == '.')
	{
		source.skip();
		c = source.peekNextChar();
		while ((c >= '0' && c <= '9'))
		{
			source.skip();
			numDigits++;
			c = source.peekNextChar();
		}

		hasPoint = true;
	}

	if (numDigits == 0)
		return false;

	c = source.peekNextChar();
	if (c == 'e' || c == 'E')
	{
		int numExpDigits = 0;
		source.skip();

		c = source.peekNextChar();
		if (c == '+' || c == '-')
			source.skip();

		c = source.peekNextChar();
		while ((c >= '0' && c <= '9'))
		{
			source.skip();
			numExpDigits++;
			c = source.peekNextChar();
		}

		if (numExpDigits == 0)
			return false;

		hasExp = true;
	}

	c = source.peekNextChar();
	if (c == 'f' || c == 'F')
		source.skip();
	else if (c == 'l' || c == 'L')
	{
		source.skip();
		c = source.peekNextChar();
		if (c == 'f' || c == 'F')
			source.skip();
		else return false;
	}
	else if (!(hasPoint || hasExp))
		return false;

	return true;
}

int SBGlslTokeniserFunction::parseNumber(CodeDocument::Iterator & source)
{
	const CodeDocument::Iterator orig(source);

	if (isFloatLiteral(source)) return SBGlslTokeniser::tokenType_float;
	source = orig;

	if (isHexLiteral(source)) return SBGlslTokeniser::tokenType_integer;
	source = orig;

	if (isIntLiteral(source)) return SBGlslTokeniser::tokenType_integer;
	source = orig;

	return 0;
}

int SBGlslTokeniserFunction::parseIdentifier(CodeDocument::Iterator & source)
{
	juce_wchar c;
	int tokenLength = 0;
	String::CharPointerType::CharType possibleIdentifier[100];
	String::CharPointerType possible(possibleIdentifier);

	c = source.peekNextChar();
	while (iswalnum((wint_t)c) || c == '_' || c == '@')
	{
		juce_wchar cc;
		cc = source.nextChar();

		if (tokenLength < 22)
			possible.write(c);

		tokenLength++;
		c = source.peekNextChar();
	}

	if (tokenLength > 1 && tokenLength <= 22)
	{
		int ret = 0;

		possible.writeNull();

		ret = isReservedKeyword(String::CharPointerType(possibleIdentifier), tokenLength);

		switch (ret)
		{
		case 0:
			break;
		case 1:
			return SBGlslTokeniser::tokenType_keyword;
		case 2:
			return SBGlslTokeniser::tokenType_intrinsic;
		default:
			break;
		}
	}

	return SBGlslTokeniser::tokenType_identifier;
}
