#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include "fstream"

#include "tinyfiledialogs.h"

class Editor {
public: // Public Functions

	void RenderEditor() {

		ImGui::SetNextWindowDockID(2,0);
		ImGui::Begin(m_WindowTitle.c_str(), 0, m_WindowFlags);

		if(ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyDown(ImGuiKey_LeftShift) && ImGui::IsKeyPressed(ImGuiKey_S)) {
			SaveFileAs();
		}
		else if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S)) {
			SaveFile();
		}

		ImGui::SetWindowFontScale(m_EditorTextScale);

		if (ImGui::InputTextMultiline("##source", &m_EditorBufferStr, ImGui::GetContentRegionAvail(), m_TextInputFlags)) {
			m_WindowFlags = ImGuiWindowFlags_UnsavedDocument;
		}

		ImGui::SetWindowFontScale(1.0f);

		ImGui::End();
	}

	bool LoadFile(std::string& filePath) {

		if (m_WindowFlags == ImGuiWindowFlags_UnsavedDocument) { SaveFile(); }

		ResetText();
		
		m_CurrentFilePath = filePath;
		UpdateTitle(FilePathToFileName(filePath));

		m_FileInput.open(filePath);

		if (!m_FileInput.is_open()) {
			tinyfd_messageBox("Error", ("Could not open file: " + FilePathToFileName(filePath)).c_str(), "ok", "error", 1);
			return false;
		}

		std::string text = "";

		while (std::getline(m_FileInput, text)) {
			text += "\n";
			AppendToText(text);
		}

		m_FileInput.close();

		return true;
	}

	void LoadPreferences(float fontScale) {
		m_EditorTextScale = fontScale;
	}

	void NewFile() {
		ResetText();
		UpdateTitle("New Document");
		m_CurrentFilePath = "";
	}

	inline std::string FilePathToFileName(std::string& filePath) {

		std::string fileName = "";

		for (int i = filePath.length(); i > 0; i--) {
			if (filePath[i] != '\\') {
				fileName.insert(fileName.begin(), filePath[i]);
			}
			else return fileName;
		}

		return "";
	}

	void SaveFile() {
		if (m_CurrentFilePath.empty())
			SaveFileAs();

		m_FileOutput.open(m_CurrentFilePath);
		if (!m_FileOutput.is_open()) { return; }

		m_FileOutput << m_EditorBufferStr;

		m_WindowFlags = ImGuiWindowFlags_None;

		m_FileOutput.close();
	}

	void SaveFileAs() {
		if (!SetCurrentFilePath(tinyfd_saveFileDialog("Save File", "", 0, NULL, NULL), true))
			return;
			
		SaveFile();
	}
	
private: // Private Functions

	inline void UpdateTitle(std::string newTitle) {
		m_WindowTitle = newTitle;
	}

	inline void ResetText() {
		m_EditorBufferStr = "";
	}

	bool SetCurrentFilePath(std::string filePath, bool isSaveFileAs) {
		if (filePath.empty())
			return false;

		if (isSaveFileAs)
			std::remove(m_CurrentFilePath.c_str());

		m_CurrentFilePath = filePath;

		UpdateTitle(FilePathToFileName(filePath));

		return true;
	}
	
	inline void AppendToText(std::string& newText) {
		m_EditorBufferStr += newText;
	}

private: // Private Variables

	std::string m_EditorBufferStr = "";

	std::string m_WindowTitle = "Luxim Editor";

	std::string m_CurrentFilePath = "";

	bool m_DocumentSaved = true;

	int m_WindowFlags = ImGuiWindowFlags_None;
	int m_TextInputFlags = ImGuiInputTextFlags_AllowTabInput;

	std::ifstream m_FileInput;
	std::ofstream m_FileOutput;

private: //Preferences

	float m_EditorTextScale = 1.0f;

};