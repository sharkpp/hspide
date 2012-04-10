/**
 * @file  spplib/thunk_generator.hpp
 * @brief �T���N�W�F�l���[�^��`�E����
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
	typedef unsigned char u8;
	typedef unsigned int  u32;
	u8* m_code_top;  // �t�b�N�p�R�[�h�f�[�^�̐擪�A�h���X
	u8* m_code_last; // �t�b�N�p�R�[�h�f�[�^�̐擪�A�h���X
	u8* m_code;      // �t�b�N�p�R�[�h�f�[�^�̐擪�A�h���X
public:
	static T * thunk;
public:

	// �R���X�g���N�^
	thunk_generator(void * this_ptr, void * func, size_t code_len = 256)
	{
#if (defined(WIN32) || defined(_WIN32)) && (!defined(WIN64) || !defined(_WIN64))
		m_code_top  = m_code = (LPBYTE)VirtualAlloc(NULL, code_len, MEM_COMMIT, PAGE_READWRITE);
		m_code_last = m_code_top + code_len;
		// mov DWORD PTR [&thunk], this_ptr
		*m_code++       = 0xC7;
		*m_code++       = 0x05;
		*((u32*)m_code) = (u32)&thunk;   m_code += sizeof(u32);
		*((u32*)m_code) = (u32)this_ptr; m_code += sizeof(u32);
		// jmp func
		*m_code++       = 0xE9;
		*((u32*)m_code) = (u32)func - (u32)m_code - 4; m_code += sizeof(u32);
		// �y�[�W���������s�݂̂ɕύX
		DWORD op;
		::VirtualProtect(m_code_top, DWORD(m_code - m_code_top), PAGE_EXECUTE, &op);
#else
#error "not impliment!"
#endif
	}

	// �f�X�g���N�^
	~thunk_generator()
	{
		::VirtualFree(m_code_top, 0, MEM_RELEASE);
	}

	// �t�b�N�p�̃R�[�h���擾
	void* get_code()
	{
		return m_code_top;
	}

	// �t�b�N�p�̃R�[�h��}��
	void* injection_code(void * func, int x)
	{
		u8* code_top = m_code;

		DWORD op, op2;
		// �֐��t�b�N�̂��߈�U�������������݂ł���悤�ɂ���
		VirtualProtect(func, 5, PAGE_EXECUTE_READWRITE, &op);

		// ���R�[�h�ǂݍ��݊֐������O�Ɋ֐������荞�܂�
		u8* code = (u8*)func;

		// ��jmp���߂����邱�Ƃ�O��ɂ��Ă���...
		if( 0&&0xE9 != *code )
		{
			// ���ό��m�̂��߃y�[�W���������ɖ߂�
			VirtualProtect(code, 5, op, &op);
			return NULL;
		}

		VirtualProtect(m_code_top, 64, PAGE_EXECUTE_READWRITE, &op2);

		memcpy(m_code, code, x);
		m_code += x;

		u8* jmp_addr  = (u8*)((u32)code + x);
	//	u8* jmp_addr  = (u8*)((u32)code + *((u32*)(code+1)) + 5);
		*code++       = 0xE9; // jmp
		*((u32*)code) = (u32)m_code_top - (u32)code - 4;

		// ���ό��m�̂��߃y�[�W���������ɖ߂�
		VirtualProtect(code, 5, op, &op);

		*m_code++       = 0xE9; // jmp
		*((u32*)m_code) = (u32)jmp_addr - (u32)m_code - 4; m_code += sizeof(u32);

		// �y�[�W���������s�݂̂ɕύX
		::VirtualProtect(m_code_top, DWORD(m_code - m_code_top), PAGE_EXECUTE, &op2);

		return code_top;
	}
};

template<typename T>
T * thunk_generator<T>::thunk = NULL;

} // namespace spplib

#endif // !defined(SPPLIB_AC7B9E21_2435_4031_AE45_DE16FE059E1B_INCLUDE_GUARD)
