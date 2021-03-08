#ifndef GUI_WIDGETS_MEMORYMAPWIDGET_H
#define GUI_WIDGETS_MEMORYMAPWIDGET_H

#include "Common/Common.h"
#include <QTreeWidget>

class CompiledOutput;

class MemoryMapWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit MemoryMapWidget(QWidget* parent = nullptr);
    ~MemoryMapWidget() override;

    void setData(CompiledOutput* linkerOutput);

private:
    DISABLE_COPY(MemoryMapWidget);
};

#endif
