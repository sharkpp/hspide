#include <QTreeView>
#include <QList>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CWorkSpaceModel;
class CWorkSpaceItem;

class CWorkSpaceDock
	: public QWidget
{
	Q_OBJECT

	QTreeView* treeWidget;

public:

	CWorkSpaceDock(QWidget* parent = 0);

	// �\�����[�V����������
	bool setWorkSpace(CWorkSpaceModel* workspace);

	// �A�C�e����I��
	void selectItem(CWorkSpaceItem* item);

	// ���݂̃A�C�e�����擾
	CWorkSpaceItem* currentItem();

	// ���݂̃\�����[�V�������擾
	CWorkSpaceItem* currentSolution();

	// ���݂̃v���W�F�N�g���擾
	CWorkSpaceItem* currentProject();

	// �v���W�F�N�g���擾
	QList<CWorkSpaceItem*> projects();

	// ���݂̃t�@�C�����擾
	CWorkSpaceItem* currentFile();

	QTreeView* tree();

protected:

	virtual void resizeEvent(QResizeEvent* event);

protected slots:

	void doubleClickedTree(const QModelIndex& inde);
	void expandedTree(const QModelIndex& index);

signals:

	void oepnItem(CWorkSpaceItem* item);

private:

};

inline
QTreeView* CWorkSpaceDock::tree()
{
	return treeWidget;
}