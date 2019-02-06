/*
  ==============================================================================

    SBCodeEditor.cpp
    Created: 16 Jan 2019 12:19:04am
    Author:  USER

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SBCodeEditor.h"
#include "GLView.h"

extern const char* g_pGlFragmentShader;

//==============================================================================

struct SBCodeEditorContainer : public Component, public CodeDocument::Listener
{
public:
	SBCodeEditorContainer()
	{
		m_pSubEditor.reset(new SBSubCodeEditor(m_CodeDoc, &m_Tokeniser));
		m_pSubEditor->setFont(m_Font);
		m_pSubEditor->setScrollbarThickness(10);
		m_CodeDoc.addListener(this);
		addAndMakeVisible(*m_pSubEditor);

		addAndMakeVisible(m_PanelResizer);

		m_pErrorList.reset(new SBSubCodeEditor(m_ErrorDoc, nullptr));
		m_pErrorList->setReadOnly(true);
		m_pErrorList->setScrollbarThickness(10);
		addAndMakeVisible(*m_pErrorList);

		m_PanelManager.setItemLayout(0, -0.3, -0.9, -0.7);
		m_PanelManager.setItemLayout(1, 2, 2, 2);
		m_PanelManager.setItemLayout(2, -0.1, -0.9, -0.15);

		m_bEdited = false;

		resized();
	}

	~SBCodeEditorContainer()
	{
	}

	void paint(Graphics& g) override
	{
	}

	void resized() override
	{
		Rectangle<int> localBounds = getLocalBounds();
		Component* allComponent[] = { m_pSubEditor.get(), &m_PanelResizer, m_pErrorList.get() };

		m_PanelManager.layOutComponents(allComponent, 3, localBounds.getX(), localBounds.getY(), localBounds.getWidth(), localBounds.getHeight(), true, true);
	}

	void codeDocumentTextInserted(const String& newText, int insertIndex) override
	{
		m_bEdited = true;
	}

	void codeDocumentTextDeleted(int startIndex, int endIndex) override
	{
		m_bEdited = true;
	}

	CodeDocument& getDocument()
	{
		return m_CodeDoc;
	}

	CodeDocument& getErrorDocument()
	{
		return m_ErrorDoc;
	}

	bool& isEdited()
	{
		return m_bEdited;
	}

private:
	CodeDocument m_CodeDoc;
	CodeDocument m_ErrorDoc;
	SBGlslTokeniser m_Tokeniser;
	Font m_Font { "Lucida console", 12, Font::plain };
	std::unique_ptr<SBSubCodeEditor> m_pSubEditor;
	std::unique_ptr<CodeEditorComponent> m_pErrorList;
	StretchableLayoutManager m_PanelManager;
	StretchableLayoutResizerBar m_PanelResizer { &m_PanelManager, 1, false };
	bool m_bEdited;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SBCodeEditorContainer)
};

struct SBFileHelper
{
	static void openProjectFile(SBCodeEditor* codeEditor)
	{
		FileChooser browse("Please select project file...",
			File::getSpecialLocation(File::userDocumentsDirectory),
			"*.sbproj");

		if (browse.browseForFileToOpen())
		{
			FileInputStream file(browse.getResult());


			if (file.openedOk())
			{
				XmlDocument projectContent(file.getFile());
				XmlElement* elem = projectContent.getDocumentElement();

				if (elem->hasTagName("ShaderBoiProject"))
				{
					GLView* viewer = codeEditor->getViewer();
					String mainShaderPass;

					{
						MemoryOutputStream decodedCode;
						XmlElement* mainShaderPassCode = elem->getChildByName("MainShaderPassCode");
						Base64::convertFromBase64(decodedCode, mainShaderPassCode->getAllSubText());
						mainShaderPass = decodedCode.toString();
					}

					if (viewer)
					{
						SBCodeEditorContainer* c;
						c = codeEditor->getCodeEditorContainer(0);
						viewer->lock();
						c->getDocument().replaceAllContent(mainShaderPass);
						c->getDocument().clearUndoHistory();
						viewer->setShaderCode(0, mainShaderPass);
						viewer->unlock();
					}
				}
				else
				{
					NativeMessageBox::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error", "Could not open project");
				}
			}
		}
	}

	static void saveProjectFile(SBCodeEditor* codeEditor)
	{
		FileChooser browse("Please select a directory...",
			File::getSpecialLocation(File::userDocumentsDirectory),
			".sbproj");

		if (browse.browseForFileToSave(true))
		{
			File file(browse.getResult());

			if(file.create().ok())
			{
				XmlElement projectContent("ShaderBoiProject");
				String XmlCode;

				{
					XmlElement* projectInfoElement = new XmlElement("ProjectInfo");

					{
						XmlElement* authorElement = new XmlElement("Author");
						authorElement->addTextElement("PLACEHOLDER");
						projectInfoElement->addChildElement(authorElement);
					}

					{
						XmlElement* commentElement = new XmlElement("Comment");
						commentElement->addTextElement("PLACEHOLDER");
						projectInfoElement->addChildElement(commentElement);
					}

					projectContent.addChildElement(projectInfoElement);
				}

				{
					XmlElement* mainShaderPassElement = new XmlElement("MainShaderPassCode");
					String mainShaderPassBase64 = Base64::toBase64(codeEditor->getCodeEditorContainer(0)->getDocument().getAllContent());
					mainShaderPassElement->addTextElement(mainShaderPassBase64);
					projectContent.addChildElement(mainShaderPassElement);
				}

				XmlCode = projectContent.createDocument(String());
				file.replaceWithText(XmlCode);
			}
		}
	}
};

SBCodeEditor::SBCodeEditor()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
	Colour tabColor = findColour(ResizableWindow::backgroundColourId);
	SBCodeEditorContainer* codeEditorContainer;

	m_pCodeTabs.reset(new TabbedComponent(TabbedButtonBar::TabsAtBottom));
	m_pCodeTabs->setTabBarDepth(30);
	m_pCodeTabs->addTab("Main", tabColor, new SBCodeEditorContainer(), true);
	m_pCodeTabs->setCurrentTabIndex(0);
	addAndMakeVisible(*m_pCodeTabs);
	codeEditorContainer = dynamic_cast<SBCodeEditorContainer*>(m_pCodeTabs->getTabContentComponent(0));
	codeEditorContainer->getDocument().replaceAllContent(g_pGlFragmentShader);
	codeEditorContainer->isEdited() = false;

	m_pNewBtn.reset(new TextButton("New..."));
	m_pNewBtn->setSize(80, 25);
	m_pNewBtn->onClick = [this]() {
		int numTabs = m_pCodeTabs->getNumTabs();
		for (int i = 0; i < numTabs; i++)
		{
			SBCodeEditorContainer* codeEditor = dynamic_cast<SBCodeEditorContainer*>(m_pCodeTabs->getTabContentComponent(i));
			if (codeEditor->isEdited())
			{
				int ync = NativeMessageBox::showYesNoCancelBox(AlertWindow::AlertIconType::QuestionIcon, "Message", "Project is not saved.\nDo you want to save current project?");
				switch (ync)
				{
				case 0:
					return;
				case 1:
					// TODO: Create a "Yes" button handler
					SBFileHelper::saveProjectFile(this);
					return;
				case 2:
					// TODO: Create a "No" button handler
					return;
				}
				break;
			}
		}

	};
	addAndMakeVisible(*m_pNewBtn);

	m_pOpenBtn.reset(new TextButton("Open..."));
	m_pOpenBtn->setSize(80, 25);
	m_pOpenBtn->onClick = [this]() {
		SBFileHelper::openProjectFile(this);
	};
	addAndMakeVisible(*m_pOpenBtn);

	m_pSaveBtn.reset(new TextButton("Save..."));
	m_pSaveBtn->setSize(80, 25);
	m_pSaveBtn->onClick = [this]() {
		SBFileHelper::saveProjectFile(this);
	};
	addAndMakeVisible(*m_pSaveBtn);

	m_pCompileBtn.reset(new TextButton("Compile (F5)"));
	m_pCompileBtn->setSize(100, 25);
	m_pCompileBtn->onClick = [this]() {
		SBCodeEditorContainer* codeEditorContainer = dynamic_cast<SBCodeEditorContainer*>(m_pCodeTabs->getTabContentComponent(0));
		String newMainShader = codeEditorContainer->getDocument().getAllContent();

		if (m_pShaderViewer)
		{
			m_pShaderViewer->lock();
			m_pShaderViewer->setShaderCode(0, newMainShader);
			m_pShaderViewer->unlock();
		}
	};
	addAndMakeVisible(*m_pCompileBtn);

	resized();
}

SBCodeEditor::~SBCodeEditor()
{
}

void SBCodeEditor::paint(Graphics & g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void SBCodeEditor::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
	Rectangle<int> sz = getLocalBounds();
	m_pNewBtn->setTopLeftPosition(5, 5);
	m_pOpenBtn->setTopLeftPosition(90, 5);
	m_pSaveBtn->setTopLeftPosition(175, 5);
	m_pCompileBtn->setTopLeftPosition(sz.getWidth() - 105, 5);
	m_pCodeTabs->setBounds(sz.withHeight(sz.getHeight() - 35).withY(35));
}

void SBCodeEditor::setShaderViewer(GLView * view)
{
	m_pShaderViewer = view;
}

CodeDocument& SBCodeEditor::getErrorMessageDocumentFromEditor(int index)
{
	return dynamic_cast<SBCodeEditorContainer*>(m_pCodeTabs->getTabContentComponent(index))->getErrorDocument();
}

SBCodeEditorContainer * SBCodeEditor::getCodeEditorContainer(int index)
{
	return dynamic_cast<SBCodeEditorContainer*>(m_pCodeTabs->getTabContentComponent(index));
}

int SBCodeEditor::getCodeEditorContainerCount()
{
	return m_pCodeTabs->getNumTabs();
}

GLView * SBCodeEditor::getViewer()
{
	return m_pShaderViewer;
}
