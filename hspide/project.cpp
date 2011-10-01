#include <QtDebug>
#include <QFileInfo>
#include "project.h"
#include "projectitem.h"
#include "editor.h"
#include "projectreader.h"

/////////////////////////////////////////////////////////////////////

CProject::CProject(QObject *parent)
	: CProjectItem(parent)
{
}

CProject::~CProject()
{
}

// �v���W�F�N�g��ǂݍ���
bool CProject::load(const QString & filename)
{
	CProjectReader reader;
	return reader.load(filename, *this);
}

// �v���W�F�N�g��ۑ�
bool CProject::save(const QString & filename)
{
	return false;
}

// �t�H���_���擾
CProjectItem * CProject::getFolderItem(const QString & path, bool createAlways)
{
	CProjectItem *item = this;

	for(QStringListIterator ite(path.split("/"));
		ite.hasNext(); )
	{
		CFolderItem * folderItem = NULL;
		QString name = ite.next();

		for(int row = 0, rowNum = item->rowCount();
			row < rowNum; row++, folderItem = NULL)
		{
			if( NULL != (folderItem = dynamic_cast<CFolderItem*>(item->child(row))) &&
				!folderItem->fileName().compare(name, Qt::CaseSensitive) )
			{
				break;
			}
		}

		if( folderItem )
		{
			item = folderItem;
		}
		else if( createAlways )
		{
			folderItem = new CFolderItem(this);
			folderItem->setName(name);
			item->insertRow(rowCount(), folderItem);
			item = folderItem;
		}
		else
		{
			return NULL;
		}
	}

	return item;
}

// �v���W�F�N�g�Ƀt�@�C����ǉ�
bool CProject::append(const QString & filename, const QString & path)
{
	CProjectItem *folder = getFolderItem(path);
	if( !folder ) {
		return false;
	}

	CProjectItem *item = new CProjectItem(this);
	item->setPath(filename);

	folder->insertRow(rowCount(), item);
	return true;
}

// �v���W�F�N�g���珜�O
bool CProject::remove(const QString & filename)
{
	return false;
}

// �v���W�F�N�g�փG�f�B�^���֘A�t��
bool CProject::openFile(CEditor* editor)
{
//	editor->filename()

	return false;
}

// �v���W�F�N�g���̃t�@�C�������
bool CProject::closeFile(CEditor* editor)
{
	return false;
}

// �v���W�F�N�g���̃t�@�C�����ꎞ�I�ɕۑ�
bool CProject::saveTemp()
{
	return false;
}

// �v���W�F�N�g���̈ꎟ�ۑ��t�@�C�����N���A
bool CProject::clearTemp()
{
	return false;
}

// �v���W�F�N�g���̎擾
QString CProject::name() const
{
	return QFileInfo(filePath()).baseName();
}
