#include "MemoryMapWidget.h"
#include "Compiler/Linker/CompiledOutput.h"
#include "Compiler/Linker/DebugInformation.h"
#include "GUI/Util/Conversion.h"

MemoryMapWidget::MemoryMapWidget(QWidget* parent)
    : QTreeWidget(parent)
{
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    setDropIndicatorShown(false);
    setDragEnabled(false);
    setDragDropOverwriteMode(false);
    setDragDropMode(QAbstractItemView::NoDragDrop);
    setDefaultDropAction(Qt::IgnoreAction);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setRootIsDecorated(true);
    setUniformRowHeights(true);
    setItemsExpandable(true);
    setSortingEnabled(false);
    setWordWrap(false);
    setExpandsOnDoubleClick(true);

    setColumnCount(2);
    setColumnWidth(0, 350);
    setHeaderItem(new QTreeWidgetItem(QStringList() << tr("Name") << tr("Address")));

    auto hdr = header();
    hdr->setVisible(true);
    hdr->setDefaultSectionSize(400);
    hdr->setStretchLastSection(true);
}

MemoryMapWidget::~MemoryMapWidget()
{
}

static QString makeRange(uint64_t start, int64_t uncompressedSize, std::optional<int64_t> compressedSize = {})
{
    int64_t size = (compressedSize ? *compressedSize : uncompressedSize);
    int64_t end = start + size;
    if (size > 0)
        end -= 1;

    QString sizeString;
    if (!compressedSize) {
        sizeString = QApplication::tr("%1 byte(s)", nullptr, int(uncompressedSize))
            .arg(uncompressedSize);
    } else {
        sizeString = QApplication::tr("%2 / %1 byte(s)", nullptr, int(*compressedSize))
            .arg(*compressedSize).arg(uncompressedSize);
    }

    return QStringLiteral("0x%1/%2 .. 0x%3/%4  %5")
        .arg(QStringLiteral("%1").arg(start, 4, 16, QChar('0')).toUpper())
        .arg(start, -5, 10, QChar(' '))
        .arg(QStringLiteral("%1").arg(end, 4, 16, QChar('0')).toUpper())
        .arg(end, -5, 10, QChar(' '))
        .arg(sizeString);
}

void MemoryMapWidget::setData(CompiledOutput* linkerOutput)
{
    clear();

    QFont headerFont = font();
    headerFont.setBold(true);

    for (CompiledFile* file : linkerOutput->files()) {
        QTreeWidgetItem* fileItem = new QTreeWidgetItem(this,
            QStringList() << fromUtf8(file->name()) << makeRange(file->loadAddress(), file->size()));
        fileItem->setFont(0, headerFont);
        fileItem->setFont(1, headerFont);
        fileItem->setExpanded(true);

        auto debugInfo = file->debugInfo();
        for (const auto& section : debugInfo->sections()) {
            std::optional<int64_t> compressedSize;
            if (section.compression != Compression::None)
                compressedSize = section.compressedSize;

            QTreeWidgetItem* sectionItem = new QTreeWidgetItem(fileItem, QStringList()
                << fromUtf8(section.name)
                << makeRange(section.startAddress, section.uncompressedSize, compressedSize));
            (void)sectionItem;
        }
    }
}
