/**
 * @file  spplib/module_loader.hpp
 * @brief モジュール管理クラス定義・実装
 *
 * Written by sharkpp<webmaster@sharkpp.net> in 2009.
 */

#include <windows.h>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef SPPLIB_3A48909C_86EE_4588_8943_4CB9583173A1_INCLUDE_GUARD
#define SPPLIB_3A48909C_86EE_4588_8943_4CB9583173A1_INCLUDE_GUARD

/// sharkpp class library namespace
namespace spplib {

/**
 * @brief モジュール管理クラス
 */
template<typename T>
class module_loader
{
private:

	bool		m_attached;
	HMODULE		m_module;

protected:

	module_loader()
		: m_attached(false)
		, m_module(NULL)
	{
	}

public:

	module_loader(const char * module_name, bool attached = false)
		: m_attached(false)
		, m_module(NULL)
	{
		attached ? attach(module_name) : load(module_name);
	}

	module_loader(const wchar_t * module_name, bool attached = false)
		: m_attached(false)
		, m_module(NULL)
	{
		attached ? attach(module_name) : load(module_name);
	}

	virtual ~module_loader()
	{
		if( !m_attached ) {
			::FreeLibrary(m_module);
		}
	}

	template<typename Tp>
	void def(const char * proc_name, Tp & proc)
	{
		proc = reinterpret_cast<Tp>( ::GetProcAddress(m_module, proc_name) );
	}

	bool load(const char * module_name)
	{
		if( !m_module &&
			NULL != (m_module = ::LoadLibraryA(module_name)) )
		{
			return true;
		}
		return false;
	}

	bool load(const wchar_t * module_name)
	{
		if( !m_module &&
			NULL != (m_module = ::LoadLibraryW(module_name)) )
		{
			return true;
		}
		return false;
	}

	bool unload()
	{
		if( !m_attached && m_module ) {
			::FreeLibrary(m_module);
			m_module   = NULL;
			return true;
		}
		return false;
	}

	bool attach(const char * module_name)
	{
		if( NULL != (m_module = ::GetModuleHandleA(module_name)) ) {
			m_attached = true;
			return true;
		}
		return false;
	}

	bool attach(const wchar_t * module_name)
	{
		if( NULL != (m_module = ::GetModuleHandleW(module_name)) ) {
			m_attached = true;
			return true;
		}
		return false;
	}

	bool dettach()
	{
		if( m_attached && m_module ) {
			m_attached = false;
			m_module   = NULL;
			return true;
		}
		return false;
	}

};

} // namespace spplib

#endif // !defined(SPPLIB_3A48909C_86EE_4588_8943_4CB9583173A1_INCLUDE_GUARD)
