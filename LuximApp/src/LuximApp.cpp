#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/UI/UI.h"
#include "Walnut/ImGui/ImGuiTheme.h"

#include "../src/LuximEditor.cpp"

#include "libtinyfiledialogs/include/tinyfiledialogs.h"

class ExampleLayer : public Walnut::Layer
{
public:
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
			if (ImGui::MenuItem("Add Favorite")){
				std::string filePath = tinyfd_openFileDialog("Open File", "*.txt", 0, 0, 0, 0);

				if (filePath == "") { return; }

				m_FavoritePaths.emplace_back(filePath);
			}

			ImGui::EndMenu();
		}

		for (int i = 0; i < m_FavoritePaths.size(); i++) {

			ImGui::Button(m_FavoritePaths[i].c_str(),{ImGui::GetContentRegionAvail().x, m_FavoriteButtonHeight});
		}

		ImGui::End();
	}

	void UI_DrawAboutModal()
	{
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

	void ShowAboutModal()
	{
		m_AboutModalOpen = true;
	}

	void OpenFile() {
		
		std::string filePath = tinyfd_openFileDialog("Open File", "*.txt", 0, 0, 0, 0);

		if (filePath == "") { return; }

		m_FileOpen = true;

		if (!m_LuximEditor.LoadFile(filePath)) { return; }
		
		m_LuximEditor.UpdateTitle(FilePathToFileName(filePath));
	}

private:

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

private:

	Editor m_LuximEditor;

	bool m_AboutModalOpen = false;
	bool m_FileOpen = false;

	std::vector<std::string> m_FavoritePaths{};

	float m_FavoriteButtonHeight = 40.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Luxim";
	spec.CustomTitlebar = true;

	
	spec.TitlebarButtonColour = Walnut::UI::Colors::Theme::text;
	spec.TitlebarButtonHoveredColour = ImColor{ 0, 255, 220 ,140};
	spec.TitlebarButtonPressedColour = ImColor{ 0, 255, 220 ,50 };

	spec.TitlebarHeight = 70.0f;
	spec.TitlebarPaddingY = 20.0f;
	
	spec.IconPath = "img/AppIcon.png";


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

			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}