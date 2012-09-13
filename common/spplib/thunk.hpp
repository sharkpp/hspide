/**
 * @file  spplib/thunk.hpp
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

class thunk
{
public:

	typedef enum CALLING_CONVENTIONS_TYPE {
		CALL_CDECL = 0,
		CALL_STDCALL,
		CALL_FASTCALL,
	} ;

private:

	class func_holder {
		typedef struct void_ {};
		template<typename T>
		struct static_invoker {
		};
		template<typename T, typename R>
		struct static_invoker<R(T::*)()> {
			typedef static_invoker<R(T::*)()> self_;
			static void* this_ptr;
			static R (T::*func_ptr)();
			static R __stdcall  invokeSTDCALL()  { return invokeCDECL(); }
			static R __fastcall invokeFASTCALL() { return invokeCDECL(); }
			static R __cdecl    invokeCDECL()    { R (T::*p)() = self_::func_ptr; return ((reinterpret_cast<T*>(self_::this_ptr))->*p)(); }
		};
		template<typename T, typename R, typename A1>
		struct static_invoker<R(T::*)(A1)> {
			typedef static_invoker<R(T::*)(A1)> self_;
			static void* this_ptr;
			static R (T::*func_ptr)(A1);
			static R __stdcall  invokeSTDCALL(A1 a1)  { return invokeCDECL(a1); }
			static R __fastcall invokeFASTCALL(A1 a1) { return invokeCDECL(a1); }
			static R __cdecl    invokeCDECL(A1 a1)    { R (T::*p)(A1) = self_::func_ptr; return ((reinterpret_cast<T*>(self_::this_ptr))->*p)(a1); }
		};
		template<typename T, typename R, typename A1, typename A2>
		struct static_invoker<R(T::*)(A1,A2)> {
			typedef static_invoker<R(T::*)(A1,A2)> self_;
			static void* this_ptr;
			static R (T::*func_ptr)(A1,A2);
			static R __stdcall  invokeSTDCALL(A1 a1,A2 a2)  { return invokeCDECL(a1,a2); }
			static R __fastcall invokeFASTCALL(A1 a1,A2 a2) { return invokeCDECL(a1,a2); }
			static R __cdecl    invokeCDECL(A1 a1,A2 a2)    { R (T::*p)(A1,A2) = self_::func_ptr; return ((reinterpret_cast<T*>(self_::this_ptr))->*p)(a1,a2); }
		};
		template<typename T, typename R, typename A1, typename A2, typename A3>
		struct static_invoker<R(T::*)(A1,A2,A3)> {
			typedef static_invoker<R(T::*)(A1,A2,A3)> self_;
			static void* this_ptr;
			static R (T::*func_ptr)(A1,A2,A3);
			static R __stdcall  invokeSTDCALL(A1 a1,A2 a2,A3 a3)  { return invokeCDECL(a1,a2,a3); }
			static R __fastcall invokeFASTCALL(A1 a1,A2 a2,A3 a3) { return invokeCDECL(a1,a2,a3); }
			static R __cdecl    invokeCDECL(A1 a1,A2 a2,A3 a3)    { R (T::*p)(A1,A2,A3) = self_::func_ptr; return ((reinterpret_cast<T*>(self_::this_ptr))->*p)(a1,a2,a3); }
		};
		template<typename T, typename R, typename A1, typename A2, typename A3, typename A4>
		struct static_invoker<R(T::*)(A1,A2,A3,A4)> {
			typedef static_invoker<R(T::*)(A1,A2,A3,A4)> self_;
			static void* this_ptr;
			static R (T::*func_ptr)(A1,A2,A3,A4);
			static R __stdcall  invokeSTDCALL(A1 a1,A2 a2,A3 a3,A4 a4)  { return invokeCDECL(a1,a2,a3,a4); }
			static R __fastcall invokeFASTCALL(A1 a1,A2 a2,A3 a3,A4 a4) { return invokeCDECL(a1,a2,a3,a4); }
			static R __cdecl    invokeCDECL(A1 a1,A2 a2,A3 a3,A4 a4)    { R (T::*p)(A1,A2,A3,A4) = self_::func_ptr; return ((reinterpret_cast<T*>(self_::this_ptr))->*p)(a1,a2,a3,a4); }
		};
		template<typename T, typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
		struct static_invoker<R(T::*)(A1,A2,A3,A4,A5)> {
			typedef static_invoker<R(T::*)(A1,A2,A3,A4,A5)> self_;
			static void* this_ptr;
			static R (T::*func_ptr)(A1,A2,A3,A4,A5);
			static R __stdcall  invokeSTDCALL(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5)  { return invokeCDECL(a1,a2,a3,a4,a5); }
			static R __fastcall invokeFASTCALL(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) { return invokeCDECL(a1,a2,a3,a4,a5); }
			static R __cdecl    invokeCDECL(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5)    { R (T::*p)(A1,A2,A3,A4,A5) = self_::func_ptr; return ((reinterpret_cast<T*>(self_::this_ptr))->*p)(a1,a2,a3,a4,a5); }
		};
		template<typename R>
		struct static_invoker<R(*)()> {
			typedef static_invoker<R(*)()> self_;
			static R (*func_ptr)();
			static R __stdcall  invokeSTDCALL()  { return invokeCDECL(); }
			static R __fastcall invokeFASTCALL() { return invokeCDECL(); }
			static R __cdecl    invokeCDECL()    { R (*p)() = self_::func_ptr; return (*p)(); }
		};
		template<typename R, typename A1>
		struct static_invoker<R(*)(A1)> {
			typedef static_invoker<R(*)(A1)> self_;
			static R (*func_ptr)(A1);
			static R __stdcall  invokeSTDCALL(A1 a1)  { return invokeCDECL(a1); }
			static R __fastcall invokeFASTCALL(A1 a1) { return invokeCDECL(a1); }
			static R __cdecl    invokeCDECL(A1 a1)    { R (*p)(A1) = self_::func_ptr; return (*p)(a1); }
		};
		template<typename R, typename A1, typename A2>
		struct static_invoker<R(*)(A1,A2)> {
			typedef static_invoker<R(*)(A1,A2)> self_;
			static R (*func_ptr)(A1,A2);
			static R __stdcall  invokeSTDCALL(A1 a1,A2 a2)  { return invokeCDECL(a1,a2); }
			static R __fastcall invokeFASTCALL(A1 a1,A2 a2) { return invokeCDECL(a1,a2); }
			static R __cdecl    invokeCDECL(A1 a1,A2 a2)    { R (*p)(A1,A2) = self_::func_ptr; return (*p)(a1,a2); }
		};
		template<typename R, typename A1, typename A2, typename A3>
		struct static_invoker<R(*)(A1,A2,A3)> {
			typedef static_invoker<R(*)(A1,A2,A3)> self_;
			static R (*func_ptr)(A1,A2,A3);
			static R __stdcall  invokeSTDCALL(A1 a1,A2 a2,A3 a3)  { return invokeCDECL(a1,a2,a3); }
			static R __fastcall invokeFASTCALL(A1 a1,A2 a2,A3 a3) { return invokeCDECL(a1,a2,a3); }
			static R __cdecl    invokeCDECL(A1 a1,A2 a2,A3 a3)    { R (*p)(A1,A2,A3) = self_::func_ptr; return (*p)(a1,a2,a3); }
		};
		template<typename R, typename A1, typename A2, typename A3, typename A4>
		struct static_invoker<R(*)(A1,A2,A3,A4)> {
			typedef static_invoker<R(*)(A1,A2,A3,A4)> self_;
			static R (*func_ptr)(A1,A2,A3,A4);
			static R __stdcall  invokeSTDCALL(A1 a1,A2 a2,A3 a3,A4 a4)  { return invokeCDECL(a1,a2,a3,a4); }
			static R __fastcall invokeFASTCALL(A1 a1,A2 a2,A3 a3,A4 a4) { return invokeCDECL(a1,a2,a3,a4); }
			static R __cdecl    invokeCDECL(A1 a1,A2 a2,A3 a3,A4 a4)    { R (*p)(A1,A2,A3,A4) = self_::func_ptr; return (*p)(a1,a2,a3,a4); }
		};
		template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
		struct static_invoker<R(*)(A1,A2,A3,A4,A5)> {
			typedef static_invoker<R(*)(A1,A2,A3,A4,A5)> self_;
			static R (*func_ptr)(A1,A2,A3,A4,A5);
			static R __stdcall  invokeSTDCALL(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5)  { return invokeCDECL(a1,a2,a3,a4,a5); }
			static R __fastcall invokeFASTCALL(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) { return invokeCDECL(a1,a2,a3,a4,a5); }
			static R __cdecl    invokeCDECL(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5)    { R (*p)(A1,A2,A3,A4,A5) = self_::func_ptr; return (*p)(a1,a2,a3,a4,a5); }
		};
	public:
		void* thunk_cdecl_ptr;
		void* thunk_stdcall_ptr;
		void* thunk_fastcall_ptr;
		void* func_ptr;
		void* func_holder_ptr;
		void* this_holder_ptr;
	public:
		template<typename T, typename R>
		func_holder(R(T::*func)())
			: thunk_cdecl_ptr   (reinterpret_cast<void *>(&static_invoker<R(T::*)()>::invokeCDECL))
			, thunk_stdcall_ptr (reinterpret_cast<void *>(&static_invoker<R(T::*)()>::invokeSTDCALL))
			, thunk_fastcall_ptr(reinterpret_cast<void *>(&static_invoker<R(T::*)()>::invokeFASTCALL))
			, func_ptr          (*reinterpret_cast<void **>(&func))
			, func_holder_ptr   (reinterpret_cast<void *>(&static_invoker<R(T::*)()>::func_ptr))
			, this_holder_ptr   (&static_invoker<R(T::*)()>::this_ptr)
		{
		}
		template<typename T, typename R, typename A1>
		func_holder(R(T::*func)(A1))
			: thunk_cdecl_ptr   (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1)>::invokeCDECL))
			, thunk_stdcall_ptr (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1)>::invokeSTDCALL))
			, thunk_fastcall_ptr(reinterpret_cast<void *>(&static_invoker<R(T::*)(A1)>::invokeFASTCALL))
			, func_ptr          (*reinterpret_cast<void **>(&func))
			, func_holder_ptr   (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1)>::func_ptr))
			, this_holder_ptr   (&static_invoker<R(T::*)(A1)>::this_ptr)
		{
		}
		template<typename T, typename R, typename A1, typename A2>
		func_holder(R(T::*func)(A1,A2))
			: thunk_cdecl_ptr   (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2)>::invokeCDECL))
			, thunk_stdcall_ptr (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2)>::invokeSTDCALL))
			, thunk_fastcall_ptr(reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2)>::invokeFASTCALL))
			, func_ptr          (*reinterpret_cast<void **>(&func))
			, func_holder_ptr   (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2)>::func_ptr))
			, this_holder_ptr   (&static_invoker<R(T::*)(A1,A2)>::this_ptr)
		{
		}
		template<typename T, typename R, typename A1, typename A2, typename A3>
		func_holder(R(T::*func)(A1,A2,A3))
			: thunk_cdecl_ptr   (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3)>::invokeCDECL))
			, thunk_stdcall_ptr (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3)>::invokeSTDCALL))
			, thunk_fastcall_ptr(reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3)>::invokeFASTCALL))
			, func_ptr          (*reinterpret_cast<void **>(&func))
			, func_holder_ptr   (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3)>::func_ptr))
			, this_holder_ptr   (&static_invoker<R(T::*)(A1,A2,A3)>::this_ptr)
		{
		}
		template<typename T, typename R, typename A1, typename A2, typename A3, typename A4>
		func_holder(R(T::*func)(A1,A2,A3,A4))
			: thunk_cdecl_ptr   (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3,A4)>::invokeCDECL))
			, thunk_stdcall_ptr (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3,A4)>::invokeSTDCALL))
			, thunk_fastcall_ptr(reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3,A4)>::invokeFASTCALL))
			, func_ptr          (*reinterpret_cast<void **>(&func))
			, func_holder_ptr   (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3,A4)>::func_ptr))
			, this_holder_ptr   (&static_invoker<R(T::*)(A1,A2,A3,A4)>::this_ptr)
		{
		}
		template<typename T, typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
		func_holder(R(T::*func)(A1,A2,A3,A4,A5))
			: thunk_cdecl_ptr   (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3,A4,A5)>::invokeCDECL))
			, thunk_stdcall_ptr (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3,A4,A5)>::invokeSTDCALL))
			, thunk_fastcall_ptr(reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3,A4,A5)>::invokeFASTCALL))
			, func_ptr          (*reinterpret_cast<void **>(&func))
			, func_holder_ptr   (reinterpret_cast<void *>(&static_invoker<R(T::*)(A1,A2,A3,A4,A5)>::func_ptr))
			, this_holder_ptr   (&static_invoker<R(T::*)(A1,A2,A3,A4,A5)>::this_ptr)
		{
		}
		template<typename R>
		func_holder(R(*func)())
			: thunk_cdecl_ptr   (reinterpret_cast<void *>(&static_invoker<R(*)()>::invokeCDECL))
			, thunk_stdcall_ptr (reinterpret_cast<void *>(&static_invoker<R(*)()>::invokeSTDCALL))
			, thunk_fastcall_ptr(reinterpret_cast<void *>(&static_invoker<R(*)()>::invokeFASTCALL))
			, func_ptr          (*reinterpret_cast<void **>(&func))
			, func_holder_ptr   (reinterpret_cast<void *>(&static_invoker<R(*)()>::func_ptr))
			, this_holder_ptr   (NULL)
		{
		}
		template<typename R, typename A1>
		func_holder(R(*func)(A1))
			: thunk_cdecl_ptr   (reinterpret_cast<void *>(&static_invoker<R(*)(A1)>::invokeCDECL))
			, thunk_stdcall_ptr (reinterpret_cast<void *>(&static_invoker<R(*)(A1)>::invokeSTDCALL))
			, thunk_fastcall_ptr(reinterpret_cast<void *>(&static_invoker<R(*)(A1)>::invokeFASTCALL))
			, func_ptr          (*reinterpret_cast<void **>(&func))
			, func_holder_ptr   (reinterpret_cast<void *>(&static_invoker<R(*)(A1)>::func_ptr))
			, this_holder_ptr   (NULL)
		{
		}
		template<typename R, typename A1, typename A2>
		func_holder(R(*func)(A1,A2))
			: thunk_cdecl_ptr   (reinterpret_cast<void *>(&static_invoker<R(*)(A1,A2)>::invokeCDECL))
			, thunk_stdcall_ptr (reinterpret_cast<void *>(&static_invoker<R(*)(A1,A2)>::invokeSTDCALL))
			, thunk_fastcall_ptr(reinterpret_cast<void *>(&static_invoker<R(*)(A1,A2)>::invokeFASTCALL))
			, func_ptr          (*reinterpret_cast<void **>(&func))
			, func_holder_ptr   (reinterpret_cast<void *>(&static_invoker<R(*)(A1,A2)>::func_ptr))
			, this_holder_ptr   (NULL)
		{
		}
		template<typename R, typename A1, typename A2, typename A3>
		func_holder(R(*func)(A1,A2,A3))
			: thunk_cdecl_ptr   (reinterpret_cast<void *>(&static_invoker<R(*)(A1,A2,A3)>::invokeCDECL))
			, thunk_stdcall_ptr (reinterpret_cast<void *>(&static_invoker<R(*)(A1,A2,A3)>::invokeSTDCALL))
			, thunk_fastcall_ptr(reinterpret_cast<void *>(&static_invoker<R(*)(A1,A2,A3)>::invokeFASTCALL))
			, func_ptr          (*reinterpret_cast<void **>(&func))
			, func_holder_ptr   (reinterpret_cast<void *>(&static_invoker<R(*)(A1,A2,A3)>::func_ptr))
			, this_holder_ptr   (NULL)
		{
		}
		template<typename R, typename A1, typename A2, typename A3, typename A4>
		func_holder(R(*func)(A1,A2,A3,A4))
			: thunk_cdecl_ptr   (reinterpret_cast<void *>(&static_invoker<R(*)(A1,A2,A3,A4)>::invokeCDECL))
			, thunk_stdcall_ptr (reinterpret_cast<void *>(&static_invoker<R(*)(A1,A2,A3,A4)>::invokeSTDCALL))
			, thunk_fastcall_ptr(reinterpret_cast<void *>(&static_invoker<R(*)(A1,A2,A3,A4)>::invokeFASTCALL))
			, func_ptr          (*reinterpret_cast<void **>(&func))
			, func_holder_ptr   (reinterpret_cast<void *>(&static_invoker<R(*)(A1,A2,A3,A4)>::func_ptr))
			, this_holder_ptr   (NULL)
		{
		}
		template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
		func_holder(R(*func)(A1,A2,A3,A4,A5))
			: thunk_cdecl_ptr   (reinterpret_cast<void *>(&static_invoker<R(*)(A1,A2,A3,A4,A5)>::invokeCDECL))
			, thunk_stdcall_ptr (reinterpret_cast<void *>(&static_invoker<R(*)(A1,A2,A3,A4,A5)>::invokeSTDCALL))
			, thunk_fastcall_ptr(reinterpret_cast<void *>(&static_invoker<R(*)(A1,A2,A3,A4,A5)>::invokeFASTCALL))
			, func_ptr          (*reinterpret_cast<void **>(&func))
			, func_holder_ptr   (reinterpret_cast<void *>(&static_invoker<R(*)(A1,A2,A3,A4,A5)>::func_ptr))
			, this_holder_ptr   (NULL)
		{
		}
	};

	static const size_t CALLING_CONVENTIONS_NUM = CALL_FASTCALL + 1;

	typedef unsigned char u8;
	typedef unsigned int  u32;
	u8* m_code_top;      // フック用コードデータの先頭アドレス
	u8* m_code_last;     // フック用コードデータの先頭アドレス
	u8* m_code;          // フック用コードデータの先頭アドレス
	u8* m_thunk[CALLING_CONVENTIONS_NUM]; // 
	size_t m_code_max;   // フック用のコード追加用のバッファの最大長

	// from instructionLength.hpp
	size_t thunk::instructionLength_x86(const u8* codes);

public:

	thunk(){}

	thunk(void* this_, const func_holder& f, size_t code_max = 256)
	{
		m_code_max = code_max;
#if (defined(WIN32) || defined(_WIN32)) && (!defined(WIN64) || !defined(_WIN64))
		m_code_top  = m_code = (u8*)VirtualAlloc(NULL, m_code_max, MEM_COMMIT, PAGE_READWRITE);
		m_code_last = m_code_top + m_code_max;
		// 呼び出しタイプごとにコードを生成
		for(size_t call_type = 0; call_type < CALLING_CONVENTIONS_NUM; call_type++)
		{
			m_thunk[call_type] = m_code;
			void* thunk = NULL;
			switch(call_type) {
			case CALL_CDECL:    thunk = f.thunk_cdecl_ptr;    break;
			case CALL_STDCALL:  thunk = f.thunk_stdcall_ptr;  break;
			case CALL_FASTCALL: thunk = f.thunk_fastcall_ptr; break;
			}
			// mov dword ptr [f.this_holder_ptr], this_
			*m_code++       = 0xC7;
			*m_code++       = 0x05;
			*((u32*)m_code) = (u32)f.this_holder_ptr; m_code += sizeof(u32);
			*((u32*)m_code) = (u32)this_;             m_code += sizeof(u32);
			// mov dword ptr [f.func_holder_ptr], f.func_ptr
			*m_code++       = 0xC7;
			*m_code++       = 0x05;
			*((u32*)m_code) = (u32)f.func_holder_ptr; m_code += sizeof(u32);
			*((u32*)m_code) = (u32)f.func_ptr;        m_code += sizeof(u32);
			// jmp func
			*m_code++       = 0xE9;
			*((u32*)m_code) = (u32)thunk - (u32)m_code - 4; m_code += sizeof(u32);
		}
		// ページ属性を実行のみに変更
		DWORD op;
		::VirtualProtect(m_code_top, DWORD(m_code - m_code_top), PAGE_EXECUTE, &op);
#else
#error "not impliment!"
#endif
	}

	thunk(const func_holder& f, size_t code_max = 256)
	{
		m_code_max = code_max;
#if (defined(WIN32) || defined(_WIN32)) && (!defined(WIN64) || !defined(_WIN64))
		m_code_top  = m_code = (u8*)VirtualAlloc(NULL, m_code_max, MEM_COMMIT, PAGE_READWRITE);
		m_code_last = m_code_top + m_code_max;
		// 呼び出しタイプごとにコードを生成
		for(size_t call_type = 0; call_type < CALLING_CONVENTIONS_NUM; call_type++)
		{
			m_thunk[call_type] = m_code;
			void* thunk = NULL;
			switch(call_type) {
			case CALL_CDECL:    thunk = f.thunk_cdecl_ptr;    break;
			case CALL_STDCALL:  thunk = f.thunk_stdcall_ptr;  break;
			case CALL_FASTCALL: thunk = f.thunk_fastcall_ptr; break;
			}
			// mov dword ptr [f.func_holder_ptr], f.func_ptr
			*m_code++       = 0xC7;
			*m_code++       = 0x05;
			*((u32*)m_code) = (u32)f.func_holder_ptr; m_code += sizeof(u32);
			*((u32*)m_code) = (u32)f.func_ptr;        m_code += sizeof(u32);
			// jmp func
			*m_code++       = 0xE9;
			*((u32*)m_code) = (u32)thunk - (u32)m_code - 4; m_code += sizeof(u32);
		}
		// ページ属性を実行のみに変更
		DWORD op;
		::VirtualProtect(m_code_top, DWORD(m_code - m_code_top), PAGE_EXECUTE, &op);
#else
#error "not impliment!"
#endif
	}

	~thunk()
	{
		::VirtualFree(m_code_top, 0, MEM_RELEASE);
	}

	void* get_code(CALLING_CONVENTIONS_TYPE call_type = CALL_CDECL)
	{
		return (void*)m_thunk[call_type];
	}

	// フック用のコードを挿入
	void* injection_code(void * func, CALLING_CONVENTIONS_TYPE call_type = CALL_CDECL)
	{
		u8* code_top = m_code;

		DWORD op, op2;

		// 次コード読み込み関数処理前に関数を割り込ます
		u8* code = (u8*)func;

		::VirtualProtect(m_code_top, m_code_max, PAGE_EXECUTE_READWRITE, &op);

		u8* jmp_addr;

		// 無条件 jmp の場合は飛び先に飛んでフック
		while( 0xE9 == *code )
		{
			// 飛び先を計算
			code  = (u8*)((u32)code + *((u32*)(code+1)) + 5);
		}

		size_t copy_len = 0;

		// ジャンプコードを上書くために待避するサイズを取得
		for(; copy_len < 5; ) {
			size_t len = instructionLength_x86(code + copy_len);
			if( !len ) {
				return NULL;
			}
			copy_len += len;
		}

		// 待避
		::memcpy(m_code, code, copy_len);

		// 退避した中に call があった場合、アドレスを書き換える
		for(size_t pos = 0; pos < copy_len; ) {
			size_t len = instructionLength_x86(m_code + pos);
			if( 0xE8 == m_code[pos] ) { // call XXXX
				u8* addr = m_code + pos + 1;
				*((u32*)addr) = (u32)((u32)code + *((u32*)addr) - (u32)m_code);
			}
			pos += len;
		}

		m_code += copy_len;

		// 飛び先を計算
		jmp_addr = (u8*)((u32)code + copy_len);

		// 関数フックのため一旦属性を書き込みできるようにする
		::VirtualProtect(code, 5, PAGE_EXECUTE_READWRITE, &op2);

#ifdef _DEBUG
		for(size_t i = 0; i < copy_len; i++) {
			code[i] = 0xCC; // int3
		}
#endif

		// フックする関数の先頭をフックルーチンへのジャンプに書き換える
		*code++       = 0xE9; // jmp
		*((u32*)code) = (u32)m_thunk[call_type] - (u32)code - 4;

		// 改変検知のためページ属性を元に戻す
		::VirtualProtect(code, 5, op2, &op2);

		// 元の関数位置へジャンプするコードを追加
		*m_code++       = 0xE9; // jmp
		*((u32*)m_code) = (u32)jmp_addr - (u32)m_code - 4; m_code += sizeof(u32);

		// ページ属性を実行のみに変更
		::VirtualProtect(m_code_top, m_code_max, PAGE_EXECUTE, &op);

		return (void*)code_top;
	}

	// フックコードの削除
	bool uninjection_code(void* func, void* hook_code)
	{
		u8* code_= (u8*)func;
		u8* code = code_;

		// 無条件 jmp の場合は飛び先に飛んでフック
		while( 0xE9 == *code_ )
		{
			// 飛び先を計算
			code   = code_;
			code_  = (u8*)((u32)code_ + *((u32*)(code_+1)) + 5);
		}

		size_t copy_len = 0;

		// ジャンプコードを上書くために待避するサイズを取得
		for(; copy_len < 5; ) {
			size_t len = instructionLength_x86((u8*)hook_code + copy_len);
			if( !len ) {
				return false;
			}
			copy_len += len;
		}

		DWORD op;
		::VirtualProtect(code, copy_len, PAGE_EXECUTE_READWRITE, &op);

		::memcpy(code, hook_code, copy_len);

		// 復帰した中に call があった場合、アドレスを書き換える
		for(size_t pos = 0; pos < copy_len; ) {
			size_t len = instructionLength_x86(code + pos);
			if( 0xE8 == code[pos] ) { // call XXXX
				u8* addr = code + pos + 1;
				*((u32*)addr) = (u32)((u32)hook_code + *((u32*)addr) - (u32)code);
			}
			pos += len;
		}

		::VirtualProtect(code, copy_len, op, &op);

		return true;
	}
};

template<typename T, typename R> void*   thunk::func_holder::static_invoker<R(T::*)()>::this_ptr    = NULL;
template<typename T, typename R> R (T::* thunk::func_holder::static_invoker<R(T::*)()>::func_ptr)() = NULL;

template<typename T, typename R, typename A1> void*   thunk::func_holder::static_invoker<R(T::*)(A1)>::this_ptr      = NULL;
template<typename T, typename R, typename A1> R (T::* thunk::func_holder::static_invoker<R(T::*)(A1)>::func_ptr)(A1) = NULL;

template<typename T, typename R, typename A1, typename A2> void*   thunk::func_holder::static_invoker<R(T::*)(A1,A2)>::this_ptr         = NULL;
template<typename T, typename R, typename A1, typename A2> R (T::* thunk::func_holder::static_invoker<R(T::*)(A1,A2)>::func_ptr)(A1,A2) = NULL;

template<typename T, typename R, typename A1, typename A2, typename A3> void*   thunk::func_holder::static_invoker<R(T::*)(A1,A2,A3)>::this_ptr            = NULL;
template<typename T, typename R, typename A1, typename A2, typename A3> R (T::* thunk::func_holder::static_invoker<R(T::*)(A1,A2,A3)>::func_ptr)(A1,A2,A3) = NULL;

template<typename T, typename R, typename A1, typename A2, typename A3, typename A4> void*   thunk::func_holder::static_invoker<R(T::*)(A1,A2,A3,A4)>::this_ptr               = NULL;
template<typename T, typename R, typename A1, typename A2, typename A3, typename A4> R (T::* thunk::func_holder::static_invoker<R(T::*)(A1,A2,A3,A4)>::func_ptr)(A1,A2,A3,A4) = NULL;

template<typename T, typename R, typename A1, typename A2, typename A3, typename A4, typename A5> void*   thunk::func_holder::static_invoker<R(T::*)(A1,A2,A3,A4,A5)>::this_ptr                  = NULL;
template<typename T, typename R, typename A1, typename A2, typename A3, typename A4, typename A5> R (T::* thunk::func_holder::static_invoker<R(T::*)(A1,A2,A3,A4,A5)>::func_ptr)(A1,A2,A3,A4,A5) = NULL;

template<typename R> R (* thunk::func_holder::static_invoker<R(*)()>::func_ptr)() = NULL;

template<typename R, typename A1> R (* thunk::func_holder::static_invoker<R(*)(A1)>::func_ptr)(A1) = NULL;

template<typename R, typename A1, typename A2> R (* thunk::func_holder::static_invoker<R(*)(A1,A2)>::func_ptr)(A1,A2) = NULL;

template<typename R, typename A1, typename A2, typename A3> R (* thunk::func_holder::static_invoker<R(*)(A1,A2,A3)>::func_ptr)(A1,A2,A3) = NULL;

template<typename R, typename A1, typename A2, typename A3, typename A4> R (* thunk::func_holder::static_invoker<R(*)(A1,A2,A3,A4)>::func_ptr)(A1,A2,A3,A4) = NULL;

template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5> R (* thunk::func_holder::static_invoker<R(*)(A1,A2,A3,A4,A5)>::func_ptr)(A1,A2,A3,A4,A5) = NULL;

// from instructionLength.hpp
inline
size_t thunk::instructionLength_x86(const u8* codes)
{
	const u8* top = codes;
	u8 prefix, opecode;
	bool presentModRM = false;
	bool presentSIB = false;
	u8 bitModRM = 0;
	u8 bitSIB = 0;
	size_t  displacement = 0;
	size_t  immediate = 0;
	size_t  address_size = 4;

	// Prefix
	prefix = *codes++;

	for(;;)
	{
		// instruction prefixes check from Volume.2A 2.1.1
		switch( prefix )
		{
		case 0x67: // Group 4
			address_size = 2;
		case 0xF0: case 0xF2: case 0xF3: // Group 1
		case 0x2E: case 0x36: case 0x3E: // Group 2
		case 0x26: case 0x64: case 0x65:
		case 0x66: // Group 3
			prefix = *codes++;
			continue;
		// opecode
		default:
			break;
		}
		break;
	}
	opecode = prefix;

	// Opcode
	switch( opecode )
	{
	// XX
	                                                                  case 0x06: case 0x07:
	                                                                  case 0x0E:
	                                                                  case 0x16: case 0x17:
	                                                                  case 0x1E: case 0x1F:
	                                                                             case 0x27:
	                                                                             case 0x2F:
	                                                                             case 0x37:
	                                                                             case 0x3F:
	case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E: case 0x4F:
	case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
	case 0x60: case 0x61:
	                                            case 0x6C: case 0x6D: case 0x6E: case 0x6F:
	case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
	case 0x98: case 0x99:            case 0x9B: case 0x9C: case 0x9D: case 0x9E: case 0x9F:
	                                            case 0xA4: case 0xA5: case 0xA6: case 0xA7:
	                      case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAE: case 0xAF:
	                                 case 0xC3:
	           case 0xC9:            case 0xCB: case 0xCC:            case 0xCE: case 0xCF:
	                                                                             case 0xD7:
	                                            case 0xEC: case 0xED: case 0xEE: case 0xEF:
	                                            case 0xF4: case 0xF5:
	case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD:
		break;
	// XX ib/cb
	                                            case 0x04:
	                                            case 0x0C:
	                                            case 0x14:
	                                            case 0x1C:
	                                            case 0x24:
	                                            case 0x2C:
	                                            case 0x34:
	                                            case 0x3C:
	                      case 0x6A:
	case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x76: case 0x77:
	case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7E: case 0x7F:
	                                                       case 0xCD:
	                                            case 0xD4: case 0xD5:
	case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5: case 0xE6: case 0xE7: 
	                                 case 0xEB:
	case 0xA0:            case 0xA2:
	case 0xA8:
	case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB7:
		immediate = 1;
		break;
	// XX iw
	case 0xC2: case 0xCA:
		immediate = 2;
		break;
	// XX id/iw
	                                                       case 0x05:
	                                                       case 0x0D:
	                                                       case 0x15:
	                                                       case 0x1D:
	                                                       case 0x25:
	                                                       case 0x2D:
	                                                       case 0x35:
	                                                       case 0x3D:
	case 0x68:
	           case 0xA1:            case 0xA3:
	           case 0xA9:
	case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBE: case 0xBF:
	case 0xE8: case 0xE9:
		immediate = address_size;
		break;
	// XX cp
	case 0x9A: case 0xEA:
		immediate = address_size + 2;
		break;
	// XX /r
	case 0x00: case 0x01: case 0x02: case 0x03:
	case 0x08: case 0x09: case 0x0A: case 0x0B:
	case 0x10: case 0x11: case 0x12: case 0x13:
	case 0x18: case 0x19: case 0x1A: case 0x1B:
	case 0x20: case 0x21: case 0x22: case 0x23:
	case 0x28: case 0x29: case 0x2A: case 0x2B:
	case 0x30: case 0x31: case 0x32: case 0x33:
	case 0x38: case 0x39: case 0x3A: case 0x3B:
	                      case 0x62: case 0x63: case 0x84: case 0x85: case 0x86: case 0x87:
	case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8E: case 0x8F:
	                                            case 0xC4: case 0xC5: 
	case 0xD0: case 0xD1: case 0xD2: case 0xD3:
	case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDD: case 0xDE: case 0xDF:
	                                                                  case 0xFE: case 0xFF:
		presentModRM = true;
		break;
	// F6 /r
	case 0xF6:
		presentModRM = true;
		immediate = 0 == (*codes & 0x38) ? 1 : 0;
		break;
	// F7 /r
	case 0xF7:
		presentModRM = true;
		immediate = 0 == (*codes & 0x38) ? address_size : 0;
		break;
	// XX /r ib
	case 0x6B: case 0xC6: case 0x80: case 0x83: case 0xC0: case 0xC1:
		presentModRM = true;
		immediate = 1;
		break;
	// XX /r id/iw
	case 0x69: case 0xC7: case 0x81:
		presentModRM = true;
		immediate = address_size;
		break;
	// XX iw ib
	case 0xC8: 
		immediate = 3;
		break;

	// 0F XX XX
	case 0x0F:
		opecode = *codes++;
		switch( opecode )
		{
		// 0F XX
		case 0x06: case 0x08: case 0x09: case 0x0B: case 0x30: case 0x31: case 0x32: case 0x33:
		case 0x34: case 0x35: case 0x77: case 0xA0: case 0xA1: case 0xA2: case 0xA8: case 0xA9:
		case 0xAA: case 0xC8: case 0xC9: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCE:
		case 0xCF:
			break;
		// 0F XX /r
		case 0x00: case 0x01: case 0x02: case 0x03:
		case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
		case 0x18:
		case 0x20: case 0x21: case 0x22: case 0x23:
		case 0x28: case 0x29: case 0x2A: case 0x2B: case 0x2C: case 0x2D: case 0x2E: case 0x2F:
		case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
		case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4E: case 0x4F:
		case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
		case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D: case 0x5E: case 0x5F:
		case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67:
		case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6E: case 0x6F:
		                                            case 0x74: case 0x75: case 0x76:
		                                                                  case 0x7E: case 0x7F:
		case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
		case 0x98: case 0x99: case 0x9A: case 0x9B: case 0x9C: case 0x9D: case 0x9E: case 0x9F:
		                                 case 0xA3:            case 0xA5: case 0xA6: case 0xA7:
		                                 case 0xAB:            case 0xAD: case 0xAE: case 0xAF:
		case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB7:
		                                 case 0xBB: case 0xBC: case 0xBD: case 0xBE: case 0xBF:
		case 0xC0: case 0xC1:            case 0xC3:                                  case 0xC7:
		           case 0xD1: case 0xD2: case 0xD3: case 0xD4: case 0xD5:            case 0xD7:
		case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDD: case 0xDE: case 0xDF:
		case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5:            case 0xE7:
		case 0xE8: case 0xE9: case 0xEA: case 0xEB: case 0xEC: case 0xED: case 0xEE: case 0xEF:
		           case 0xF1: case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF6: case 0xF7:
		case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD: case 0xFE:
			presentModRM = true;
			break;
		// 0F XX /r imm8/ib
		case 0xA4: case 0xAC: case 0xBA:
		case 0x70: case 0x71: case 0x72: case 0x73: case 0xC2: case 0xC4: case 0xC5: case 0xC6: 
			presentModRM = true;
			immediate = 1;
			break;
		// 0F XX cb/cw
		case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87:
		case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8E: case 0x8F:
			immediate = address_size;
			break;
		}
		break;
	default:
		return 0;
	}

	// ModR/M
	if( presentModRM )
	{
		bitModRM = *codes++;

		switch( address_size )
		{
		case 2: // Volume.2A 2.1.5 Table 2-1.
			if(  0x80 == (bitModRM & 0xC0) || // Mod == 10
				(0x00 == (bitModRM & 0xC0) && // Mod == 00 &&
				 0x06 == (bitModRM & 0x07)) ) //   R/M == 110
			{
				displacement = 2;
			}
			if(  0x40 == (bitModRM & 0xC0) ) // Mod == 01
			{
				displacement = 1;
			}
			break;
		case 4: // Volume.2A 2.1.5 Table 2-2.
			// SIB present check
			presentSIB = 0x04 == (bitModRM & 0x07) &&
			             0xC0 != (bitModRM & 0xC0);
			// displacement size check
			if(  0x80 == (bitModRM & 0xC0) || // Mod == 10
				(0x00 == (bitModRM & 0xC0) && // Mod == 00 &&
				 0x05 == (bitModRM & 0x07)) ) //   R/M == 101
			{
				displacement = 4;
			}
			if(  0x40 == (bitModRM & 0xC0) ) // Mod == 01
			{
				displacement = 1;
			}
			break;
		}

		// SIB
		if( presentSIB )
		{
			// Volume.2A 2.1.5 Table 2-3.
			bitSIB = *codes++;
			// displacement size check
			if( 0x00 == (bitModRM & 0xC0) && // Mod == 00 &&
				0x05 == (bitSIB & 0x07) )    //   Base == 101
			{
				displacement = 4;
			}
		}
	}

	codes += displacement + immediate;

	return size_t(codes - top);
}

} // namespace spplib

#endif // !defined(SPPLIB_AC7B9E21_2435_4031_AE45_DE16FE059E1B_INCLUDE_GUARD)
