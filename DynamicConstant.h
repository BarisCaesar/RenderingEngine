#pragma once

#include "ConditionalNoexcept.h"
#include <cassert>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <type_traits>
#include <string>
#include <numeric>
#include <optional>

#define LEAF_ELEMENT_TYPES \
	X(Float) \
	X(Float2) \
	X(Float3) \
	X(Float4) \
	X(Matrix) \
	X(Bool) \

namespace DynamicConstBuf
{
	namespace dx = DirectX;

	enum Type
	{
		#define X(el) el,
		LEAF_ELEMENT_TYPES
		#undef X
		Struct,
		Array,
		Empty
	};

	template<Type type>
	struct Map
	{
		static constexpr bool valid = false;
	};
	template<> struct Map<Float>
	{
		using SysType = float; // type used in CPU side
		static constexpr size_t hlslSize = sizeof(SysType); // size of type on GPU side
		static constexpr const char* code = "F1"; // code used when generating signature for layout
		static constexpr bool valid = true; // flag to check validity of map type
	};
	template<> struct Map<Float2>
	{
		using SysType = dx::XMFLOAT2; // type used in CPU side
		static constexpr size_t hlslSize = sizeof(SysType); // size of type on GPU side
		static constexpr const char* code = "F2"; // code used when generating signature for layout
		static constexpr bool valid = true; // flag to check validity of map type
	};
	template<> struct Map<Float3>
	{
		using SysType = dx::XMFLOAT3; // type used in CPU side
		static constexpr size_t hlslSize = sizeof(SysType); // size of type on GPU side
		static constexpr const char* code = "F3"; // code used when generating signature for layout
		static constexpr bool valid = true; // flag to check validity of map type
	};
	template<> struct Map<Float4>
	{
		using SysType = dx::XMFLOAT4; // type used in CPU side
		static constexpr size_t hlslSize = sizeof(SysType); // size of type on GPU side
		static constexpr const char* code = "F4"; // code used when generating signature for layout
		static constexpr bool valid = true; // flag to check validity of map type
	};
	template<> struct Map<Matrix>
	{
		using SysType = dx::XMFLOAT4X4; // type used in CPU side
		static constexpr size_t hlslSize = sizeof(SysType); // size of type on GPU side
		static constexpr const char* code = "M4"; // code used when generating signature for layout
		static constexpr bool valid = true; // flag to check validity of map type
	};
	template<> struct Map<Bool>
	{
		using SysType = bool; // type used in CPU side
		static constexpr size_t hlslSize = sizeof(SysType); // size of type on GPU side
		static constexpr const char* code = "BL"; // code used when generating signature for layout
		static constexpr bool valid = true; // flag to check validity of map type
	};

	// ensures that every leaf type in master list has an entry in the static attribute map
	#define X(el) static_assert(Map<el>::valid, "Missing map implementation for " #el);
	LEAF_ELEMENT_TYPES
	#undef X

	// enables reverse lookup from SysType to leaf type
	template<typename T>
	struct ReverseMap
	{
		static constexpr bool valid = false;
	};
	#define X(el) \
	template<> struct ReverseMap<typename Map<el>::SysType> \
	{\
		static constexpr Type type = el;\
		static constexpr bool valid = true;\
	};
	LEAF_ELEMENT_TYPES
	#undef X

	// LayoutElements instances form a tree that describes the layout of the data buffer
	// supporting nested aggregates of structs and arrays
	class LayoutElement
	{
	private:
		// this forms the polymorpic base for extra data that Struct and Array have
		struct ExtraDataBase
		{
			virtual ~ExtraDataBase() = default;
		};

		friend class RawLayout;
		friend struct ExtraData;
	public:
		// get a string signature for this element (recursive)
		std::string GetSignature() const noxnd;

