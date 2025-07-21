#include "signal_selection_dialog.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>

SignalSelectionDialog::SignalSelectionDialog(const QStringList &signalss, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Sélectionner les signaux");
    listWidget = new QListWidget(this);
    listWidget->setSelectionMode(QAbstractItemView::MultiSelection);

    for (const QString &signal : signalss) {
        QListWidgetItem *item = new QListWidgetItem(signal, listWidget);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // permet le checkbox
        item->setCheckState(Qt::Unchecked); // ou Qt::Checked par défaut si tu veux
    }

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(listWidget);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QStringList SignalSelectionDialog::selectedSignals() const
{
    QStringList selected;
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);
        if (item->checkState() == Qt::Checked) {
            selected << item->text();
        }
    }
    return selected;
}

