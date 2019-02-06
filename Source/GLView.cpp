/*
  ==============================================================================

    GLView.cpp
    Created: 13 Jan 2019 7:12:05pm
    Author:  Native-M

  ==============================================================================
*/

#include "GLView.h"
#include "SBCodeEditor.h"

const char* g_pGlVertexShader =
"#version 400\n"
"attribute vec3 position;\n"
"attribute vec2 texCoord;\n"
"\n"
"varying vec2 fUvCoord;\n"
"\n"
"void main()\n"
"{\n"
"	fUvCoord = texCoord;\n"
"	gl_Position = vec4(position, 1.0);\n"
"}\n";

//const char* g_pGlFragmentShader =
//"#version 400\n"
//"varying vec2 fUvCoord;\n"
//"uniform float fTime;\n"
//"\n"
//"void main()\n"
//"{\n"
//"	vec2 uv2 = fUvCoord * 4.0;\n"
//"	for(int n = 0; n < 10; n++)\n"
//"	{\n"
//"		float i = float(n);\n"
//"		uv2 += vec2(0.7 / i * sin(i * 2*cos(uv2.y+fTime) + fTime + 0.3 * i) + 0.8,\n"
//"					0.4 / i * sin(i * 2*cos(uv2.x+fTime) + fTime + 0.3 * i) + 1.6);\n"
//"	}\n"
//"	gl_FragColor = vec4(0.5 * cos(uv2.y + fTime*2.0) + 0.5,\n"
//"						0.5 * sin(uv2.y + 0.5*sin(2.0*fTime + uv2.y + uv2.x)) + 0.5,\n"
//"						sin(fTime + uv2.x + uv2.y), 1.0);\n"
//"}\n";

const char* g_pGlFragShaderUniforms =
"varying vec2 fUvCoord;\n"
"uniform vec2 fResolution;\n"
"uniform float fTime;\n"
"uniform float fFps;\n"
"void mainShader(out vec4 fragColor);\n"
"void main()\n"
"{\n"
"	mainShader(gl_FragColor);\n"
"}\n";

const float g_pRectShape[] = {
	 1.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-1.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-1.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

const unsigned int g_pIndicies[] = {
	0, 1, 2,
	0, 2, 3
};

struct MainCanvas
{
	MainCanvas(OpenGLContext& context, OpenGLShaderProgram& shaderProgram)
		: openGLContext(context)
	{
		openGLContext.extensions.glGenBuffers(1, &vertexBuffer);
		openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER, sizeof(g_pRectShape), g_pRectShape, GL_STATIC_DRAW);
		openGLContext.extensions.glGenBuffers(1, &indexBuffer);
		openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		openGLContext.extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_pIndicies), g_pIndicies, GL_STATIC_DRAW);

		position.reset(createAttribute(context, shaderProgram, "position"));
		texCoord.reset(createAttribute(context, shaderProgram, "texCoord"));

		fResolution.reset(createUniform(context, shaderProgram, "fResolution"));
		fTime.reset(createUniform(context, shaderProgram, "fTime"));
		fFps.reset(createUniform(context, shaderProgram, "fFps"));
		iFrameCounter.reset(createUniform(context, shaderProgram, "iFrameCounter"));
	}

	~MainCanvas()
	{
		openGLContext.extensions.glDeleteBuffers(1, &vertexBuffer);
		openGLContext.extensions.glDeleteBuffers(1, &indexBuffer);
	}

	void render()
	{
		GLView* comp = dynamic_cast<GLView*>(openGLContext.getTargetComponent());

		openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

		if (fResolution.get())
			fResolution->set((float)comp->getWidth(), (float)comp->getHeight());

		if (fTime.get())
			fTime->set((float)SBTimer::g_pTimerInstance->fCounter);

		if (fFps.get())
			fFps->set((float)SBTimer::g_pTimerInstance->getFps());

		if (iFrameCounter.get())
			iFrameCounter->set(comp->getFrameCounter());

		openGLContext.extensions.glVertexAttribPointer(position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, nullptr);
		openGLContext.extensions.glVertexAttribPointer(texCoord->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));
		openGLContext.extensions.glEnableVertexAttribArray(position->attributeID);
		openGLContext.extensions.glEnableVertexAttribArray(texCoord->attributeID);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		openGLContext.extensions.glDisableVertexAttribArray(position->attributeID);
		openGLContext.extensions.glDisableVertexAttribArray(texCoord->attributeID);
		openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
		openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	GLuint vertexBuffer, indexBuffer;
	OpenGLContext& openGLContext;
	std::unique_ptr<OpenGLShaderProgram::Attribute> position, texCoord;
	std::unique_ptr<OpenGLShaderProgram::Uniform> fTime, fResolution, fFps, iFrameCounter;

