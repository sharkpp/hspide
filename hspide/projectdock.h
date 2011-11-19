#include <QTreeView>

#if defined(_MSC_VER) && 1000 < _MSC_VER
#pragma once
#endif // defined(_MSC_VER) && 1000 < _MSC_VER

class CWorkSpaceModel;
class CWorkSpaceItem;

class CProjectDock
	: public QWidget
{
	Q_OBJECT

	QTreeView* treeWidget;

public:

	CProjectDock(QWidget *parent = 0);

	// �\�����[�V����������
	bool setWorkSpace(CWorkSpaceModel * workspace);

	// �A�C�e����I��
	void selectItem(CWorkSpaceItem * item);

	// ���݂̃A�C�e�����擾
	CWorkSpaceItem * currentItem();

	// ���݂̃\�����[�V�������擾
	CWorkSpaceItem * currentSolution();

	// ���݂̃v���W�F�N�g���擾
	CWorkSpaceItem * currentProject();

	// ���݂̃t�@�C�����擾
	CWorkSpaceItem * currentFile();


protected:

	virtual void resizeEvent(QResizeEvent * event);

public slots:

	void doubleClickedTree(const QModelIndex & inde);

signals:

	void oepnItem(CWorkSpaceItem * item);

private:

};
