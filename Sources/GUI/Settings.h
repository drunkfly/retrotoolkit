#ifndef GUI_SETTINGS_H
#define GUI_SETTINGS_H

#include "Common/Common.h"
#include <QSettings>

class Settings
{
public:
    template <typename T> class Property
    {
    public:
        Property(QSettings& settings, const QString& name) : mSettings(settings), mName(name) {}

        void operator=(const T& value) const { mSettings.setValue(mName, value); }
        operator T() const { return mSettings.value(mName).value<T>(); }

    private:
        QSettings& mSettings;
        QString mName;

        DISABLE_COPY(Property);
    };

    #define PROPERTY(TYPE, NAME) Property<TYPE> NAME{mSettings, QStringLiteral(#NAME)}

    PROPERTY(bool, loadLastProjectOnStart);
    PROPERTY(QString, lastProjectFile);

    Settings();
    ~Settings();

private:
    QSettings mSettings;

    Q_DISABLE_COPY(Settings)
    #undef PROPERTY
};

#endif
