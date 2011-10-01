//#include <QObject>
#include <QString>
#include <QMap>
#include <QSharedPointer>
//#include <QAbstractItemModel>
#include "projectitem.h"

#ifndef INCLUDE_GUARD_3CBB7292_EDC6_4258_B45F_54C680152079
#define INCLUDE_GUARD_3CBB7292_EDC6_4258_B45F_54C680152079

class CProject;
class CEditor;

class CSolution
	: public CProjectItem
{
	Q_OBJECT

public:

	CSolution(QObject *parent);

	virtual ~CSolution();

	// �\�����[�V������ǂݍ���
	bool load(const QString & filename);

	// �\�����[�V������ۑ�
	bool save(const QString & filename);

	// �\�����[�V�����Ƀv���W�F�N�g��ǉ�
	CProject * append(const QString & filename = QString());

	// �\�����[�V��������v���W�F�N�g�����O
	bool remove(const QString & filename);

	// �\�����[�V�����փG�f�B�^���֘A�t��
	bool openFile(CEditor* editor);

	// �\�����[�V��������G�f�B�^������
	bool closeFile(CEditor* editor);

	// �\�����[�V���������擾
	QString title() const;

	// �\�����[�V���������擾
	QString filename() const;

	// �v���W�F�N�g�����擾
	int count() const;

	// �v���W�F�N�g���擾
	const CProject & at(int index) const;

private:

};

#endif // !defined(INCLUDE_GUARD_3CBB7292_EDC6_4258_B45F_54C680152079)
