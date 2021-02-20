#include "AboutDialog.h"
#include "GUI/ThirdPartyDialog.h"
#include "ui_AboutDialog.h"
#include <QMessageBox>

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)
    , mUi(new Ui_AboutDialog)
{
    mUi->setupUi(this);
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::on_aboutQtButton_clicked()
{
    QMessageBox::aboutQt(this);
}

void AboutDialog::on_thirdPartyButton_clicked()
{
    ThirdPartyDialog dlg(this);
    dlg.exec();
}