		// Check the integrity of the element.
		bool Exists() const noexcept;
		//calculate array indexing offset
		std::pair<size_t, const LayoutElement*> CalculateIndexingOffset(size_t offset, size_t index) const noxnd;
		// [] only works for Structs; access member by name;
		LayoutElement& operator[](const std::string& key) noxnd;
		const LayoutElement& operator[](const std::string& key) const noxnd;
		// T() only works for Arrays; gets the array type layout object
		LayoutElement& T() noxnd;
		const LayoutElement& T() const noxnd;

		// offset based- functions only work after finalization
		size_t GetOffsetBegin() const noxnd;
		size_t GetOffsetEnd() const noxnd;
		// get size in bytes derived from offsets
		size_t GetSizeInBytes() const noxnd;
		// only works for Structs; add LayoutElement
		LayoutElement& Add(Type addedType, std::string name) noxnd;
		template<Type addedType>
		LayoutElement& Add(const std::string& key) noxnd
		{
			return Add(addedType, std::move(key));
		}

		// only works for Arrays; set the type and the # of elements
		LayoutElement& Set(Type addedType, size_t size) noxnd;
		template<Type addedType>
		LayoutElement& Set(size_t size) noxnd
		{
			return Set(addedType, size);
		}

		// returns offset of leaf types for read/write purposes w/ typecheck in Debug
		template<typename T>
		size_t Resolve() const noxnd
		{
			switch (type)
			{
				#define X(el) case el : assert(typeid(Map<el>::SysType) == typeid(T)); return *offset;
				LEAF_ELEMENT_TYPES
				#undef X
			default:
				assert("Tried to resolve non-leaf element" && false);
				return 0u;
			}
		}
	private:
		LayoutElement() noexcept = default;
		LayoutElement(Type typeIn) noxnd;
		size_t Finalize(size_t offsetIn) noxnd;
		std::string GetSignatureForStruct() const noxnd;
		std::string GetSignatureForArray() const noxnd;
		size_t FinalizeForStruct(size_t offsetIn);
		size_t FinalizeForArray(size_t offsetIn);
		static LayoutElement& GetEmptyElement() noexcept
		{
			static LayoutElement empty{};
			return empty;
		}
		static size_t AdvanceToBoundary(size_t offset) noexcept;
		static bool CrossesBoundary(size_t offset, size_t size) noexcept;
		static size_t AdvanceIfCrossesBoundary(size_t offset, size_t size) noexcept;
		static bool ValidateSymbolName(const std::string& name) noexcept;
	private:
		std::optional<size_t> offset;
		Type type = Empty;
		std::unique_ptr<ExtraDataBase> pExtraData;
	};
	

	class Layout
	{
		friend class LayoutCodex;
		friend class Buffer;
	public:
		size_t GetSizeInBytes() const noexcept;
		std::string GetSignature() const noxnd;
	protected:
		Layout(std::shared_ptr<LayoutElement> pRoot) noexcept;
		std::shared_ptr<LayoutElement> pRoot;
	};
	class RawLayout : public Layout
	{
		friend class LayoutCodex;
	public:
		RawLayout() noexcept;
		LayoutElement& operator[](const std::string& key) noxnd;

		template<Type type>
		LayoutElement& Add(const std::string& key) noxnd
		{
			return pRoot->Add<type>(key);
		}
	private:
		// finalize the layout and then relinquish (by yielding the root layout element)
		std::shared_ptr<LayoutElement> DeliverRoot() noexcept;
		void ClearRoot() noexcept;
	};

	class CookedLayout : public Layout
	{
		friend class LayoutCodex;
		friend class Buffer;
	public:
		const LayoutElement& operator[](const std::string& key) const noxnd;
		// add reference to shared ptr to layout tree root
		std::shared_ptr<LayoutElement> ShareRoot() const noexcept;
	private:
		// this ctor used by Codex to return cooked layouts
		CookedLayout(std::shared_ptr<LayoutElement> pRoot) noexcept;
		// use to pilfer the layout tree
		std::shared_ptr<LayoutElement> RelinquishRoot();
	};


