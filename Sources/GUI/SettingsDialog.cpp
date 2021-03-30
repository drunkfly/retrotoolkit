#include "SettingsDialog.h"
#include "Compiler/Java/JVM.h"
#include "Compiler/CompilerError.h"
#include "GUI/Util/Conversion.h"
#include "GUI/Settings.h"
#include "ui_SettingsDialog.h"

static int selectedTab;
static const QString red = QStringLiteral("<font color=\"red\">%1</font>");

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
    , mUi(new Ui_SettingsDialog)
{
    mUi->setupUi(this);
    mUi->tabWidget->setCurrentIndex(selectedTab);

    mUi->jdkDllNotFoundLabel->setVisible(false);
    mUi->jdkDllNotFoundLabel->setText(red.arg(mUi->jdkDllNotFoundLabel->text()));
    mUi->jdkCompilerNotFoundLabel->setVisible(false);
    mUi->jdkCompilerNotFoundLabel->setText(red.arg(mUi->jdkCompilerNotFoundLabel->text()));
    mUi->jdkWrongArchitectureLabel->setVisible(false);
    mUi->jdkWrongArchitectureLabel->setText(red.arg(mUi->jdkWrongArchitectureLabel->text()));
    mUi->jdkReloadNoticeLabel->setVisible(false);
    mUi->jdkReloadNoticeLabel->setText(red.arg(mUi->jdkReloadNoticeLabel->text()));

    Settings settings;
    mUi->jdkPathEdit->setText(settings.jdkPath);
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::done(int r)
{
    if (r == QDialog::Accepted) {
        Settings settings;
        settings.jdkPath = mUi->jdkPathEdit->text();
    }

    QDialog::done(r);
}

void SettingsDialog::on_tabWidget_currentChanged(int index)
{
    selectedTab = index;
}

void SettingsDialog::on_jdkPathEdit_textChanged(const QString& newText)
{
    std::filesystem::path jvmDllPath;
    std::filesystem::path javacPath;

    mUi->jdkDllNotFoundLabel->setVisible(false);
    mUi->jdkCompilerNotFoundLabel->setVisible(false);
    mUi->jdkWrongArchitectureLabel->setVisible(false);
    mUi->jdkReloadNoticeLabel->setVisible(false);

    try {
        jvmDllPath = JVM::findJvmDll(toPath(newText));
    } catch (const CompilerError&) {
        mUi->jdkDllNotFoundLabel->setVisible(true);
        return;
    } catch (const std::exception&) {
        mUi->jdkDllNotFoundLabel->setVisible(true);
        return;
    }

    try {
        javacPath = JVM::findJavaC(toPath(newText));
    } catch (const CompilerError&) {
        mUi->jdkCompilerNotFoundLabel->setVisible(true);
        return;
    } catch (const std::exception&) {
        mUi->jdkCompilerNotFoundLabel->setVisible(true);
        return;
    }

  #ifdef _WIN32
    DWORD dwBinaryType = DWORD(-1);
    if (!GetBinaryTypeW(javacPath.wstring().c_str(), &dwBinaryType)) {
        mUi->jdkCompilerNotFoundLabel->setVisible(true);
        return;
    }
    if ((sizeof(void*) == 4 && dwBinaryType != SCS_32BIT_BINARY)
        || (sizeof(void*) == 8 && dwBinaryType != SCS_64BIT_BINARY)
        || (sizeof(void*) != 4 && sizeof(void*) != 8)) {
        mUi->jdkWrongArchitectureLabel->setVisible(true);
        return;
    }
  #endif

    // FIXME: Linux, MacOS

    mUi->jdkReloadNoticeLabel->setVisible(JVM::isLoaded() && JVM::loadedDllPath() != jvmDllPath);
}

void SettingsDialog::on_jdkBrowseButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Browse for JDK"), mUi->jdkPathEdit->text());
    if (path.isEmpty())
        return;

    mUi->jdkPathEdit->setText(path);
}
