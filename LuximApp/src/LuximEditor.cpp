#include "imgui.h"

#include "iostream"
#include "fstream"
#include <filesystem>

#include "tinyfiledialogs.h"

class Editor {
public: // Public Functions

	void RenderEditor() {

		ImGui::Begin(m_WindowTitle.c_str(), 0, m_WindowFlags);

		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S)) {
			SaveFile();
		}

		if (ImGui::InputTextMultiline("##source", m_EditorBuffer, IM_ARRAYSIZE(m_EditorBuffer), ImGui::GetContentRegionAvail(), m_TextInputFlags)) {
			m_WindowFlags = ImGuiWindowFlags_UnsavedDocument;
		}

		ImGui::End();
	}

	void UpdateTitle(std::string newTitle) {
		m_WindowTitle = newTitle;
	}

	bool LoadFile(std::string& filePath) {

		ResetText();
		
		m_CurrentFilePath = filePath;

		m_FileInput.open(filePath);

		if (!m_FileInput.is_open()) { return false; }

		std::string text = "";

		while (std::getline(m_FileInput, text)) {
			text += "\n";
			AppendToText(text);
		}

		m_FileInput.close();

		return true;
	}

private: // Private Functions

	void SaveFile() {

		if (m_CurrentFilePath == "") {
			if (!SetCurrentFilePath(tinyfd_saveFileDialog("Save File", NULL, NULL, NULL, NULL)))
				return;
		}

		m_FileOutput.open(m_CurrentFilePath);

		if (!m_FileOutput.is_open()) { return; }

		m_FileOutput << m_EditorBuffer;

		m_WindowFlags = ImGuiWindowFlags_None;

		m_FileOutput.close();
	}

	bool SetCurrentFilePath(const char* filePath) {
		if (filePath == nullptr)
			return false;

		m_CurrentFilePath = filePath;

		return true;
	}
	
	void AppendToText(std::string& newText) {
		strcat_s(m_EditorBuffer, newText.c_str());
	}

	void ResetText() {
		strcpy_s(m_EditorBuffer,"");
	}

private: // Private Variables

	char m_EditorBuffer[1024 * 64] = "";

	std::string m_WindowTitle = "Luxim Editor";

	std::string m_CurrentFilePath = "";

	bool m_DocumentSaved = true;

	int m_WindowFlags = ImGuiWindowFlags_None;
	int m_TextInputFlags = ImGuiInputTextFlags_AllowTabInput;

	std::ifstream m_FileInput;
	std::ofstream m_FileOutput;

};