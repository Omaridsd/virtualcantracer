#ifndef SIGNAL_SELECTION_DIALOG_H
#define SIGNAL_SELECTION_DIALOG_H

#include <QDialog>
#include <QListWidget>

class SignalSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SignalSelectionDialog(const QStringList &signalss, QWidget *parent = nullptr);
    QStringList selectedSignals() const;

private:
    QListWidget *listWidget;
};

#endif // SIGNALSELECTIONDIALOG_H
