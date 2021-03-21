#include "ComboBox.h"
#include <QComboBox>
#include <QVariant>

bool comboSelectItem(QComboBox* combo, const QVariant& value)
{
    int n = combo->count();
    for (int i = 0; i < n; i++) {
        if (combo->itemData(i) == value) {
            combo->setCurrentIndex(i);
            return true;
        }
    }
    return false;
}

QVariant comboSelectedItem(const QComboBox* combo)
{
    int selected = combo->currentIndex();
    return (selected < 0 ? QVariant() : combo->itemData(selected));
}
