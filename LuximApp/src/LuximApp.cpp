#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/UI/UI.h"
#include "Walnut/ImGui/ImGuiTheme.h"

#include "LuximEditor.cpp"

#define FONTSCALE '1'
#define TEXTCOLOUR 't'
#define BORDERCOLOUR 'b'
#define BACKGROUNDCOLOUR 'g'

class ExampleLayer : public Walnut::Layer
{
public: // Public Functions

	void OnAttach() override {
		LoadFavourites();
		LoadPrefs();
	}

	void OnDetach() override {
		if (m_FileOpen)
			m_LuximEditor.SaveFile();
	}

	virtual void OnUIRender() override {

		if (m_FileOpen) {
			m_LuximEditor.RenderEditor();
		}
		else {
			UI_DrawIntro();
		}

		UI_DrawSidebar();

		UI_DrawAboutModal();

		UI_DrawPreferencesModal();
	}

	void OpenFile() {
		if (!AddFavorite(tinyfd_openFileDialog("Open File", "", 0, 0, 0, 0)))
			return;

		std::string filePath = m_FavoritePaths.at(m_FavoritePaths.size() - 1);

		if (filePath == "") { return; }

		m_FileOpen = true;

		m_LuximEditor.LoadFile(filePath);
	}

	void CreateNewFile() {
		m_LuximEditor.NewFile();
		m_FileOpen = true;
	}

	void ShowAboutModal()
	{
		m_AboutModalOpen = true;
	}

	void ShowPreferencesModal()
	{
		m_PreferencesOpen = true;
	}

	void SaveFile() {
		if(m_FileOpen)
			m_LuximEditor.SaveFile();
		else
			tinyfd_messageBox("Error", "No file is currently open.", "ok", "error", 1);
	}

	void SaveFileAs() {
		if (m_FileOpen) {
			m_LuximEditor.SaveFileAs();
		}
		else
			tinyfd_messageBox("Error", "No file is currently open.", "ok", "error", 1);
	}

private: // Private Functions

	void UI_DrawIntro() {

		ImGui::Begin("Welcome to Luxim");

		ImGui::Text("Luxim is a text editor created for ease of use.");
		ImGui::Text("Add a document to the sidebar or simply open one from the top menu to get started!");

		ImGui::End();
	}

