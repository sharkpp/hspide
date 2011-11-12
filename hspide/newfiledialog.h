#include "ui_newfiledialog.h"
#include <QFileSystemModel>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CNameOnlyFileSystemModel
	: public QFileSystemModel
{
public:
	CNameOnlyFileSystemModel(QObject * parent = 0);
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
};

class CNewFileDialog
	: public QDialog
{
	Q_OBJECT

	Ui::NewFileDialog ui;

	QString m_filePath;
	QString m_templateFilePath;

public:

	CNewFileDialog(QWidget *parent = 0);

	const QString & filePath() const;
	const QString & templateFilePath() const;

protected:

public slots:

	void onSelectFileName();
	void onChangeTemplate(const QModelIndex & index);
	void onFileNameChanged(const QString & text);

private:

	void setupTemplateList();
};

inline
const QString & CNewFileDialog::filePath() const
{
	return m_filePath;
}

inline
const QString & CNewFileDialog::templateFilePath() const
{
	return m_templateFilePath;
}