	// proxy type that is emitted when keying/indexing into a Buffer
	// implement conversions/assignment that allows manipulation of the
	// raw bytes of the Buffer. This version is const, only supports reading
	// Refs can be further keyed/indexed to traverse the layout structure
	class ConstElementRef
	{
		friend class Buffer;
		friend class ElementRef;
	public:
		// this is a proxy type emitted when you use addressof& on the Ref
		// it allows conversion to pointer type, useful for using Buffer
		// elements with ImGui widget functions etc.
		class Ptr
		{
			friend ConstElementRef;
		public:
			template<typename T>
			operator const T* () const noxnd
			{
				static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in pointer conversion");
				return &static_cast<const T&>(*ref);
			}
		private:
			Ptr(const ConstElementRef* ref) noexcept;
			const ConstElementRef* ref;
		};
	public:
		// check if the indexed element actually exists
		// this is possible because if you key into a Struct with a nonexistent key
		// it will still return an Empty LayoutElement that will enable this test
		// but will not enable any other kind of access
		bool Exists() const noexcept;
		ConstElementRef operator[](const std::string& key) const noxnd;
		ConstElementRef operator[](size_t index) const noxnd;
		Ptr operator&() const noxnd;
		
		// conversion for reading as a supported systype
		template<typename T>
		operator const T& () const noxnd
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in conversion");
			return *reinterpret_cast<const T*>(pBytes + offset + pLayout->Resolve<T>());
		}
	private:
		ConstElementRef(const LayoutElement* pLayout, const char* pBytes, size_t offset) noexcept;
		// this offset is the offset that is built up by indexing into arrays
		// accumulated for every array index in the path of access into the structure
		size_t offset;
		const class LayoutElement* pLayout;
		const char* pBytes;
	};

	class ElementRef
	{
		friend class Buffer;
	public:
		class Ptr
		{
			friend ElementRef;
		public:
			// conversion to read/write pointer to supported SysType
			template<typename T>
			operator T* () const noxnd
			{
				static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in pointer conversion");
				return &static_cast<T&>(*ref);
			}
		private:
			Ptr(ElementRef* ref) noexcept;
			ElementRef* ref;
		};
		operator ConstElementRef() const noexcept;
		bool Exists() const noexcept;
		
		ElementRef operator[](const std::string& key) const noxnd;
		ElementRef operator[](size_t index) const noxnd;
		Ptr operator&() const noxnd;

		// conversion for reading/writing as a supported SysType
		template<typename T>
		operator T& () const noxnd
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in conversion");
			return *reinterpret_cast<T*>(pBytes + offset + pLayout->Resolve<T>());
		}
		// assignment for writing to as a supported SysType
		template<typename T>
		T& operator=(const T& rhs) const noxnd
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in assignment");
			return static_cast<T&>(*this) = rhs;
		}
	private:
		ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset) noexcept;
		size_t offset;
		const LayoutElement* pLayout;
		char* pBytes;
	};

	class Buffer
	{
	public:
		Buffer (RawLayout&& lay) noxnd;
		Buffer (const CookedLayout& lay) noxnd;
		Buffer(CookedLayout&& lay) noxnd;
		Buffer(const Buffer&) noexcept;
		Buffer(Buffer&&) noexcept;

		ElementRef operator[](const std::string& key) noxnd;
		ConstElementRef operator[](const std::string& key) const noxnd;
		const char* GetData() const noexcept;
		size_t GetSizeInBytes() const noexcept;
		const LayoutElement& GetRootLayoutElement() const noexcept;
		void CopyFrom(const Buffer&) noxnd;
		std::shared_ptr<LayoutElement> ShareLayoutRoot() const noexcept;
	private:
		std::shared_ptr<LayoutElement> pLayoutRoot;
		std::vector<char> bytes;
	};

}

#ifndef DCB_IMPL_SOURCE
#undef LEAF_ELEMENT_TYPES
#endif


