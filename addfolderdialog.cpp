#include "addfolderdialog.h"
#include "ui_addfolderdialog.h"

AddFolderDialog::AddFolderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFolderDialog)
{
    ui->setupUi(this);
}

AddFolderDialog::~AddFolderDialog()
{
    delete ui;
}
