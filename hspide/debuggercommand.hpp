#pragma once

class CDebuggerCommand
{
	unsigned char* m_data;
	size_t m_size;
	size_t m_allocate;
	size_t m_offset;

public:

	static const unsigned char CMD_CONNECT			= 0x00;
	static const unsigned char CMD_PUT_LOG			= 0x01;
	static const unsigned char CMD_SET_BREAK_POINT	= 0x02;
	static const unsigned char CMD_STOP_RUNNING		= 0x03;
	static const unsigned char CMD_SUSPEND_DEBUG	= 0x04;
	static const unsigned char CMD_STOP_DEBUG		= 0x05;

	class scoped_ptr {
		friend class CDebuggerCommand;
		const unsigned char* m_data;
		size_t m_size;
		CDebuggerCommand* m_parent;
		scoped_ptr();
		scoped_ptr(CDebuggerCommand* parent, const unsigned char* data, size_t size);
	public:
		scoped_ptr(const scoped_ptr & lhs);
		scoped_ptr & operator=(const scoped_ptr & lhs);
		~scoped_ptr();
		void detach();
		const unsigned char* data() const;
		size_t size() const;
		bool valid() const;
	};

public:

	CDebuggerCommand();
	CDebuggerCommand(const void * data, size_t size);
	~CDebuggerCommand();

	void swap(CDebuggerCommand& lhs);

	const scoped_ptr read(unsigned long long & id, unsigned char & cmd_id, unsigned int & length);

	void write(unsigned long long id, unsigned char cmd_id, const void* data = NULL, unsigned int length = 0);

	void push(const void* data, unsigned int length);

	const unsigned char* data() const;
	size_t size() const;

protected:

	void unlock(const void * data, size_t size);
};

inline
CDebuggerCommand::scoped_ptr::scoped_ptr()
	: m_data(NULL)
	, m_size(0)
	, m_parent(NULL)
{
}

inline
CDebuggerCommand::scoped_ptr::scoped_ptr(CDebuggerCommand* parent, const unsigned char* data, size_t size)
	: m_data(data)
	, m_size(size)
	, m_parent(parent)
{
}

inline
CDebuggerCommand::scoped_ptr::scoped_ptr(const scoped_ptr & lhs)
	: m_data(NULL)
	, m_size(0)
	, m_parent(NULL)
{
	operator=(lhs);
}

inline
CDebuggerCommand::scoped_ptr & CDebuggerCommand::scoped_ptr::operator=(const scoped_ptr & lhs)
{
	scoped_ptr * lhs_ = &const_cast<scoped_ptr&>(lhs);
	if( m_data ) {
		m_parent->unlock(m_data, m_size);
	}
	m_data = lhs_->m_data;
	m_size = lhs_->m_size;
	m_parent = lhs_->m_parent;
	lhs_->m_data = NULL;
	return *this;
}

inline
CDebuggerCommand::scoped_ptr::~scoped_ptr()
{
	if( m_data ) {
		m_parent->unlock(m_data, m_size);
	}
}

inline
void CDebuggerCommand::scoped_ptr::detach()
{
	if( m_data ) {
		m_parent->unlock(m_data, m_size);
	}
}

inline
const unsigned char* CDebuggerCommand::scoped_ptr::data() const
{
	return m_data;
}

inline
size_t CDebuggerCommand::scoped_ptr::size() const
{
	return m_size;
}

inline
bool CDebuggerCommand::scoped_ptr::valid() const
{
	return NULL != m_data;
}

inline
CDebuggerCommand::CDebuggerCommand()
	: m_data(NULL)
	, m_size(0)
	, m_allocate(0)
	, m_offset(0)
{
}

inline
CDebuggerCommand::CDebuggerCommand(const void * data, size_t size)
	: m_data(new unsigned char[size])
	, m_size(size)
	, m_allocate(size)
	, m_offset(0)
{
	memcpy(m_data, data, size);
}

inline
CDebuggerCommand::~CDebuggerCommand()
{
	delete [] m_data;
}

inline
void CDebuggerCommand::swap(CDebuggerCommand& lhs)
{
	unsigned char* data = m_data;
	size_t size     = m_size;
	size_t allocate = m_allocate;
	size_t offset   = m_offset;
	m_data          = lhs.m_data;
	m_size          = lhs.m_size;
	m_allocate      = lhs.m_allocate;
	m_offset        = lhs.m_offset;
	lhs.m_data      = data;
	lhs.m_size      = size;
	lhs.m_allocate  = allocate;
	lhs.m_offset    = offset;
}

inline
const unsigned char* CDebuggerCommand::data() const
{
	return m_data;
}

inline
size_t CDebuggerCommand::size() const
{
	return m_size;
}

inline
const CDebuggerCommand::scoped_ptr CDebuggerCommand::read(unsigned long long & id, unsigned char & cmd_id, unsigned int & length)
{
	if( m_size - m_offset < sizeof(id) + sizeof(cmd_id) + sizeof(length) )
	{
		// まだ十分なサイズが揃っていない
		return scoped_ptr();
	}

	memcpy(&id, m_data + m_offset, sizeof(id));
	m_offset += sizeof(id);

	memcpy(&cmd_id, m_data + m_offset, sizeof(cmd_id));
	m_offset += sizeof(cmd_id);

	memcpy(&length, m_data + m_offset, sizeof(length));
	m_offset += sizeof(length);

	scoped_ptr ptr(this, m_data + m_offset, length);
	m_offset += length;

	return ptr;
}

inline
void CDebuggerCommand::write(unsigned long long id, unsigned char cmd_id, const void* data, unsigned int length)
{
	size_t write_size = length + sizeof(id) + sizeof(cmd_id) + sizeof(length);

	if( m_allocate <= m_size + write_size )
	{
		m_allocate = (m_size + write_size + 1024) & 1023;
		unsigned char* old_data = m_data;
		m_data = new unsigned char[m_allocate];
		memcpy(m_data, old_data, m_size);
		delete [] old_data;
	}

	memcpy(m_data + m_size, &id, sizeof(id));
	m_size += sizeof(id);

	memcpy(m_data + m_size, &cmd_id, sizeof(cmd_id));
	m_size += sizeof(cmd_id);

	memcpy(m_data + m_size, &length, sizeof(length));
	m_size += sizeof(length);

	if( data ) {
		memcpy(m_data + m_size, data, length);
		m_size += length;
	}

	m_offset = 0;
}

inline
void CDebuggerCommand::push(const void* data, unsigned int length)
{
	if( m_allocate <= m_size + length )
	{
		m_allocate = (m_size + length + 1024) & 1023;
		unsigned char* old_data = m_data;
		m_data = new unsigned char[m_allocate];
		memcpy(m_data, old_data, m_size);
		delete [] old_data;
	}

	if( data ) {
		memcpy(m_data + m_size, data, length);
		m_size += length;
	}

	m_offset = 0;
}

inline
void CDebuggerCommand::unlock(const void * data, size_t size)
{
	memcpy(m_data, m_data + m_offset, m_size - m_offset);
	m_size -= m_offset;
	m_offset = 0;
}

