#include <QObject>
#include <QString>
#include <QStandardItem>
#include <QIcon>

#ifndef INCLUDE_GUARD_ACA7DD5A_DAF1_4F62_AD97_1E377CC1013C
#define INCLUDE_GUARD_ACA7DD5A_DAF1_4F62_AD97_1E377CC1013C

class CProjectItem
	: public QObject
	, public QStandardItem
{
	Q_OBJECT

public:

private:

	QString	mItemName;
	QString	mItemPath;

public:

	CProjectItem(QObject *parent);

	virtual ~CProjectItem();

	bool setPath(const QString & path);

	bool setName(const QString & name);

	const QString & filePath() const;

	const QString & fileName() const;

protected:

//	virtual QVariant data(int role = Qt::UserRole + 1) const;
};

class CFolderItem
	: public CProjectItem
{
	Q_OBJECT

public:

	CFolderItem(QObject * parent)
		: CProjectItem(parent)
	{
		setIcon(QIcon(":/images/tango/folder.png"));
	}
};

class CFileItem
	: public CProjectItem
{
	Q_OBJECT

public:

	CFileItem(QObject * parent)
		: CProjectItem(parent)
	{
		setIcon(QIcon(":/images/tango/text-x-generic.png"));
	}
};

#endif // !defined(INCLUDE_GUARD_ACA7DD5A_DAF1_4F62_AD97_1E377CC1013C)
