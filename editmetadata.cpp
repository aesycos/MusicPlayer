#include "editmetadata.h"
#include "ui_editmetadata.h"

EditMetaData::EditMetaData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditMetaData)
{
    ui->setupUi(this);
}

EditMetaData::~EditMetaData()
{
    delete ui;
}