private:
	static OpenGLShaderProgram::Attribute* createAttribute(OpenGLContext& context, OpenGLShaderProgram& shaderProgram, const char* attrName)
	{
		if (context.extensions.glGetAttribLocation(shaderProgram.getProgramID(), attrName) < 0)
			return nullptr;

		return new OpenGLShaderProgram::Attribute(shaderProgram, attrName);
	}

	static OpenGLShaderProgram::Uniform* createUniform(OpenGLContext& context, OpenGLShaderProgram& shaderProgram, const char* uniName)
	{
		if (context.extensions.glGetUniformLocation(shaderProgram.getProgramID(), uniName) < 0)
			return nullptr;

		return new OpenGLShaderProgram::Uniform(shaderProgram, uniName);
	}
};

//==============================================================================
GLView::GLView(const String& shader, SBCodeEditor* codeEditor)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
	m_strShader = shader;
	m_pCodeEditor = codeEditor;
	m_bPaused = false;
}

GLView::~GLView()
{
	shutdownOpenGL();
}

void GLView::initialise()
{
	compileShader();
	m_pMainCanvas.reset(new MainCanvas(openGLContext, *m_pShader));
}

void GLView::shutdown()
{
}

void GLView::render()
{
	float scale;
	double newTime = 0.0;

	if (SBTimer::g_pTimerInstance)
	{
		jassert(OpenGLHelpers::isContextActive());
		OpenGLHelpers::clear(juce::Colour((uint8)0, (uint8)0, (uint8)0, (uint8)0));
		scale = (float)openGLContext.getRenderingScale();

		if(!m_bWait)
			compileShader();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glViewport(0, 0, roundToInt(scale * getWidth()), roundToInt(scale * getHeight()));

		m_pShader->use();
		m_pMainCanvas->render();

		if (m_bPaused != true)
			newTime = SBTimer::g_pTimerInstance->getTime();
		else
			SBTimer::g_pTimerInstance->fTime = 0.0;

		SBTimer::g_pTimerInstance->update(newTime);
	}
}

void GLView::paint (Graphics& g)
{
}

void GLView::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void GLView::paused(bool state)
{
	m_bPaused = state;
}

void GLView::lock()
{
	m_bWait = true;
}

void GLView::unlock()
{
	m_bWait = false;
}

void GLView::setShaderCode(int index, const String& newMainShader)
{
	if(index == 0)
		m_strShader = newMainShader;
}

void GLView::compileShader()
{
	if(m_strShader.isNotEmpty())
	{
		String vertexShader(g_pGlVertexShader);
		std::unique_ptr<OpenGLShaderProgram> newShader(new OpenGLShaderProgram(openGLContext));

		double version = OpenGLShaderProgram::getLanguageVersion() * 100;
		m_strShader = "#version " + String(version) + "\n" + g_pGlFragShaderUniforms + m_strShader;

		if (newShader->addVertexShader(OpenGLHelpers::translateVertexShaderToV3(vertexShader))
			&& newShader->addFragmentShader(OpenGLHelpers::translateFragmentShaderToV3(m_strShader))
			&& newShader->link())
		{
			m_pMainCanvas.reset();

			m_pShader.reset(newShader.release());
			m_pShader->use();

			m_pMainCanvas.reset(new MainCanvas(openGLContext, *m_pShader));

			m_strLastError = {};
		}
		else
		{
			m_strLastError = newShader->getLastError();

			newShader.reset();
			if(m_pShader.get())
				m_pShader->use();
		}

		m_strShader = {};
		triggerAsyncUpdate();
	}
}

const String& GLView::getLastErrorMessage()
{
	return m_strLastError;
}

void GLView::handleAsyncUpdate()
{
	CodeDocument& errMsg = m_pCodeEditor->getErrorMessageDocumentFromEditor(0);
	errMsg.replaceAllContent(m_strLastError.fromFirstOccurrenceOf("Fragment shader failed to compile with the following errors:\n", false, true));
	errMsg.clearUndoHistory();
}
