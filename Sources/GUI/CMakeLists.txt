
if(NOT EXCLUDE_GUI)

add(RetroToolkit
    QT_EXECUTABLE
    USES
        Core
        Gui
        Multimedia
        Network
        Widgets
    LIBS
        Common
        Compiler
        EmulatorCore
        RunTests
    SOURCES
        Build/BuildDialog.cpp
        Build/BuildDialog.h
        Build/BuildDialog.ui
        Build/BuildThread.cpp
        Build/BuildThread.h
        Build/OutputProxy.cpp
        Build/OutputProxy.h
        Resources/resources.qrc
        Util/ComboBox.cpp
        Util/ComboBox.h
        Util/Conversion.cpp
        Util/Conversion.h
        Util/Exception.cpp
        Util/Exception.h
        Util/Localization.cpp
        Util/Localization.h
        Widgets/BuildStatusLabel.cpp
        Widgets/BuildStatusLabel.h
        Widgets/LogWidget.cpp
        Widgets/LogWidget.h
        Widgets/MemoryMapWidget.cpp
        Widgets/MemoryMapWidget.h
        AboutDialog.cpp
        AboutDialog.h
        AboutDialog.ui
        Common.h
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui
        PlayAudioDialog.cpp
        PlayAudioDialog.h
        PlayAudioDialog.ui
        Settings.cpp
        Settings.h
        SettingsDialog.cpp
        SettingsDialog.h
        SettingsDialog.ui
        ThirdPartyDialog.cpp
        ThirdPartyDialog.h
        ThirdPartyDialog.ui
        main.cpp
    )

target_precompile_headers(RetroToolkit
    PRIVATE
        [["GUI/Common.h"]]
    )

endif()