	void UI_DrawSidebar() {

		ImGui::Begin("Sidebar");

		if (ImGui::BeginMenu("Favourites"))
		{
			if (ImGui::MenuItem("Add Favourite")) {

				if (!AddFavorite(tinyfd_openFileDialog("Open File", "", NULL, NULL, NULL, 0))) {
					ImGui::EndMenu();
					ImGui::End();
					return;
				}
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();
		
		for (int i = 0; i < m_FavoritePaths.size(); i++) {

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Right-click to Delete");
			}

			if (ImGui::Selectable(m_LuximEditor.FilePathToFileName(m_FavoritePaths[i]).c_str()))
			{
				m_FileOpen = true;

				m_LuximEditor.LoadFile(m_FavoritePaths[i]);
			}

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::Selectable("Delete")) {

					m_FavoritePaths.erase(m_FavoritePaths.begin() + i);

					SaveFavourites();

					if(m_FavoritePaths.size() > 0)
						m_LuximEditor.LoadFile(m_FavoritePaths.back());

					ImGui::CloseCurrentPopup();
					ImGui::EndPopup();
					ImGui::End();

					return;
				}

				if (ImGui::Selectable("Move Down")) {

					std::string temp = "";

					if (i == m_FavoritePaths.size() - 1) { ImGui::End(); return; }

					temp = m_FavoritePaths[i + 1];
					m_FavoritePaths[i + 1] = m_FavoritePaths[i];
					m_FavoritePaths[i] = temp;

					ImGui::CloseCurrentPopup();
					ImGui::EndPopup();
					ImGui::End();

					return;
				}

				if (ImGui::Selectable("Move Up")) {
					std::string temp = "";

					if (i == 0) { ImGui::End();  return; }

					temp = m_FavoritePaths[i - 1];
					m_FavoritePaths[i - 1] = m_FavoritePaths[i];
					m_FavoritePaths[i] = temp;

					ImGui::CloseCurrentPopup();
					ImGui::EndPopup();
					ImGui::End();

					return;
				}

				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}

	void UI_DrawAboutModal() {
		if (!m_AboutModalOpen)
			return;
		
		const ImVec2 windowSize{ 280, 185 };
		ImGui::SetCursorPos({ ImGui::GetContentRegionAvail().x / 2 - windowSize.x / 2,
							  ImGui::GetContentRegionAvail().y / 2 - windowSize.y / 2 });

		ImGui::SetNextWindowSize(windowSize);
		ImGui::OpenPopup("About");
		m_AboutModalOpen = ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_NoMove + ImGuiWindowFlags_NoResize);
		if (m_AboutModalOpen)
		{
			auto image = Walnut::Application::Get().GetApplicationIcon();
			ImGui::Image(image->GetDescriptorSet(), { 48, 48 });

			ImGui::SameLine();
			Walnut::UI::ShiftCursorX(20.0f);

			ImGui::BeginGroup();
			ImGui::Text("Luxim - The Text Editor");
			ImGui::Text("by Zentifyyy.");
			ImGui::EndGroup();

			Walnut::UI::ShiftCursorY(10.0f);
			ImGui::Separator();

			ImGui::SetWindowFontScale(0.75f);
			ImGui::Text("Made With the Walnut Application Framework");
			ImGui::SetWindowFontScale(1.0f);

			Walnut::UI::ShiftCursorY(10.0f);

			if (Walnut::UI::ButtonCentered("Close"))
			{
				m_AboutModalOpen = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	bool AddFavorite(const char* filePath) {
		if (filePath == nullptr)
			return false;

		for (int i = 0; i < m_FavoritePaths.size(); i++) {
			if (filePath == m_FavoritePaths[i])
				return false;
		}

		m_FavoritePaths.push_back(filePath);

		SaveFavourites();

		return true;
	}

	bool LoadFavourites() {

		m_FileInput.open("favorites.ini");

		if (!m_FileInput.is_open()) { return false; }

		std::string text = "";

		while (std::getline(m_FileInput, text)) {
			if (std::filesystem::exists(text)) {
				m_FavoritePaths.emplace_back(text);
			}
			else
			{
				tinyfd_messageBox("File Not Found", ("The file: " + text + " could not be found. It will be removed from your favorites.").c_str(),"ok", "error", 1);
				SaveFavourites();
			}
		}

		m_FileInput.close();

		return true;
	}

	void SaveFavourites() {

		m_FileOutput.open("favorites.ini");

		if (!m_FileOutput.is_open()) { return; }

		for (int i = 0; i < m_FavoritePaths.size(); i++) {
			m_FileOutput << m_FavoritePaths[i] << "\n";
		}

		m_FileOutput.close();
	}

	bool IsValidPath(std::string path, std::string& out) {
		if (path.empty())
			return false;

		out = path;
		return true;
	}

	void UI_DrawPreferencesModal() {
		if (!m_PreferencesOpen)
			return;

		const ImVec2 windowSize{ 500, 292 };
		ImGui::SetCursorPos({ ImGui::GetContentRegionAvail().x / 2 - windowSize.x / 2,
							  ImGui::GetContentRegionAvail().y / 2 - windowSize.y / 2 });

		ImGui::SetNextWindowSize(windowSize);

		ImGui::OpenPopup("Preferences");
		m_PreferencesOpen = ImGui::BeginPopupModal("Preferences", nullptr, ImGuiWindowFlags_NoMove + ImGuiWindowFlags_NoResize);
		if (m_PreferencesOpen)
		{
			ImGui::SliderFloat("Editor Font Scale", &Pref_EditorFontScale , 0.5f, 2.0f,"%.2f",ImGuiSliderFlags_AlwaysClamp);

			ImGui::Separator();

			if(ImGui::ColorEdit3("Text Colour", (float*)&style.Colors[ImGuiCol_Text]))
				 Pref_TextColour = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]);

			if (ImGui::ColorEdit3("Border Colour", (float*)&style.Colors[ImGuiCol_Border]))
				Pref_BorderColour = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Border]);

			if (ImGui::ColorEdit3("Background Colour", (float*)&style.Colors[ImGuiCol_WindowBg])) {
				Pref_BackgroundColour = ImGui::GetColorU32(style.Colors[ImGuiCol_WindowBg]);

				ImVec4 bgCol = style.Colors[ImGuiCol_WindowBg];
				style.Colors[ImGuiCol_Tab] = bgCol;
				style.Colors[ImGuiCol_TabActive] = bgCol;
				style.Colors[ImGuiCol_TabUnfocused] = bgCol;
				style.Colors[ImGuiCol_TabUnfocusedActive] = bgCol;
				style.Colors[ImGuiCol_TabHovered] = bgCol;
				style.Colors[ImGuiCol_TitleBg] = bgCol;
				style.Colors[ImGuiCol_TitleBgActive] = bgCol;
			}

			if (ImGui::Button("Revert")) {
				Pref_BackgroundColour = ImGui::GetColorU32(style.Colors[ImGuiCol_WindowBg]);

				style.Colors[ImGuiCol_Text] = ImGui::ColorConvertU32ToFloat4(Walnut::UI::Colors::Theme::text);
				style.Colors[ImGuiCol_Border] = ImGui::ColorConvertU32ToFloat4(Walnut::UI::Colors::Theme::backgroundDark);

				ImVec4 bgCol = ImGui::ColorConvertU32ToFloat4(Walnut::UI::Colors::Theme::titlebar);
				style.Colors[ImGuiCol_WindowBg] = bgCol;
				style.Colors[ImGuiCol_Tab] = bgCol;
				style.Colors[ImGuiCol_TabActive] = bgCol;
				style.Colors[ImGuiCol_TabUnfocused] = bgCol;
				style.Colors[ImGuiCol_TabUnfocusedActive] = bgCol;
				style.Colors[ImGuiCol_TabHovered] = bgCol;
				style.Colors[ImGuiCol_TitleBg] = bgCol;
				style.Colors[ImGuiCol_TitleBgActive] = bgCol;
			}

			ImGui::Separator();
			
			Walnut::UI::ShiftCursorY(5);
			if (Walnut::UI::ButtonCentered("Save Preferences"))
			{
				m_LuximEditor.LoadPreferences(Pref_EditorFontScale);
				SavePrefs();
				m_PreferencesOpen = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void LoadPrefs() 
	{
		m_FileInput.open("prefs.ini");

		if (!m_FileInput.is_open()) { return; }

		std::string text = "";

		while (std::getline(m_FileInput, text))
		{
			ImGuiIO io = ImGui::GetIO();

			switch (text[0])
			{
			case FONTSCALE:
				text.erase(0, 1);
				Pref_EditorFontScale = std::stof(text);
				break;
			case TEXTCOLOUR:
				text.erase(0, 1);
				Pref_TextColour = std::stoul(text);
				SetCustomColourTheme();
				break;
			case BORDERCOLOUR:
				text.erase(0, 1);
				Pref_BorderColour = std::stoul(text);
				SetCustomColourTheme();
				break;
			case BACKGROUNDCOLOUR:
				text.erase(0, 1);
				Pref_BackgroundColour = std::stoul(text);
				SetCustomColourTheme();
				break;
			default:
				break;
			}
		}

		m_FileInput.close();
	}

	void SavePrefs() 
	{
		m_FileOutput.open("prefs.ini");

		if (!m_FileOutput.is_open()) { return; }

		m_FileOutput << FONTSCALE << std::to_string(Pref_EditorFontScale) << "\n";

		m_FileOutput << TEXTCOLOUR << Pref_TextColour << "\n";
		m_FileOutput << BORDERCOLOUR << Pref_BorderColour << "\n";
		m_FileOutput << BACKGROUNDCOLOUR << Pref_BackgroundColour << "\n";

		m_FileOutput.close();
	}

	void SetCustomColourTheme() {

		style.Colors[ImGuiCol_Text] = ImGui::ColorConvertU32ToFloat4(Pref_TextColour);

		style.Colors[ImGuiCol_Border] = ImGui::ColorConvertU32ToFloat4(Pref_BorderColour);

		style.Colors[ImGuiCol_WindowBg] = ImGui::ColorConvertU32ToFloat4(Pref_BackgroundColour);
		ImVec4 bgCol = style.Colors[ImGuiCol_WindowBg];
		style.Colors[ImGuiCol_Tab] = bgCol;
		style.Colors[ImGuiCol_TabActive] = bgCol;
		style.Colors[ImGuiCol_TabUnfocused] = bgCol;
		style.Colors[ImGuiCol_TabUnfocusedActive] = bgCol;
		style.Colors[ImGuiCol_TabHovered] = bgCol;
		style.Colors[ImGuiCol_TitleBg] = bgCol;
		style.Colors[ImGuiCol_TitleBgActive] = bgCol;
	}

private: // Private Variables

	Editor m_LuximEditor;

	bool m_AboutModalOpen = false;
	bool m_FileOpen = false;
	bool m_PreferencesOpen = false;

	std::vector<std::string> m_FavoritePaths{};

	float m_FavoriteButtonHeight = 40.0f;

	std::ifstream m_FileInput;
	std::ofstream m_FileOutput;

	ImGuiStyle& style = ImGui::GetStyle();

private: // Preferences

	float Pref_EditorFontScale = 1.0f;

	ImU32 Pref_TextColour = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]);
	ImU32 Pref_BorderColour = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Border]);
	ImU32 Pref_BackgroundColour = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_WindowBg]);

};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Luxim";
	spec.CustomTitlebar = true;
	spec.CenterWindow = true;
	
	spec.TitlebarButtonColour = Walnut::UI::Colors::Theme::text;
	spec.TitlebarButtonHoveredColour = ImColor{ 0, 255, 220 ,140};
	spec.TitlebarButtonPressedColour = ImColor{ 0, 255, 220 ,50 };

	spec.TitlebarHeight = 48.0f;
	spec.TitlebarPaddingY = 10.0f;

	spec.LogoSize = { 34.0f , 34.0f };
	
	spec.IconPath = "img/AppIcon.png";

	spec.UseLogging = false;

	Walnut::Application* app = new Walnut::Application(spec);
	std::shared_ptr<ExampleLayer> exampleLayer = std::make_shared<ExampleLayer>();
	
	//Styling
	ImGuiStyle& style = ImGui::GetStyle();

	ImVec4 titlebarCol = ImGui::ColorConvertU32ToFloat4(Walnut::UI::Colors::Theme::titlebar);
	style.Colors[ImGuiCol_TabActive] = titlebarCol; 
	style.Colors[ImGuiCol_TabHovered] = titlebarCol;
	style.Colors[ImGuiCol_Tab] = titlebarCol;
	style.Colors[ImGuiCol_TabUnfocusedActive] = titlebarCol;

	style.Colors[ImGuiCol_SeparatorHovered] = { 0, 255, 220 , 0.4f };
	style.Colors[ImGuiCol_SeparatorActive] = { 0, 255, 220 , 0.3f };

	app->SetApplicationIcon("img/AppIcon.png");

	app->PushLayer(exampleLayer);
	app->SetMenubarCallback([app, exampleLayer]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open File"))
			{
				exampleLayer->OpenFile();
			}

			if (ImGui::MenuItem("New File"))
			{
				exampleLayer->CreateNewFile();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Save File"))
			{
				exampleLayer->SaveFile();
			}

			if (ImGui::MenuItem("Save File As"))
			{
				exampleLayer->SaveFileAs();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("About"))
			{
				exampleLayer->ShowAboutModal();
			}

			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Settings"))
		{
			if (ImGui::MenuItem("Preferences"))
			{
				exampleLayer->ShowPreferencesModal();
			}

			ImGui::EndMenu();
		}
	});
	return app;
}