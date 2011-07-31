// packfile_manager.hpp : �p�b�N���X�g����N���X
//

#include <fstream>
#include <vector>
#include <string>

#if defined(_UNICODE) || defined(UNICODE)
#error "Building in unicode is not supported"
#endif

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef INCLUDE_GUARD_BFA758C4_8698_4B3F_A57F_51BB511D0EC5
#define INCLUDE_GUARD_BFA758C4_8698_4B3F_A57F_51BB511D0EC5

class packfile_manager
{
public:

	typedef enum {
		PACK_OPTION_NORMAL = 0,
		PACK_OPTION_ENCRYPT,
	} PACK_OPTION;

private:

	typedef std::vector<std::pair<PACK_OPTION, std::string> > PACK_LIST_TYPE;

	std::string		m_filename;

	bool			m_modify;

	PACK_LIST_TYPE	m_packlist;

public:

	packfile_manager(const char * packfile = NULL);

	// �p�b�N�t�@�C�����̎擾
	const char* filename() const;

	// �ύX�̂���/�Ȃ�
	bool modified() const;

	// �p�b�N���X�g��ǂݍ���
	bool load(const char * packfile = NULL);

	// �p�b�N���X�g����������
	bool save(const char * packfile = NULL);

	// �p�b�N���X�g�ɒǉ�
	bool append(const char * filename, PACK_OPTION option);

#ifdef _DEBUG
	void dump();
#endif
};

inline
packfile_manager::packfile_manager(const char * packfile)
	: m_modify(false)
{
	load(packfile);
}

// �p�b�N�t�@�C�����̎擾
inline
const char* packfile_manager::filename() const
{
	return m_filename.c_str();
}

// �ύX�̂���/�Ȃ�
bool packfile_manager::modified() const
{
	return m_modify;
}

// �p�b�N���X�g��ǂݍ���
inline
bool packfile_manager::load(const char * packfile)
{
	if( !packfile ) {
		packfile = m_filename.c_str();
	} else {
		m_filename = packfile;
	}
	if( m_filename.empty() ) {
		return false;
	}

	std::ifstream ifs(m_filename.c_str());

	if( ifs.fail() ) {
		return false;
	}

	m_modify = false;
	std::string buff;
	while( std::getline(ifs, buff) && !buff.empty() )
	{
		if( '+' == buff[0] ) {
			buff.erase(0, 1);
			append(buff.c_str(), PACK_OPTION_ENCRYPT);
		} else {
			append(buff.c_str(), PACK_OPTION_NORMAL);
		}
	}

	return true;
}

// �p�b�N���X�g����������
inline
bool packfile_manager::save(const char * packfile)
{
	if( !packfile ) {
		packfile = m_filename.c_str();
	}
	
	if( !m_modify ) {
		return true;
	}

	std::ofstream ofs(packfile);
	for(PACK_LIST_TYPE::const_iterator
			ite = m_packlist.begin(),
			last= m_packlist.end();
		ite != last; ++ite)
	{
		ofs << (PACK_OPTION_ENCRYPT == ite->first ? "+" : "")
		    << ite->second << std::endl;
	}

	m_modify = false;

	return true;
}

// �p�b�N���X�g�ɒǉ�
inline
bool packfile_manager::append(const char * filename, PACK_OPTION option)
{
	for(PACK_LIST_TYPE::const_iterator
			ite = m_packlist.begin(),
			last= m_packlist.end();
		ite != last; ++ite)
	{
		if( ite->second == filename ) {
			return false;
		}
	}

	m_packlist.push_back(
		std::make_pair(option, std::string(filename)));
	m_modify = true;

	return true;
}

#ifdef _DEBUG
inline
void packfile_manager::dump()
{
	printf("packfile_manager::m_filename = \"%s\"\n", m_filename.c_str());
	printf("packfile_manager::m_modify = %s\n", m_modify ? "true" : false);
	printf("packfile_manager::m_packlist.size() = %d\n", m_packlist.size());
	for(size_t i = 0; i < m_packlist.size(); i++) {
		printf("packfile_manager::m_packlist[%d] = %s\"%s\"\n"
			, i, PACK_OPTION_ENCRYPT == m_packlist[i].first ? "+" : " ", m_packlist[i].second.c_str());
	}
}
#endif

#endif // !defined(INCLUDE_GUARD_BFA758C4_8698_4B3F_A57F_51BB511D0EC5)
