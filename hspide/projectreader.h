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
	QStringList mVirtualPath;

	CProject* mProject;

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

		mProject = &project;
		mProject->removeRows(0, mProject->rowCount());

		mVirtualPath.clear();
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

		// swapping project
		//for(int row = 0, rowNum = mProject->rowCount();
		//	row < rowNum; row++)
		//{
		//	project.appendRow(mProject.takeRow(row));
		//}

		return true;
	}

protected:

 // QXmlDefaultHandler class method override

	bool startElement(const QString &namespaceURI, const QString &localName,
	                  const QString &qName, const QXmlAttributes &attributes)
	{
		if( !qName.compare("project", Qt::CaseSensitive) )
		{
			// �P���x���� or �v���W�F�N�g����ł͂Ȃ�������
			// XML�\�������������̂ŃG���[
			if( !mXmlDomStack.isEmpty() ||
				mProject->rowCount() )
			{
				return false;
			}

			// ��������v���W�F�N�g�����擾
			int idxAttr;
			QString sName = 0 <= (idxAttr = attributes.index("name"))
							? attributes.value(idxAttr)
							: QString();

			mProject->setText(sName);
		}
		else if( !qName.compare("folder", Qt::CaseSensitive) )
		{
			// XML�\�������������̂ŃG���[
			if( mXmlDomStack.empty() ||
				!mXmlDomStack.back().compare("file", Qt::CaseSensitive) )
			{
				return false;
			}

			QString sBasePath = mVirtualPath.join("/");

			// ��������t�H���_�����擾
			int idxAttr;
			QString sName = 0 <= (idxAttr = attributes.index("name"))
							? attributes.value(idxAttr)
							: QString();

			if( !sName.isEmpty() )
			{
				mProject->append(sName, sBasePath, true);
			}

			mVirtualPath.push_back(sName);
		}
		else if( !qName.compare("file", Qt::CaseSensitive) )
		{
			// XML�\�������������̂ŃG���[
			if( mXmlDomStack.empty() ||
				!mXmlDomStack.back().compare("file", Qt::CaseSensitive) )
			{
				return false;
			}

			QString sBasePath = mVirtualPath.join("/");

			// ��������t�@�C���p�X���擾
			int idxAttr;
			QString sPath = 0 <= (idxAttr = attributes.index("path"))
							? attributes.value(idxAttr)
							: QString();

			if( !sPath.isEmpty() )
			{
				mProject->append(sPath, sBasePath);
			}
		}

		mXmlDomStack.push_back(qName);

		return true;
	}

	bool endElement(const QString &namespaceURI, const QString &localName,
	                const QString &qName)
	{
		if( !qName.compare("folder", Qt::CaseSensitive) )
		{
			mVirtualPath.pop_back();
		}

		mXmlDomStack.pop_back();

		return true;
	}

	bool characters(const QString &str)
	{
		return true;
	}

	bool fatalError(const QXmlParseException &exception)
	{
		return true;
	}
};

#endif // !defined(INCLUDE_GUARD_DC5B32EB_8282_4EC7_956E_75D397D44EFA)
