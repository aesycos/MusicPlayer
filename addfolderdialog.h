#ifndef ADDFOLDERDIALOG_H
#define ADDFOLDERDIALOG_H

#include <QDialog>

namespace Ui {
class AddFolderDialog;
}

class AddFolderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddFolderDialog(QWidget *parent = nullptr);
    ~AddFolderDialog();

private:
    Ui::AddFolderDialog *ui;
};

#endif // ADDFOLDERDIALOG_H
