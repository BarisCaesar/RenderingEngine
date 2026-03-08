#pragma once
#include <vector>
#include <string>
#include <filesystem>

std::vector<std::string> TokenizeQuoted(const std::string& input);

std::wstring ToWide(const std::string& narrow);

std::string ToNarrow(const std::wstring& wide);

std::filesystem::path FindFileInProject(const std::string& relativePath);