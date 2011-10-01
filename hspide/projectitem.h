#include <QObject>
#include <QString>
#include <QVector>
#include <QStandardItem>

#ifndef INCLUDE_GUARD_ACA7DD5A_DAF1_4F62_AD97_1E377CC1013C
#define INCLUDE_GUARD_ACA7DD5A_DAF1_4F62_AD97_1E377CC1013C

class CProjectItem
	: public QObject
	, public QStandardItem
{
	Q_OBJECT

public:

private:

//	QVector<CProjectItem*>	mChildren;

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

	virtual QVariant data(int role = Qt::UserRole + 1) const;
};

class CFolderItem
	: public CProjectItem
{
	Q_OBJECT

public:

	CFolderItem(QObject * parent) : CProjectItem(parent) {}
};

class CFileItem
	: public CProjectItem
{
	Q_OBJECT

public:

	CFileItem(QObject * parent) : CProjectItem(parent) {}
};

#endif // !defined(INCLUDE_GUARD_ACA7DD5A_DAF1_4F62_AD97_1E377CC1013C)
