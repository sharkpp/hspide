#include <QObject>
#include <QString>
#include <QVector>
#include <QStandardItem>

#ifndef INCLUDE_GUARD_ACA7DD5A_DAF1_4F62_AD97_1E377CC1013C
#define INCLUDE_GUARD_ACA7DD5A_DAF1_4F62_AD97_1E377CC1013C

class CProjectItem
	: public QStandardItem
{
public:

private:

//	QVector<CProjectItem*>	mChildren;

	QString	mItemName;
	QString	mItemPath;

public:

	CProjectItem(QObject *parent);

	virtual ~CProjectItem();

	//int getRow(const CProjectItem & item) const;

	//CProjectItem * child(int row);

	//int rowCount() const;

	//bool append(CProjectItem * item);

	bool setPath(const QString & path);

	const QString & filePath() const;

	const QString & fileName() const;

protected:

	virtual QVariant data(int role = Qt::UserRole + 1) const;
};

#endif // !defined(INCLUDE_GUARD_ACA7DD5A_DAF1_4F62_AD97_1E377CC1013C)
