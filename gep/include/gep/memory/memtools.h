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

	void addressCopy()
	{

	}

	void destroyPtr()
	{
		//call destructor explicitly
	}
}


