#include <QMessageBox>

#include "ScheduleTabWidget.h"
#include "ui_ScheduleTabWidget.h"
#include "GuiHelpers.h"
#include "Score.h"
#include "ui/DlgMatchResult.h"

ScheduleTabWidget::ScheduleTabWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScheduleTabWidget)
{
    ui->setupUi(this);

    // initialize sorting and filtering in the two match group views
    ui->mgIdleView->setFilter(MatchGroupTableView::FilterType::IDLE);
    ui->mgStagedView->setFilter(MatchGroupTableView::FilterType::STAGED);
    ui->mgStagedView->sortByColumn(MatchGroupTableModel::STAGE_SEQ_COL_ID, Qt::AscendingOrder);

    // react on selection changes in the IDLE match groups view
    connect(ui->mgIdleView->selectionModel(),
      SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
      SLOT(onIdleSelectionChanged(const QItemSelection&, const QItemSelection&)));

    // react on selection changes in the STAGED match groups view
    connect(ui->mgStagedView->selectionModel(),
      SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
      SLOT(onStagedSelectionChanged(const QItemSelection&, const QItemSelection&)));

    // default all buttons to "disabled"
    ui->btnSchedule->setEnabled(false);
    ui->btnStage->setEnabled(false);
    ui->btnUnstage->setEnabled(false);
}

//----------------------------------------------------------------------------

ScheduleTabWidget::~ScheduleTabWidget()
{
    delete ui;
}

//----------------------------------------------------------------------------

void ScheduleTabWidget::onBtnStageClicked()
{
  // lets check if a valid match group is selected
  auto mg = ui->mgIdleView->getSelectedMatchGroup();
  if (mg == nullptr) return;

  auto mm = Tournament::getMatchMngr();
  if (mm->canStageMatchGroup(*mg) != OK) return;

  mm->stageMatchGroup(*mg);
}

//----------------------------------------------------------------------------

void ScheduleTabWidget::onBtnUnstageClicked()
{
  // lets check if a valid match group is selected
  auto mg = ui->mgStagedView->getSelectedMatchGroup();
  if (mg == nullptr) return;

  auto mm = Tournament::getMatchMngr();
  if (mm->canUnstageMatchGroup(*mg) != OK) return;

  mm->unstageMatchGroup(*mg);
}

//----------------------------------------------------------------------------

void ScheduleTabWidget::onBtnScheduleClicked()
{
  // is at least one match group staged?
  auto mm = Tournament::getMatchMngr();
  if (mm->getMaxStageSeqNum() == 0) return;

  mm->scheduleAllStagedMatchGroups();
  updateButtons();
}

//----------------------------------------------------------------------------

void ScheduleTabWidget::onIdleSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
  updateButtons();
}

//----------------------------------------------------------------------------

void ScheduleTabWidget::onStagedSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
  updateButtons();
}

//----------------------------------------------------------------------------

void ScheduleTabWidget::updateButtons()
{
  auto mm = Tournament::getMatchMngr();

  // update the "stage"-button
  auto mg = ui->mgIdleView->getSelectedMatchGroup();
  if ((mg != nullptr) && (mm->canStageMatchGroup(*mg) == OK))
  {
    ui->btnStage->setEnabled(true);
  } else {
    ui->btnStage->setEnabled(false);
  }

  // update the "unstage"-button
  mg = ui->mgStagedView->getSelectedMatchGroup();
  if ((mg != nullptr) && (mm->canUnstageMatchGroup(*mg) == OK))
  {
    ui->btnUnstage->setEnabled(true);
  } else {
    ui->btnUnstage->setEnabled(false);
  }

  // update the "Schedule"-button
  ui->btnSchedule->setEnabled(mm->getMaxStageSeqNum() != 0);
}

//----------------------------------------------------------------------------

void ScheduleTabWidget::onMatchDoubleClicked(const QModelIndex &index)
{
  auto ma = ui->tvMatches->getSelectedMatch();
  if (ma == nullptr) return;

  auto cm = Tournament::getCourtMngr();
  auto mm = Tournament::getMatchMngr();

  // first of all, make sure that the match is eligible for being started
  if (ma->getState() != STAT_MA_READY)
  {
    QString msg = tr("This match cannot be started at this point in time.\n");
    msg += tr("It's probably waiting for all players to become available or \n");
    msg += tr("for matches in earlier rounds to be finished first.");
    QMessageBox::warning(this, tr("Assign match to court"), msg);
    return;
  }

  // check if there is a court available
  ERR err;
  auto nextCourt = cm->autoSelectNextUnusedCourt(&err, false);
  if (err == ONLY_MANUAL_COURT_AVAIL)
  {
    QString msg = tr("There are no free courts for automatic match assignment available right now,\n");
    msg += tr("However, there is at least one free court for manual match assignment.\n\n");
    msg += tr("Use this court for running the match?");
    int result = QMessageBox::question(this, tr("Assign match to court"), msg);
    if (result != QMessageBox::Yes)
    {
      return;
    }

    nextCourt = cm->autoSelectNextUnusedCourt(&err, true);
    if (nextCourt == nullptr)
    {
      QString msg = tr("An unexpected error occured.\n");
      msg += tr("Sorry, this shouldn't happen.\n");
      msg += tr("The match cannot be started.");
      QMessageBox::critical(this, tr("Assign match to court"), msg);
      return;
    }
  }

  if (err == NO_COURT_AVAIL)
  {
    QString msg = tr("The match cannot be started since there is no\n");
    msg += tr("free court available right now.");
    QMessageBox::warning(this, tr("Assign match to court"), msg);
    return;
  }

  // if we made it to this point and nextCourt is not null and err is OK,
  // we may try to assign the match
  if ((err == OK) && (nextCourt != nullptr))
  {
    // after all the checks before, the following call
    // should always yield "ok"
    err = mm->canAssignMatchToCourt(*ma, *nextCourt);
    if (err != OK)
    {
      QString msg = tr("An unexpected error occured.\n");
      msg += tr("Sorry, this shouldn't happen.\n");
      msg += tr("The match cannot be started.");
      QMessageBox::critical(this, tr("Assign match to court"), msg);
      return;
    }

    // prep the call
    QString call = GuiHelpers::prepCall(*ma, *nextCourt);
    int result = QMessageBox::question(this, tr("Assign match to court"), call);

    if (result == QMessageBox::Yes)
    {
      // after all the checks before, the following call
      // should always yield "ok"
      err = mm->assignMatchToCourt(*ma, *nextCourt);
      if (err != OK)
      {
        QString msg = tr("An unexpected error occured.\n");
        msg += tr("Sorry, this shouldn't happen.\n");
        msg += tr("The match cannot be started.");
        QMessageBox::critical(this, tr("Assign match to court"), msg);
      }      
      ui->tvMatches->updateSelectionAfterDataChange();
      return;
    }
    QMessageBox::information(this, tr("Assign match to court"), tr("Call cancled, match not started"));
  }

}

