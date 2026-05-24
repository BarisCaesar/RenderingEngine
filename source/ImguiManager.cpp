#include "ImguiManager.h"
#include "imgui/imgui.h"
#include <filesystem>
#include "RUtil.h"

ImguiManager::ImguiManager()
{
	namespace fs = std::filesystem;
	auto defaultConfigPath = FindFileInProject("imgui_default.ini");
	if (!fs::exists("imgui.ini") && fs::exists(defaultConfigPath))
	{
		fs::copy_file(defaultConfigPath, "imgui.ini");
	}
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
}

ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();
}

