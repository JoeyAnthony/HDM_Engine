#pragma once
namespace memtools
{
	template <class T>
	void initCopy(T* dst, T* src, int count)
	{
		for (int i = 0; i < count; i++)
		{
			new(dst + i) T(*(src + i));
		}
	}

	template <class T>
	void addressCopy(T* dst, T* srs, int count)
	{
		memcpy(dst, srs, count * sizeof(T));
	}
	/// \brief moves data from src to dst with move constructor, Does not destroy the src
	template <class T>
	void initMove(T* dst, T* src, int count)
	{
		for (int i = 0; i < count; i++)
		{
			new (dst+i) T(std::move(*(src+i)));

			//printf("num: %i ",*a);
			//(ptr)T(std::move(*(src + 1)));
		}
	}

	/// \brief moves data from src to dst without move constructor, Does not destroy the src :not implemented
	template <class T>
	void memMove(T* dst, T* src, int count)
	{
		
	}

	/// \brief shifts memory block with offset, overlap while shifting back?
	template<class T>
	void shift(T* start, size_t count, int offset)
	{
		//shift memory
		memcpy(start + offset, start, count * sizeof(T));
	}

	template<class T>
	void destroyPtr(T* ptr)
	{
		ptr->~T();
	}

	inline size_t AlignedSize(size_t num)
	{
		if (sizeof(void*) == 4)
			return num + (4 - (num % 4))%4;
		else
			return num + (8 - (num % 8))%4;

	}
}


