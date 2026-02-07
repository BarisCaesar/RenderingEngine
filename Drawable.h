#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"
#include <memory>

namespace Bind
{
	class Bindable;
	class IndexBuffer;
}

class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noxnd;
	virtual ~Drawable() = default;
protected:
	template<class T>
	T* QueryBindable()
	{
		for (auto& pbindable : binds)
		{
			if (auto pRawBindable = dynamic_cast<T*>(pbindable.get()))
			{
				return pRawBindable;
			}
		}
		return nullptr;
	}
	void AddBind(std::shared_ptr<Bind::Bindable> bind) noxnd;

private:
	const class Bind::IndexBuffer* pIndexBuffer = nullptr;
	std::vector < std::shared_ptr < Bind:: Bindable >> binds;
};