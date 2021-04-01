#include "SettingsDialog.h"
#include "Compiler/Java/JVM.h"
#include "Compiler/CompilerError.h"
#include "GUI/Util/Conversion.h"
#include "GUI/Settings.h"
#include "ui_SettingsDialog.h"

enum class SettingsDialog::JdkError : int
{
    JvmDllNotFound,
    CompilerNotFound,
    CompilerApiNotFound,
    WrongArchitecture,
    AlreadyLoaded,
};

static const QString RED = QStringLiteral("<font color=\"red\">%1</font>");
static const QString RED2 = QStringLiteral("<font color=\"red\">%1 %2</font>");

static int selectedTab;

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
    , mUi(new Ui_SettingsDialog)
{
    mUi->setupUi(this);

    mUi->tabWidget->setCurrentIndex(selectedTab);
    mUi->jdkErrorLabel->setVisible(false);
    mUi->jdkAlreadyLoadedLabel->setVisible(false);

    Settings settings;
    mUi->jdkPathEdit->setText(settings.jdkPath);
    mUi->jdkVerboseGC->setChecked(settings.jdkVerboseGC);
    mUi->jdkVerboseClass->setChecked(settings.jdkVerboseClass);
    mUi->jdkVerboseJNI->setChecked(settings.jdkVerboseJNI);
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::done(int r)
{
    if (r == QDialog::Accepted) {
        Settings settings;
        settings.jdkPath = mUi->jdkPathEdit->text();
        settings.jdkVerboseGC = mUi->jdkVerboseGC->isChecked();
        settings.jdkVerboseClass = mUi->jdkVerboseClass->isChecked();
        settings.jdkVerboseJNI = mUi->jdkVerboseJNI->isChecked();
    }

    QDialog::done(r);
}

void SettingsDialog::on_tabWidget_currentChanged(int index)
{
    selectedTab = index;
}

void SettingsDialog::on_jdkPathEdit_textChanged(const QString&)
{
    updateJdkError();
}

void SettingsDialog::on_jdkBrowseButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Browse for JDK"), mUi->jdkPathEdit->text());
    if (path.isEmpty())
        return;

    mUi->jdkPathEdit->setText(path);
}

void SettingsDialog::on_jdkVerboseGC_toggled(bool)
{
    updateJdkError();
}

void SettingsDialog::on_jdkVerboseClass_toggled(bool)
{
    updateJdkError();
}

void SettingsDialog::on_jdkVerboseJNI_toggled(bool)
{
    updateJdkError();
}

void SettingsDialog::updateJdkError()
{
    std::filesystem::path jvmDllPath;
    std::filesystem::path javacPath;

    mUi->jdkErrorLabel->setVisible(false);
    mUi->jdkAlreadyLoadedLabel->setVisible(false);

    std::filesystem::path jdkPath = toPath(mUi->jdkPathEdit->text());

    try {
        jvmDllPath = JVM::findJvmDll(jdkPath);
    } catch (const CompilerError&) {
        setJdkError(JdkError::JvmDllNotFound);
        return;
    } catch (const std::exception&) {
        setJdkError(JdkError::JvmDllNotFound);
        return;
    }

    try {
        javacPath = JVM::findJavaC(jdkPath);
    } catch (const CompilerError&) {
        setJdkError(JdkError::CompilerNotFound);
        return;
    } catch (const std::exception&) {
        setJdkError(JdkError::CompilerNotFound);
        return;
    }

    try {
        JVM::findToolsJar(jdkPath);
    } catch (const CompilerError&) {
        setJdkError(JdkError::CompilerApiNotFound);
        return;
    } catch (const std::exception&) {
        setJdkError(JdkError::CompilerApiNotFound);
        return;
    }

  #ifdef _WIN32
    DWORD dwBinaryType = DWORD(-1);
    if (!GetBinaryTypeW(javacPath.wstring().c_str(), &dwBinaryType)) {
        setJdkError(JdkError::CompilerNotFound);
        return;
    }
    if ((sizeof(void*) == 4 && dwBinaryType != SCS_32BIT_BINARY)
        || (sizeof(void*) == 8 && dwBinaryType != SCS_64BIT_BINARY)
        || (sizeof(void*) != 4 && sizeof(void*) != 8)) {
        setJdkError(JdkError::WrongArchitecture);
        return;
    }
  #endif

    // FIXME: Linux, MacOS

    if (JVM::isLoaded()) {
        if (JVM::loadedDllPath() != jvmDllPath
                || JVM::loadedVerboseGC() != mUi->jdkVerboseGC->isChecked()
                || JVM::loadedVerboseClass() != mUi->jdkVerboseClass->isChecked()
                || JVM::loadedVerboseJNI() != mUi->jdkVerboseJNI->isChecked()
                ) {
            setJdkError(JdkError::AlreadyLoaded);
            return;
        }
    }
}

void SettingsDialog::setJdkError(JdkError error)
{
    switch (error) {
        case JdkError::JvmDllNotFound:
            mUi->jdkErrorLabel->setVisible(true);
            mUi->jdkErrorLabel->setText(RED2
                .arg(tr("JVM shared library was not found in the selected JDK."))
                .arg(tr("Please make sure the entered path points to the root of JDK installation directory.")));
            break;

        case JdkError::CompilerNotFound:
            mUi->jdkErrorLabel->setVisible(true);
            mUi->jdkErrorLabel->setText(RED2
                .arg(tr("Java compiler was not found in the selected JDK."))
                .arg(tr("Please make sure the entered path points to the root of JDK installation directory.")));
            break;

        case JdkError::CompilerApiNotFound:
            mUi->jdkErrorLabel->setVisible(true);
            mUi->jdkErrorLabel->setText(RED2
                .arg(tr("Compiler API (tools.jar) was not found in the selected JDK."))
                .arg(tr("Please make sure the entered path points to the root of JDK installation directory.")));
            break;

        case JdkError::WrongArchitecture:
            mUi->jdkErrorLabel->setVisible(true);
            mUi->jdkErrorLabel->setText(RED
                .arg(tr("Selected JDK was compiled for different processor architecture.")));
            break;

        case JdkError::AlreadyLoaded:
            mUi->jdkAlreadyLoadedLabel->setVisible(true);
            mUi->jdkAlreadyLoadedLabel->setText(RED
                .arg(tr("JVM is already loaded. Changes will only take effect after restart of the IDE.")));
            break;
    }
}
