#ifndef SCHEDULETABWIDGET_H
#define SCHEDULETABWIDGET_H

#include <QDialog>
#include <QItemSelection>
#include <QItemSelectionModel>

#include "Match.h"

namespace Ui {
class ScheduleTabWidget;
}

using namespace QTournament;

class ScheduleTabWidget : public QDialog
{
    Q_OBJECT

public:
    explicit ScheduleTabWidget(QWidget *parent = 0);
    ~ScheduleTabWidget();

public slots:
    void onBtnStageClicked();
    void onBtnUnstageClicked();
    void onBtnScheduleClicked();
    void onIdleSelectionChanged(const QItemSelection &, const QItemSelection &);
    void onStagedSelectionChanged(const QItemSelection &, const QItemSelection &);
    void onMatchDoubleClicked(const QModelIndex& index);
    void onCourtDoubleClicked(const QModelIndex& index);

private:
    Ui::ScheduleTabWidget *ui;
    void updateButtons();
    void askAndStoreMatchResult(const Match& ma);
};

#endif // SCHEDULETABWIDGET_H