//----------------------------------------------------------------------------

void ScheduleTabWidget::onCourtDoubleClicked(const QModelIndex &index)
{
  auto court = ui->tvCourts->getSelectedCourt();
  if (court == nullptr) return;

  auto ma = court->getMatch();
  if (ma == nullptr)
  {
    return;  // no match assigned to this court
  }

  askAndStoreMatchResult(*ma);
}

//----------------------------------------------------------------------------

void ScheduleTabWidget::askAndStoreMatchResult(const Match &ma)
{
  // only accept results for running matches
  if (ma.getState() != STAT_MA_RUNNING)
  {
    return;
  }

  // ask the user for the match result
  DlgMatchResult dlg(this, ma);
  dlg.setModal(true);
  int dlgResult = dlg.exec();
  if (dlgResult != QDialog::Accepted)
  {
    return;
  }
  auto matchResult = dlg.getMatchScore();
  assert(matchResult != nullptr);

  // create a (rather ugly) confirmation message box
  QString msg = tr("Please confirm:\n\n");
  msg += ma.getPlayerPair1().getDisplayName() + "\n";
  msg += "\tvs.\n";
  msg += ma.getPlayerPair2().getDisplayName() + "\n\n";
  msg += tr("Result: ");
  QString sResult = matchResult->toString();
  sResult = sResult.replace(",", ", ");
  msg += sResult + "\n\n\n";
  if (matchResult->getWinner() == 1)
  {
    msg += tr("WINNER: ");
    msg += ma.getPlayerPair1().getDisplayName();
    msg += "\n\n";
    msg += tr("LOSER: ");
    msg += ma.getPlayerPair2().getDisplayName();
  }
  if (matchResult->getWinner() == 2)
  {
    msg += tr("WINNER: ");
    msg += ma.getPlayerPair2().getDisplayName();
    msg += "\n\n";
    msg += tr("LOSER: ");
    msg += ma.getPlayerPair1().getDisplayName();
  }
  if (matchResult->getWinner() == 0)
  {
    msg += tr("The match result is DRAW");
  }
  msg += "\n\n";

  int confirm = QMessageBox::question(this, tr("Please confirm match result"), msg);
  if (confirm != QMessageBox::Yes) return;

  // actually store the data and update the internal object states
  MatchMngr* mm = Tournament::getMatchMngr();
  mm->setMatchScoreAndFinalizeMatch(ma, *matchResult);

  // ask the user if the next available match should be started on the
  // now free court
  //
  // first of all, check if there is a next match available
  int nextMatchId;
  int nextCourtId;
  ERR e = mm->getNextViableMatchCourtPair(&nextMatchId, &nextCourtId, true);
  if ((e == NO_MATCH_AVAIL) || (nextMatchId < 1))
  {
    return;
  }

  // now ask if the match should be started
  confirm = QMessageBox::question(this, tr("Next Match"), tr("Start the next available match on the free court?"));
  if (confirm != QMessageBox::Yes) return;

  // instead of the court determined by getNextViableMatchCourtPair we use
  // the court of the previous match
  auto oldCourt = ma.getCourt();
  assert(oldCourt != nullptr);

  // can we assign the next match to the old court?
  auto nextMatch = mm->getMatch(nextMatchId);
  assert(nextMatch != nullptr);
  e = mm->canAssignMatchToCourt(*nextMatch, *oldCourt);
  if (e != OK)
  {
    QString msg = tr("The match cannot be started on this court. Please start the next match manually.");
    QMessageBox::critical(this, tr("Next Match"), msg);
    return;
  }

  // ok, we're all set. Call the match
  //
  // TODO: this is redundant code
  QString call = GuiHelpers::prepCall(*nextMatch, *oldCourt);
  int result = QMessageBox::question(this, tr("Assign match to court"), call);

  if (result == QMessageBox::Yes)
  {
    // after all the checks before, the following call
    // should always yield "ok"
    e = mm->assignMatchToCourt(*nextMatch, *oldCourt);
    if (e != OK)
    {
      QString msg = tr("An unexpected error occured.\n");
      msg += tr("Sorry, this shouldn't happen.\n");
      msg += tr("The match cannot be started.");
      QMessageBox::critical(this, tr("Assign match to court"), msg);
    }
    ui->tvMatches->updateSelectionAfterDataChange();
    return;
  }
  QMessageBox::information(this, tr("Assign match to court"), tr("Call cancled, match not started"));
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

