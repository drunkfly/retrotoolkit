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
        Property(QSettings& settings, const QString& name, const T& initialValue)
            : mSettings(settings)
            , mName(name)
            , mInitialValue(initialValue)
        {
        }

        void operator=(const T& value) const { mSettings.setValue(mName, value); }
        operator T() const { return mSettings.value(mName, mInitialValue).template value<T>(); }

    private:
        QSettings& mSettings;
        QString mName;
        T mInitialValue;

        DISABLE_COPY(Property);
    };

    #define PROPERTY(TYPE, NAME, DEFAULT) Property<TYPE> NAME{mSettings, QStringLiteral(#NAME), DEFAULT}

    PROPERTY(bool, loadLastProjectOnStart, true);
    PROPERTY(QString, lastProjectFile, QString());

    PROPERTY(QString, jdkPath, QString());

    Settings();
    ~Settings();

private:
    QSettings mSettings;

    DISABLE_COPY(Settings);
    #undef PROPERTY
};

#endif
