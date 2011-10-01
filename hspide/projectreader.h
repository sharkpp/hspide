#include <QXmlDefaultHandler>
#include <QSharedPointer>
#include <QString>
#include <QMap>

#ifndef INCLUDE_GUARD_DC5B32EB_8282_4EC7_956E_75D397D44EFA
#define INCLUDE_GUARD_DC5B32EB_8282_4EC7_956E_75D397D44EFA

class CProjectReader
	: public QXmlDefaultHandler
{
	QVector<QString> mXmlDomStack;

	CProject mProject;

public:

	CProjectReader()
		: mProject(NULL)
	{
	}

	virtual ~CProjectReader()
	{
	}

	bool load(const QString & filename, CProject & project)
	{
		QXmlSimpleReader reader;
		reader.setContentHandler(this);
		reader.setErrorHandler(this);

		mProject.removeRows(0, mProject.rowCount());

		QFile file(filename);
		if( !file.open(QFile::ReadOnly | QFile::Text) )
		{
			return false;
		}

		QXmlInputSource xmlInputSource(&file);
		if( !reader.parse(xmlInputSource) )
		{
			return false;
		}

		// swapping project
		for(int row = 0, rowNum = mProject.rowCount();
			row < rowNum; row++)
		{
			project.insertRow(row, mProject.takeRow(row));
		}

		return true;
	}

protected:

 // QXmlDefaultHandler class method override

	bool startElement(const QString &namespaceURI, const QString &localName,
	                  const QString &qName, const QXmlAttributes &attributes)
	{
		qDebug() << __FUNCTION__ << "(" << namespaceURI << "," << localName << "," << qName << ")";

		if( !qName.compare("project", Qt::CaseSensitive) )
		{
			// １レベル目 or プロジェクトが空ではなかったら
			// XML構造がおかしいのでエラー
			if( !mXmlDomStack.empty() ||
				!mProject.rowCount() )
			{
				return false;
			}
		}
		else if( !qName.compare("folder", Qt::CaseSensitive) )
		{
			// XML構造がおかしいのでエラー
			if( mXmlDomStack.empty() ||
				!mXmlDomStack.back().compare("file", Qt::CaseSensitive) )
			{
				return false;
			}

		}
		else if( !qName.compare("file", Qt::CaseSensitive) )
		{
			// XML構造がおかしいのでエラー
			if( mXmlDomStack.empty() ||
				!mXmlDomStack.back().compare("file", Qt::CaseSensitive) )
			{
				return false;
			}

		}

		mXmlDomStack.push_back(qName);

		return true;
	}

	bool endElement(const QString &namespaceURI, const QString &localName,
	                const QString &qName)
	{
		qDebug() << __FUNCTION__ << "(" << namespaceURI << "," << localName << "," << qName << ")";

		mXmlDomStack.pop_back();

		return true;
	}

	bool characters(const QString &str)
	{
		qDebug() << __FUNCTION__ << "(" << str << ")";
		return true;
	}

	bool fatalError(const QXmlParseException &exception)
	{
		return true;
	}
};

#endif // !defined(INCLUDE_GUARD_DC5B32EB_8282_4EC7_956E_75D397D44EFA)
