#pragma once
#include "DynamicConstant.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <optional>

namespace DynamicConstBuf
{
	class LayoutCodex
	{
	public:
		static Layout Resolve(Layout& layout) noxnd;
	private:
		static LayoutCodex& Get_() noexcept;
	private:
		std::unordered_map < std::string, std::shared_ptr<DynamicConstBuf::LayoutElement>> map;
	};
}
