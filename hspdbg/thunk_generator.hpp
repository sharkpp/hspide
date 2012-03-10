/**
 * @file  spplib/thunk_generator.hpp
 * @brief サンクジェネレータ定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2010-2012.
 */

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_AC7B9E21_2435_4031_AE45_DE16FE059E1B_INCLUDE_GUARD
#define SPPLIB_AC7B9E21_2435_4031_AE45_DE16FE059E1B_INCLUDE_GUARD

//////////////////////////////////////////////////////////////////////

/// sharkpp class library namespace
namespace spplib {

// 
template<typename T>
class thunk_generator
{
	BYTE* m_code;
public:
	static T * thunk;
public:
	thunk_generator(void * this_ptr, void * func)
	{
#if (defined(WIN32) || defined(_WIN32)) && (!defined(WIN64) || !defined(_WIN64))
		LPBYTE code = m_code = (LPBYTE)VirtualAlloc(NULL, 64, MEM_COMMIT, PAGE_READWRITE);
		// mov ptr DWORD[&thunk], this_ptr
		*code++                = 0xC7;
		*code++                = 0x05;
		*((unsigned int*)code) = (unsigned int)&thunk;   code += sizeof(unsigned int);
		*((unsigned int*)code) = (unsigned int)this_ptr; code += sizeof(unsigned int);
		// jmp func
		*code++                = 0xE9;
		*((unsigned int*)code) = (unsigned int)func - (unsigned int)code - 4;
		//
		DWORD dwOldProtect;
		VirtualProtect(m_code, 15, PAGE_EXECUTE, &dwOldProtect);
#else
#error "not impliment!"
#endif
	}
	~thunk_generator()
	{
		VirtualFree(m_code, 0, MEM_RELEASE);
	}
	void* get() {
		return m_code;
	}
};

template<typename T>
T * thunk_generator<T>::thunk = NULL;

} // namespace spplib

#endif // !defined(SPPLIB_AC7B9E21_2435_4031_AE45_DE16FE059E1B_INCLUDE_GUARD)
