#include <QObject>
#include <QString>
#include <QVector>
#include <QProcess>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

#ifndef INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29
#define INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29

class CWorkSpaceItem;

class CCompiler
	: public QObject
{
	Q_OBJECT

	QString m_hspCompPath;		// HSP�R���p�C��DLL�ւ̃p�X
	QString m_hspPath;			// HSP�C���X�g�[���f�B���N�g���ւ̃p�X
	QString m_hspCommonPath;	// HSP Common�t�H���_�ւ̃p�X

	QString m_buildAfterRunArgs;		// �r���h��Ɏ��s���s�����H

	QProcess *m_compilerProcess;	// �R���p�C���v���Z�X
	QProcess *m_listingSymbolsProcess;	// �V���{���擾�v���Z�X

	QVector<QStringList> m_highlightSymbols;	// �擾�����V���{���̈ꗗ

public:

	CCompiler(QObject *parent = 0);

	// �V���{���ꗗ���擾
	const QVector<QStringList> & symbols() const;

	// �R���p�C���̃p�X���擾
	const QString &  compilerPath() const;
	// �R���p�C���̃p�X���w��
	void setCompilerPath(const QString & path);

	// HSP�f�B���N�g���̃p�X���擾
	const QString &  hspPath() const;
	// HSP�f�B���N�g���̃p�X���w��
	void setHspPath(const QString & path);

	// HSP�f�B���N�g���̃p�X���擾
	const QString &  hspCommonPath() const;
	// HSP common�f�B���N�g���̃p�X���w��
	void setHspCommonPath(const QString & path);

	// �r���h
	bool build(CWorkSpaceItem * targetItem);

	// ���s
	bool run(CWorkSpaceItem * targetItem);

	// �P��t�@�C�����R���p�C��
	void compile();

protected:

	void updateCompilerPath();

	bool execCompiler(CWorkSpaceItem * targetItem, bool buildAfterRun);

public slots:

	// �V���{���̎擾����
	void listedSymbolsFinished(int exitCode, QProcess::ExitStatus exitStatus);

	// �v���W�F�N�g�̃r���h����
	void buildFinished(int exitCode, QProcess::ExitStatus exitStatus);

	// �r���h���̏o�͂��擾
	void buildReadOutput();

signals:

	void buildStart(const QString & filePath);
	void buildFinished(bool status);
	void buildOutput(const QString & output);

};

#endif // !defined(INCLUDE_GUARD_54165F25_45DB_4022_A837_202A7B98EE29)
