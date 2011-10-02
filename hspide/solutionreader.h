#include <QXmlDefaultHandler>
#include <QSharedPointer>
#include <QString>
#include <QMap>
#include <QDataStream>
#include <QByteArray>
#include "project.h"

#ifndef INCLUDE_GUARD_EADFE3B9_3FA4_4DA3_BE5D_A6C644A9A637
#define INCLUDE_GUARD_EADFE3B9_3FA4_4DA3_BE5D_A6C644A9A637

class CSolutionReader
	: public QXmlDefaultHandler
{
	QVector<QString> mXmlDomStack;

	CSolution *mSolution;

public:

	CSolutionReader()
		: mSolution(NULL)
	{
	}

	virtual ~CSolutionReader()
	{
	}

	bool load(const QString & filename, CSolution & solution)
	{
		QXmlSimpleReader reader;
		reader.setContentHandler(this);
		reader.setErrorHandler(this);

		mSolution = &solution;
		mSolution->removeRows(0, mSolution->rowCount());

		mXmlDomStack.clear();

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

		// swapping solution
		//for(int row = 0, rowNum = mSolution.rowCount();
		//	row < rowNum; row++)
		//{
		//	solution.appendRow(mSolution.child(row));
		//}
		//solution.setText(mSolution.text());
	//	QByteArray tmp0;
	//	QDataStream tmp(&tmp0, QIODevice::ReadWrite);
	//	mSolution.write(tmp);
	//	solution.read(tmp);

		return true;
	}

protected:

 // QXmlDefaultHandler class method override

	bool startElement(const QString &namespaceURI, const QString &localName,
	                  const QString &qName, const QXmlAttributes &attributes)
	{
		qDebug() << __FUNCTION__ << "(" << namespaceURI << "," << localName << "," << qName << ")";

		if( !qName.compare("solution", Qt::CaseSensitive) )
		{
			// １レベル目 or プロジェクトが空ではなかったら
			// XML構造がおかしいのでエラー
			if( !mXmlDomStack.isEmpty() ||
				mSolution->rowCount() )
			{
				return false;
			}

			// 属性からプロジェクト名を取得
			int idxAttr;
			QString sName = 0 <= (idxAttr = attributes.index("name"))
							? attributes.value(idxAttr)
							: QString();

			mSolution->setText(sName);
		}
		else if( !qName.compare("project", Qt::CaseSensitive) )
		{
			// ２レベル目以降 or Solutionの直下では無い場合は
			// XML構造がおかしいのでエラー
			if( 1 < mXmlDomStack.size() ||
				mXmlDomStack.back().compare("solution", Qt::CaseSensitive) )
			{
				return false;
			}

			// 属性からプロジェクト名を取得
			int idxAttr;
			QString sPath = 0 <= (idxAttr = attributes.index("path"))
							? attributes.value(idxAttr)
							: QString();

			if( !sPath.isEmpty() )
			{
				mSolution->append(sPath);
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

#endif // !defined(INCLUDE_GUARD_EADFE3B9_3FA4_4DA3_BE5D_A6C644A9A637)
