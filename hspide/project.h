#include "projectitem.h"

#ifndef INCLUDE_GUARD_DE12DB44_EA81_45D1_9E18_9CCA26479DE7
#define INCLUDE_GUARD_DE12DB44_EA81_45D1_9E18_9CCA26479DE7

class CEditor;

class CProject
	: public CProjectItem
{
	Q_OBJECT

private:


public:

	CProject(QObject *parent);

	virtual ~CProject();

	// �v���W�F�N�g��ǂݍ���
	bool load(const QString & filename);

	// �v���W�F�N�g��ۑ�
	bool save(const QString & filename = QString());

	// �v���W�F�N�g�Ƀt�@�C����ǉ�
	bool append(const QString & filename, const QString & path = QString());

	// �v���W�F�N�g���珜�O
	bool remove(const QString & filename);

	// �v���W�F�N�g�փG�f�B�^���֘A�t��
	bool openFile(CEditor* editor);

	// �v���W�F�N�g���̃t�@�C�������
	bool closeFile(CEditor* editor);

	// �v���W�F�N�g���̎擾
	QString name() const;

	// �t�H���_���擾
	CProjectItem * getFolderItem(const QString & path, bool createAlways = false);

private:

	// �v���W�F�N�g���̃t�@�C�����ꎞ�I�ɕۑ�
	bool saveTemp();

	// �v���W�F�N�g���̈ꎟ�ۑ��t�@�C�����N���A
	bool clearTemp();

};

#endif // !defined(INCLUDE_GUARD_DE12DB44_EA81_45D1_9E18_9CCA26479DE7)
