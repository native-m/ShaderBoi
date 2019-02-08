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

// Canvas shape (a rectangle shape)
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
		// Create vertex & index buffer
		openGLContext.extensions.glGenBuffers(1, &vertexBuffer);
		openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER, sizeof(g_pRectShape), g_pRectShape, GL_STATIC_DRAW);
		openGLContext.extensions.glGenBuffers(1, &indexBuffer);
		openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		openGLContext.extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_pIndicies), g_pIndicies, GL_STATIC_DRAW);

		// Create a vertex attribute
		position.reset(createAttribute(context, shaderProgram, "position"));
		texCoord.reset(createAttribute(context, shaderProgram, "texCoord"));

		// Create an uniform
		fResolution.reset(createUniform(context, shaderProgram, "fResolution")); // Canvas resolution
		fTime.reset(createUniform(context, shaderProgram, "fTime")); // Current time
		fFps.reset(createUniform(context, shaderProgram, "fFps")); // FPS
		iFrameCounter.reset(createUniform(context, shaderProgram, "iFrameCounter")); // Frame counter
	}

	~MainCanvas()
	{
		// Delete buffers
		openGLContext.extensions.glDeleteBuffers(1, &vertexBuffer);
		openGLContext.extensions.glDeleteBuffers(1, &indexBuffer);
	}

	void render()
	{
		GLView* comp = dynamic_cast<GLView*>(openGLContext.getTargetComponent());

		// Select our buffer
		openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

		// Set all uniforms into shader
		if (fResolution.get())
			fResolution->set((float)comp->getWidth(), (float)comp->getHeight());

		if (fTime.get())
			fTime->set((float)SBTimer::g_pTimerInstance->fCounter);

		if (fFps.get())
			fFps->set((float)SBTimer::g_pTimerInstance->getFps());

		if (iFrameCounter.get())
			iFrameCounter->set(comp->getFrameCounter());

		// Create & enable vertex attributes
		openGLContext.extensions.glVertexAttribPointer(position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, nullptr);
		openGLContext.extensions.glVertexAttribPointer(texCoord->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));
		openGLContext.extensions.glEnableVertexAttribArray(position->attributeID);
		openGLContext.extensions.glEnableVertexAttribArray(texCoord->attributeID);

		// D R A W  C A N V A S
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		// Disable vertex attributes and unbind our buffer
		openGLContext.extensions.glDisableVertexAttribArray(position->attributeID);
		openGLContext.extensions.glDisableVertexAttribArray(texCoord->attributeID);
		openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
		openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	GLuint vertexBuffer, indexBuffer, uniformBuffer;
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

	// Check if global timer is exist
	if (SBTimer::g_pTimerInstance)
	{
		jassert(OpenGLHelpers::isContextActive());

		// Clear screen
		OpenGLHelpers::clear(juce::Colour((uint8)0, (uint8)0, (uint8)0, (uint8)0));
		scale = (float)openGLContext.getRenderingScale();

		// Compile shader
		if(!m_bWait)
			compileShader();

		// Enable blending and set the viewport size
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glViewport(0, 0, roundToInt(scale * getWidth()), roundToInt(scale * getHeight()));

		// Use our shader and render to the canvas
		m_pShader->use();
		m_pMainCanvas->render();

		// Don't forget to update our global timer
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

		// Get GLSL version
		double version = OpenGLShaderProgram::getLanguageVersion() * 100;
		m_strShader = "#version " + String(version) + "\n" + g_pGlFragShaderUniforms + m_strShader;

		// C O M P I L E
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
			// If failed, we throw the error message
			m_strLastError = newShader->getLastError();

			// Don't use the shader!
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
