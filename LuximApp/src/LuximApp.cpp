#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/UI/UI.h"
#include "Walnut/ImGui/ImGuiTheme.h"

#include "../src/LuximEditor.cpp"

#include "tinyfiledialogs.h"

class ExampleLayer : public Walnut::Layer
{
public: // Public Functions

	void OnAttach() override {
		LoadFile();
	}

	virtual void OnUIRender() override{

		if (m_FileOpen) {
			m_LuximEditor.RenderEditor();
		}
		else {
			UI_DrawIntro();
		}

		UI_DrawSidebar();

		UI_DrawAboutModal();
	}

	void OpenFile() {
		if (!AddFavorite(tinyfd_openFileDialog("Open File", "", 0, 0, 0, 0)))
			return;

		std::string filePath = m_FavoritePaths.at(m_FavoritePaths.size() - 1);

		if (filePath == "") { return; }

		m_FileOpen = true;

		if (!m_LuximEditor.LoadFile(filePath)) { return; }
		
		m_LuximEditor.UpdateTitle(FilePathToFileName(filePath));
	}

	void CreateNewFile() {
		m_LuximEditor.UpdateTitle("New Document");
		m_FileOpen = true;
	}

	void ShowAboutModal()
	{
		m_AboutModalOpen = true;
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

		if (ImGui::BeginMenu("File"))
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

			if (ImGui::Selectable(FilePathToFileName(m_FavoritePaths[i]).c_str()))
			{
				m_FileOpen = true;

				if (!m_LuximEditor.LoadFile(m_FavoritePaths[i])) { return; }

				m_LuximEditor.UpdateTitle(FilePathToFileName(m_FavoritePaths[i]));;
			}

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::Selectable("Delete")) {

					m_FavoritePaths.erase(m_FavoritePaths.begin() + i);

					SaveFile();

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

					return;;
				}

				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}

	void UI_DrawAboutModal() {
		if (!m_AboutModalOpen)
			return;

		ImGui::OpenPopup("About");
		m_AboutModalOpen = ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		if (m_AboutModalOpen)
		{
			auto image = Walnut::Application::Get().GetApplicationIcon();
			ImGui::Image(image->GetDescriptorSet(), { 48, 48 });

			ImGui::SameLine();
			Walnut::UI::ShiftCursorX(20.0f);

			ImGui::BeginGroup();
			ImGui::Text("Walnut application framework");
			ImGui::Text("by Studio Cherno.");
			ImGui::EndGroup();

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

		m_FavoritePaths.push_back(filePath);

		SaveFile();

		return true;
	}

	std::string FilePathToFileName(std::string& filePath) {

		std::string fileName = "";

		for (int i = filePath.length(); i > 0; i--) {
			if (filePath[i] != '\\') {
				fileName.insert(fileName.begin(), filePath[i]);
			}
			else {
				return fileName;
			}
		}
	}

	bool LoadFile() {

		m_FileInput.open("favorites.ini");

		if (!m_FileInput.is_open()) { return false; }

		std::string text = "";

		while (std::getline(m_FileInput, text)) {
			m_FavoritePaths.emplace_back(text);
		}

		m_FileInput.close();

		return true;
	}

	void SaveFile() {

		m_FileOutput.open("favorites.ini");

		if (!m_FileOutput.is_open()) { return; }

		for (int i = 0; i < m_FavoritePaths.size(); i++) {
			m_FileOutput << m_FavoritePaths[i] << "\n";
		}

		m_FileOutput.close();
	}

private: // Private Variables

	Editor m_LuximEditor;

	bool m_AboutModalOpen = false;
	bool m_FileOpen = false;

	std::vector<std::string> m_FavoritePaths{};

	float m_FavoriteButtonHeight = 40.0f;

	std::ifstream m_FileInput;
	std::ofstream m_FileOutput;
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
	spec.TitlebarPaddingY = 8.0f;

	spec.LogoSize = { 38.0f , 38.0f };
	
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

			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}