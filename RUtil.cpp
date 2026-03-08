#include "RUtil.h"
#include <sstream>
#include <iomanip>

std::vector<std::string> TokenizeQuoted(const std::string& input)
{
	std::istringstream stream;
	stream.str(input);
	std::vector<std::string> tokens;
	std::string token;

	while (stream >> std::quoted(token))
	{
		tokens.push_back(std::move(token));
	}
	return tokens;
}

std::wstring ToWide(const std::string& narrow)
{
	wchar_t wide[512];
	mbstowcs_s(nullptr, wide, narrow.c_str(), _TRUNCATE);
	return wide;
}

std::string ToNarrow(const std::wstring& wide)
{
	char narrow[512];
	wcstombs_s(nullptr, narrow, wide.c_str(), _TRUNCATE);
	return narrow;
}

std::filesystem::path FindFileInProject(const std::string& relativePath)
{
	namespace fs = std::filesystem;

	fs::path dir = fs::current_path();

	while (!dir.empty())
	{
		fs::path candidate = dir / relativePath;

		if (fs::exists(candidate))
			return candidate;

		dir = dir.parent_path();
	}

	throw std::runtime_error("File not found in project tree: " + relativePath);
}

