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

		m_LuximEditor.RenderEditor();
		
		ImGui::Begin("Sidebar");
		
		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S)) {
			m_LuximEditor.SaveFile();
		}

		ImGui::End();

		//ImGui::ShowDemoWindow();

		UI_DrawAboutModal();
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

		if (!m_LuximEditor.LoadFile(filePath)) { return; }

		std::string fileName = "";

		for (int i = filePath.length(); i > 0; i--) {
			if (filePath.at(i - 1) != '\\') {
				fileName.insert(fileName.begin(), filePath.at(i - 1));
			}
			else {
				m_LuximEditor.UpdateTitle(fileName);
				return;
			}
		}
	}

private:

	Editor m_LuximEditor;
	bool m_AboutModalOpen = false;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Luxim";
	spec.CustomTitlebar = true;

	
	spec.TitlebarButtonColour = Walnut::UI::Colors::Theme::text;
	spec.TitlebarButtonHoveredColour = ImColor{ 0, 255, 220 ,140};
	spec.TitlebarButtonPressedColour = ImColor{ 0, 255, 220 ,50 };


	Walnut::Application* app = new Walnut::Application(spec);
	std::shared_ptr<ExampleLayer> exampleLayer = std::make_shared<ExampleLayer>();
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