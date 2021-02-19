#include "ThirdPartyDialog.h"
#include "ui_ThirdPartyDialog.h"

ThirdPartyDialog::ThirdPartyDialog(QWidget* parent)
    : QDialog(parent)
    , mUi(new Ui_ThirdPartyDialog)
{
    mUi->setupUi(this);
    mUi->textView->setHtml(QStringLiteral(
        "<html><body>"

        "</body></html>"
        ));
}

ThirdPartyDialog::~ThirdPartyDialog()
{
}
