#ifndef EDITMETADATA_H
#define EDITMETADATA_H

#include <QDialog>

namespace Ui {
class EditMetaData;
}

class EditMetaData : public QDialog
{
    Q_OBJECT

public:
    explicit EditMetaData(QWidget *parent = nullptr);
    ~EditMetaData();

private:
    Ui::EditMetaData *ui;
};

#endif // EDITMETADATA_H
