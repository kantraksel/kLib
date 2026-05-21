#pragma once
#include <memory>
#if _DEBUG
#include <stdexcept>
#endif

template<typename T>
concept TrivialType = std::is_trivially_copyable_v<T>;

template<TrivialType T, typename S = size_t>
class FixedArray
{
private:
	T* mpArray;
	S mSize;
	bool mOwnsArray;

public:
	FixedArray() : mpArray(nullptr), mSize(0), mOwnsArray(false)
	{
	}

	FixedArray(std::nullptr_t) : mpArray(nullptr), mSize(0), mOwnsArray(false)
	{
	}

	FixedArray(S n)
	{
		if (n <= 0)
		{
			mOwnsArray = false;
			mSize = 0;
			mpArray = nullptr;
			return;
		}

		mOwnsArray = true;
		mSize = n;
		mpArray = new T[n];
	}

	FixedArray(const FixedArray& other)
	{
		if (other.mSize == 0)
		{
			mOwnsArray = false;
			mSize = 0;
			mpArray = nullptr;
			return;
		}

		mOwnsArray = other.mOwnsArray;
		mSize = other.mSize;

		if (mOwnsArray)
		{
			mpArray = new T[mSize];
			memcpy(mpArray, other.mpArray, mSize * sizeof(T));
		}
		else
			mpArray = other.mpArray;
	}

private:
	FixedArray(T* pArray, S n, bool ownsArray) : mpArray(pArray), mSize(n), mOwnsArray(ownsArray)
	{
	}

	FixedArray(const T* pArray, S n, bool ownsArray) : mpArray(const_cast<T*>(pArray)), mSize(n), mOwnsArray(ownsArray)
	{
	}

public:
	FixedArray& operator=(const FixedArray& other)
	{
#if _DEBUG
		if (this == std::addressof(other))
			throw std::logic_error("Copying self to self is forbidden in FixedArray");
#endif

		if (mOwnsArray)
			delete[] mpArray;

		if (other.mSize == 0)
		{
			mOwnsArray = false;
			mSize = 0;
			mpArray = nullptr;
			return *this;
		}

		mOwnsArray = other.mOwnsArray;
		mSize = other.mSize;
		
		if (mOwnsArray)
		{
			mpArray = new T[mSize];
			memcpy(mpArray, other.mpArray, mSize * sizeof(T));
		}
		else
			mpArray = other.mpArray;

		return *this;
	}

	FixedArray(FixedArray&& other)
	{
		mOwnsArray = other.mOwnsArray;
		mSize = other.mSize;
		mpArray = other.mpArray;
		other.mOwnsArray = false;
		other.mSize = 0;
		other.mpArray = nullptr;
	}

	FixedArray& operator=(FixedArray&& other)
	{
#if _DEBUG
		if (this == std::addressof(other))
			throw std::logic_error("Moving self to self is forbidden in FixedArray");
#endif

		if (mOwnsArray)
			delete[] mpArray;

		mOwnsArray = other.mOwnsArray;
		mSize = other.mSize;
		mpArray = other.mpArray;
		other.mOwnsArray = false;
		other.mSize = 0;
		other.mpArray = nullptr;

		return *this;
	}

	FixedArray& operator=(std::nullptr_t)
	{
		reset();
		return *this;
	}

	~FixedArray()
	{
		if (mOwnsArray)
			delete[] mpArray;
	}

	T& operator[](S i) const
	{
#if _DEBUG
		if (i >= mSize || i < 0)
			throw std::out_of_range("Array index is out of range");

		if (!mpArray)
			throw std::logic_error("Array pointer is null");
#endif

		return mpArray[i];
	}

	operator T*()
	{
		return mpArray;
	}

	T* operator&()
	{
		return mpArray;
	}

	T* data()
	{
		return mpArray;
	}

	operator const T*() const
	{
		return mpArray;
	}

	const T* operator&() const
	{
		return mpArray;
	}

	const T* data() const
	{
		return mpArray;
	}

	S size() const
	{
		return mSize;
	}

	bool empty() const
	{
		return mSize == 0;
	}

	void reset()
	{
		if (mOwnsArray)
			delete[] mpArray;

		mpArray = nullptr;
		mSize = 0;
		mOwnsArray = false;
	}

	FixedArray span(S offset, S size)
	{
#if _DEBUG
		if (size < 0)
			throw std::out_of_range("Size is out of range");

		if (!mpArray)
			throw std::logic_error("Array pointer is null");
#endif

		auto pArray = mpArray + offset;
		auto pArrayBound = mpArray + mSize;
		if (pArray < mpArray || pArrayBound <= pArray)
			return nullptr;
		auto maxSize = static_cast<S>(pArrayBound - pArray);

		size = std::min(std::max(0ull, size), maxSize);
		return FixedArray(pArray, size, false);
	}

	FixedArray getRef()
	{
		return FixedArray(mpArray, mSize, false);
	}

	const FixedArray getRef() const
	{
		return FixedArray(mpArray, mSize, false);
	}

	static FixedArray CreateArrayRef(T* pArray, S n)
	{
		return FixedArray(pArray, n, false);
	}

	static const FixedArray CreateArrayRef(const T* pArray, S n)
	{
		return FixedArray(pArray, n, false);
	}

	static FixedArray CreateArrayRefUnsafe(void* pArray, S n)
	{
		auto tArray = reinterpret_cast<T*>(pArray);
		return FixedArray(tArray, n, false);
	}

	static const FixedArray CreateArrayRefUnsafe(const void* pArray, S n)
	{
		auto tArray = reinterpret_cast<const T*>(pArray);
		return FixedArray(tArray, n, false);
	}

	static FixedArray WrapArray(T* pArray, S n)
	{
		return FixedArray(pArray, n, n > 0);
	}
	
	static const FixedArray WrapArray(const T* pArray, S n)
	{
		return FixedArray(pArray, n, n > 0);
	}

	static FixedArray Copy(const T* pArray, S n)
	{
		if (n <= 0)
			return nullptr;

		auto out = FixedArray(new T[n], n, true);
		memcpy(out.mpArray, pArray, n * sizeof(T));
		return out;
	}

	static FixedArray Copy(const FixedArray& array)
	{
		return Copy(array, array.size());
	}
};

using FixedArrayChar = FixedArray<char, unsigned int>;
using FixedArrayUChar = FixedArray<unsigned char, unsigned int>;
using FixedArrayCharS = FixedArray<char, size_t>;
using FixedArrayUCharS = FixedArray<unsigned char, size_t>;
