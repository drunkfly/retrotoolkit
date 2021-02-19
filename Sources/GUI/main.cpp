#include "MainWindow.h"
#include <QApplication>

int main(int argc, char** argv)
{
    QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);

    QApplication::setOrganizationName(QStringLiteral("DrunkFly"));
    QApplication::setOrganizationDomain(QStringLiteral("drunkfly.team"));
    QApplication::setApplicationName(QStringLiteral("ZX Toolkit"));
    QApplication::setApplicationVersion(QStringLiteral("1.0"));

    QApplication app(argc, argv);
    MainWindow mainWindow;

    mainWindow.show();
    return app.exec();
}
