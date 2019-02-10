/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "SBTimer.h"

extern const char* g_pGlFragmentShader =
"vec2 rot(vec2 v, float ang)\n"
"{\n"
"	float x = v.x * cos(ang) - v.y * sin(ang);\n"
"	float y = v.x * sin(ang) + v.y * cos(ang);\n"
"	return vec2(x, y);\n"
"}\n"
"\n"
"void mainShader(out vec4 fragColor)\n"
"{\n"
"	vec2 uv = fUvCoord;\n"
"	uv -= 0.5;\n"
"	uv.x *= fResolution.x / fResolution.y;\n"
//"	uv *= 2.0;\n"
"	uv = rot(uv, fTime);\n"
"	vec2 pos = uv - vec2(0.5,0.5);\n"
"	float sz = 0.2;\n"
"	float col;\n"
"	pos.x += cos(atan(sin(uv.x),sin(uv.y)) * 2. + fTime);\n"
"	pos.y += sin(atan(cos(pos.x),sin(uv.y)) * 2. + fTime);\n"
"	pos.x += cos(atan(sin(uv.x),sin(uv.y)) * 8. + fTime);\n"
"	pos.y += sin(atan(cos(pos.x),sin(uv.y)) * 8. + fTime);\n"
"	pos.x += pow(cos(sin(pos.x) + fTime), 4.0);\n"
"	pos.y += pow(sin(cos(pos.y) + fTime), 4.0);\n"
"	col = smoothstep(sz, sz - 0.1, length(pos) * (0.5 * (cos(atan(pos.y, pos.x) * 10.) * sin(pos.y + cos(fTime + atan(pos.y,pos.x)) * 5.0))));\n"
"	fragColor = vec4(vec3(col * abs(cos(atan(pos.y,uv.x) * 2. + fTime)),\n"
"							col * abs(sin(pos.x * 2. + fTime)),\n"
"							col * abs(cos(atan(uv.y,pos.x) * 10. + fTime)) ), 1.0);\n"
"}\n"
"\n";

//==============================================================================
MainComponent::MainComponent()
{
	String mainShader = g_pGlFragmentShader;
	setOpaque(true);

	m_pCodeEditor.reset(new SBCodeEditor());
	m_pShaderView.reset(new SBShaderView(mainShader, m_pCodeEditor.get()));
	m_pCodeEditor->setShaderViewer(m_pShaderView->getShaderViewer());

	addAndMakeVisible(*m_pShaderView);
	addAndMakeVisible(m_PanelResizer);
	addAndMakeVisible(*m_pCodeEditor);

	m_PanelManager.setItemLayout(0, -0.1, -0.9, -0.5);
	m_PanelManager.setItemLayout(1, 2, 2, 2);
	m_PanelManager.setItemLayout(2, -0.1, -0.9, -0.5);

	SBTimer::g_pTimerInstance = new SBTimer();

	setSize(1280, 720);
}

MainComponent::~MainComponent()
{
	if(SBTimer::g_pTimerInstance)
	{
		delete SBTimer::g_pTimerInstance;
		SBTimer::g_pTimerInstance = nullptr;
	}
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    //g.setFont (Font (16.0f));
    //g.setColour (Colours::white);
    //g.drawText ("Hello World!", getLocalBounds(), Justification::centred, true);
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
	//m_pView->setTopLeftPosition(Point<int>(getWidth() - m_pView->getWidth(), getBounds().getY()));
	//m_pCodeEditor->setBounds(getBounds());
	Rectangle<int> localBounds = getLocalBounds();
	Component* allComponent[] = { m_pShaderView.get(), &m_PanelResizer, m_pCodeEditor.get() };

	m_PanelManager.layOutComponents(allComponent, 3, localBounds.getX(), localBounds.getY(), localBounds.getWidth(), localBounds.getHeight(), false, true);
}
