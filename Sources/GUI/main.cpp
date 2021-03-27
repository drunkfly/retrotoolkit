#include "GUI/MainWindow.h"
#include <string>
#include <QApplication>

Q_DECLARE_METATYPE(std::string);

int main(int argc, char** argv)
{
    QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);

    QApplication::setOrganizationName(QStringLiteral("DrunkFly"));
    QApplication::setOrganizationDomain(QStringLiteral("drunkfly.team"));
    QApplication::setApplicationName(QStringLiteral("RetroToolkit"));
    QApplication::setApplicationVersion(QStringLiteral("1.0"));

    qRegisterMetaType<std::string>();

    QApplication app(argc, argv);
    MainWindow mainWindow;

    auto args = app.arguments();
    if (args.length() > 1)
        mainWindow.openProject(args[1], false);
    else
        mainWindow.openLastProject();

    mainWindow.show();
    return app.exec();
}
