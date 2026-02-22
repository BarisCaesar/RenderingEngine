#include "DynamicConstant.h"
#include "LayoutCodex.h"
#include <cstring>

namespace dx = DirectX;

void TestDynamicConstant()
{
	using namespace std::string_literals;
	// data control tests
	{
		DynamicConstBuf::RawLayout s;

		s.Add<DynamicConstBuf::Struct>("structboi"s);
		s["structboi"].Add<DynamicConstBuf::Float3>("float3boi"s);
		s["structboi"].Add<DynamicConstBuf::Float>("floatboi"s);
		s.Add<DynamicConstBuf::Float>("woot"s);
		s.Add<DynamicConstBuf::Array>("arr"s);
		s["arr"s].Set<DynamicConstBuf::Struct>(4);
		s["arr"s].T().Add<DynamicConstBuf::Float3>("twerk"s);
		s["arr"s].T().Add<DynamicConstBuf::Array>("werk"s);
		s["arr"s].T()["werk"s].Set<DynamicConstBuf::Float>(6);
		s["arr"s].T().Add<DynamicConstBuf::Array>("meta"s);
		s["arr"s].T()["meta"s].Set<DynamicConstBuf::Array>(6);
		s["arr"s].T()["meta"s].T().Set<DynamicConstBuf::Matrix>(4);
		s["arr"s].T().Add<DynamicConstBuf::Bool>("booler"s);

		// fails: duplicate symbol name
		//s.Add<DynamicConstBuf::Bool>( "arr"s );

		// fails: bad symbol name
		//s.Add<DynamicConstBuf::Bool>( "120man" );

		DynamicConstBuf::Buffer b = DynamicConstBuf::Buffer(std::move(s));

		const auto sig = b.GetRootLayoutElement().GetSignature();
		{
			auto exp = 42.0f;
			b["woot"s] = exp;
			float act = b["woot"s];
			assert(act == exp);
		}
		{
			auto exp = 420.0f;
			b["structboi"s]["floatboi"s] = exp;
			float act = b["structboi"s]["floatboi"s];
			assert(act == exp);
		}
		{
			auto exp = 111.0f;
			b["arr"s][2]["werk"s][5] = exp;
			float act = b["arr"s][2]["werk"s][5];
			assert(act == exp);
		}
		{
			auto exp = DirectX::XMFLOAT3{ 69.0f,0.0f,0.0f };
			b["structboi"s]["float3boi"s] = exp;
			dx::XMFLOAT3 act = b["structboi"s]["float3boi"s];
			assert(!std::memcmp(&exp, &act, sizeof(DirectX::XMFLOAT3)));
		}
		{
			DirectX::XMFLOAT4X4 exp;
			dx::XMStoreFloat4x4(
				&exp,
				dx::XMMatrixIdentity()
			);
			b["arr"s][2]["meta"s][5][3] = exp;
			dx::XMFLOAT4X4 act = b["arr"s][2]["meta"s][5][3];
			assert(!std::memcmp(&exp, &act, sizeof(DirectX::XMFLOAT4X4)));
		}
		{
			auto exp = true;
			b["arr"s][2]["booler"s] = exp;
			bool act = b["arr"s][2]["booler"s];
			assert(act == exp);
		}
		{
			auto exp = false;
			b["arr"s][2]["booler"s] = exp;
			bool act = b["arr"s][2]["booler"s];
			assert(act == exp);
		}
		// exists
		{
			assert(b["structboi"s]["float3boi"s].Exists());
			assert(!b["structboi"s]["fubar"s].Exists());
			if (auto ref = b["structboi"s]["float3boi"s]; ref.Exists())
			{
				dx::XMFLOAT3 f = ref;
				assert(f.x == 69.0f);
			}
		}
		const auto& cb = b;
		{
			dx::XMFLOAT4X4 act = cb["arr"s][2]["meta"s][5][3];
			assert(act._11 == 1.0f);
		}
		// this doesn't compile: buffer is const
		// cb["arr"][2]["booler"] = true;

		// this fails assertion: array out of bounds
		// cb["arr"s][200];
	}
	// size test array of arrays
	{
		DynamicConstBuf::RawLayout s;
		s.Add<DynamicConstBuf::Array>("arr");
		s["arr"].Set<DynamicConstBuf::Array>(6);
		s["arr"].T().Set<DynamicConstBuf::Matrix>(4);
		DynamicConstBuf::Buffer b = DynamicConstBuf::Buffer(std::move(s));

		auto act = b.GetSizeInBytes();
		assert(act == 16u * 4u * 4u * 6u);
	}
	// size test array of structs with padding
	{
		DynamicConstBuf::RawLayout s;
		s.Add<DynamicConstBuf::Array>("arr");
		s["arr"].Set<DynamicConstBuf::Struct>(6);
		s["arr"s].T().Add<DynamicConstBuf::Float2>("a");
		s["arr"].T().Add<DynamicConstBuf::Float3>("b"s);
		DynamicConstBuf::Buffer b = DynamicConstBuf::Buffer(std::move(s));

		auto act = b.GetSizeInBytes();
		assert(act == 16u * 2u * 6u);
	}
	// size test array of primitive that needs padding
	{
		DynamicConstBuf::RawLayout s;
		s.Add<DynamicConstBuf::Array>("arr");
		s["arr"].Set<DynamicConstBuf::Float3>(6);
		DynamicConstBuf::Buffer b = DynamicConstBuf::Buffer(std::move(s));

		auto act = b.GetSizeInBytes();
		assert(act == 16u * 6u);
	}
	// testing CookedLayout
	{
		DynamicConstBuf::RawLayout s;
		s.Add<DynamicConstBuf::Array>("arr");
		s["arr"].Set<DynamicConstBuf::Float3>(6);
		auto cooked = DynamicConstBuf::LayoutCodex::Resolve(std::move(s));
		// raw is cleared after donating
		s.Add<DynamicConstBuf::Float>("arr");
		// fails to compile, cooked returns const&
		// cooked["arr"].Add<Dcb::Float>("buttman");
		auto b1 = DynamicConstBuf::Buffer(cooked);
		b1["arr"][0] = dx::XMFLOAT3{ 69.0f,0.0f,0.0f };
		auto b2 = DynamicConstBuf::Buffer(cooked);
		b2["arr"][0] = dx::XMFLOAT3{ 420.0f,0.0f,0.0f };
		assert(static_cast<dx::XMFLOAT3>(b1["arr"][0]).x == 69.0f);
		assert(static_cast<dx::XMFLOAT3>(b2["arr"][0]).x == 420.0f);
	}
}