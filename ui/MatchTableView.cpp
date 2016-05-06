/*
 *    This is QTournament, a badminton tournament management program.
 *    Copyright (C) 2014 - 2015  Volker Knollmann
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QMessageBox>

#include "MatchTableView.h"
#include "MainFrame.h"

#include "CourtMngr.h"
#include "Court.h"
#include "ui/GuiHelpers.h"
#include "SignalRelay.h"
#include "MatchMngr.h"
#include "CourtMngr.h"
#include "DlgSelectReferee.h"

MatchTableView::MatchTableView(QWidget* parent)
  :QTableView(parent), db(nullptr), curDataModel(nullptr)
{
  // an empty model for clearing the table when
  // no tournament is open
  emptyModel = new QStringListModel();

  // prepare a proxy model to support sorting by columns
  sortedModel = new QSortFilterProxyModel();
  sortedModel->setSourceModel(emptyModel);
  setModel(sortedModel);

  defaultDelegate = itemDelegate();

  // initiate the model(s) as empty
  setDatabase(nullptr);

  // react on selection changes in the match table view
  connect(selectionModel(),
    SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
    SLOT(onSelectionChanged(const QItemSelection&, const QItemSelection&)));

  // handle context menu requests
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(onContextMenuRequested(const QPoint&)));

  // setup the context menu
  initContextMenu();

  // register for an ackward signal relay mechanism in order
  // to give reports access to the currently selected match
  SignalRelay::getInstance()->registerSender(this);
}

//----------------------------------------------------------------------------
    
MatchTableView::~MatchTableView()
{
  delete emptyModel;
  delete sortedModel;
  SignalRelay::cleanUp();
  if (curDataModel != nullptr) delete curDataModel;
  if (defaultDelegate != nullptr) delete defaultDelegate;
}

//----------------------------------------------------------------------------
    
unique_ptr<Match> MatchTableView::getSelectedMatch() const
{
  // make sure we have non-empty model
  auto mod = model();
  if (mod == nullptr) return nullptr;
  if (mod->rowCount() == 0) return nullptr;

  // make sure we have one item selected
  QModelIndexList indexes = selectionModel()->selection().indexes();
  if (indexes.count() == 0)
  {
    return nullptr;
  }

  // return the selected item
  int selectedSourceRow = sortedModel->mapToSource(indexes.at(0)).row();
  MatchMngr mm{db};
  return mm.getMatchBySeqNum(selectedSourceRow);
}

//----------------------------------------------------------------------------

void MatchTableView::onSelectionChanged(const QItemSelection& selectedItem, const QItemSelection& deselectedItem)
{
  for (auto item : selectedItem)
  {
    matchItemDelegate->setSelectedRow(item.top());
    //resizeRowToContents(item.top());
  }
  for (auto item : deselectedItem)
  {
    //resizeRowToContents(item.top());
  }

  // emit a signal containing the newly selected match
  // this is used by the result sheets report to update
  // the range of printed matches
  auto ma = getSelectedMatch();
  emit matchSelectionChanged((ma == nullptr) ? -1 : ma->getId());
}

//----------------------------------------------------------------------------

void MatchTableView::updateSelectionAfterDataChange()
{
  QModelIndexList indexes = selectionModel()->selection().indexes();
  if (indexes.count() == 0)
  {
    return;
  }
  int selectedTargetRow = indexes.at(0).row();
  matchItemDelegate->setSelectedRow(selectedTargetRow);
  resizeRowToContents(selectedTargetRow);
}

//----------------------------------------------------------------------------

void MatchTableView::setDatabase(TournamentDB* _db)
{
  // set the new data model
  MatchTableModel* newDataModel = nullptr;
  if (_db != nullptr)
  {
    newDataModel = new MatchTableModel(_db);
    sortedModel->setSourceModel(newDataModel);
    setColumnHidden(MatchTableModel::STATE_COL_ID, true);  // hide the column containing the internal object state

    // create a regular expression, that matches either the match state
    // READY, BUSY, FUZZY or WAITING
    QString reString = "^" + QString::number(static_cast<int>(STAT_MA_READY)) + "|";
    reString += QString::number(static_cast<int>(STAT_MA_BUSY)) + "|";
    reString += QString::number(static_cast<int>(STAT_MA_FUZZY)) + "|";   // TODO: check if there can be a condition where a match is FUZZY but without assigned match number
    reString += QString::number(static_cast<int>(STAT_MA_WAITING)) + "$";

    // apply the regExp as a filter on the state id column
    sortedModel->setFilterRegExp(reString);
    sortedModel->setFilterKeyColumn(MatchTableModel::STATE_COL_ID);

    // sort matches in ascing match number order
    sortedModel->sort(MatchTableModel::MATCH_NUM_COL_ID, Qt::AscendingOrder);

    // define a delegate for drawing the match items
    matchItemDelegate = make_unique<MatchItemDelegate>(_db, this);
    matchItemDelegate->setProxy(sortedModel);
    setItemDelegate(matchItemDelegate.get());

    // resize columns and rows to content once (we do not want permanent automatic resizing)
    horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    verticalHeader()->resizeSections(QHeaderView::ResizeToContents);

  } else {
    sortedModel->setSourceModel(emptyModel);
    setItemDelegate(defaultDelegate);
  }

  // delete the old data model, if it was a
  // CategoryTableModel instance
  if (curDataModel != nullptr)
  {
    delete curDataModel;
  }

  // store the new CategoryTableModel instance, if any
  curDataModel = newDataModel;

  // update the database pointer and set the widget's enabled state
  db = _db;
  setEnabled(db != nullptr);
}

//----------------------------------------------------------------------------

void MatchTableView::onContextMenuRequested(const QPoint& pos)
{
  // map from scroll area coordinates to global widget coordinates
  QPoint globalPos = viewport()->mapToGlobal(pos);

  // resolve the click coordinates to the table row
  int clickedRow = rowAt(pos.y());

  // exit if no table row is under the cursor
  if (clickedRow < 0) return;

  // find out which match is selected
  auto ma = getSelectedMatch();
  if (ma == nullptr) return;  // shouldn't happen

  // enable / disable the action for assigning umpires, depending
  // on the current umpire mode and match state
  actAssignReferee->setEnabled(ma->canAssignReferee() == OK);

  // show the context menu
  updateContextMenu();
  QAction* selectedItem = contextMenu->exec(globalPos);

  if (selectedItem == nullptr) return; // user canceled

  // a bad hack:
  // if the text of the selected item is purely numeric, the user
  // selected "call match on court XXX". We use this information
  // to trigger the right action
  bool isOk = false;
  int selectedCourt = selectedItem->text().toInt(&isOk);
  if (isOk)
  {
    // get the selected court
    CourtMngr cm{db};
    auto co = cm.getCourt(selectedCourt);
    if (co == nullptr) return;  // shouldn't happen

    // call the match on the selected court
    MatchMngr mm{db};
    ERR e = mm.canAssignMatchToCourt(*ma, *co);
    if (e != OK)
    {
      QMessageBox::warning(this, tr("Call match"),
                           tr("Match can't be called on the selected court."));
      return;
    }

    execCall(*ma, *co);;
  }

  // another hack:
  // if the selected item has non-empty user data, one of the umpire modes
  // has been selected.
  //
  // try to assign the new umpire mode to the match
  if (!(selectedItem->data().isNull()))
  {
    int modeId = selectedItem->data().toInt();
    REFEREE_MODE refMode = static_cast<REFEREE_MODE>(modeId);

    MatchMngr mm{db};
    ERR e = mm.setRefereeMode(*ma, refMode);
    if (e != OK)
    {
      QMessageBox::warning(this, tr("Set umpire mode"),
                           tr("The mode can't be set for this match."));
    }
  }
}

//----------------------------------------------------------------------------

void MatchTableView::onWalkoverP1Triggered()
{
  execWalkover(1);
}

//----------------------------------------------------------------------------

void MatchTableView::onWalkoverP2Triggered()
{
  execWalkover(2);
}

//----------------------------------------------------------------------------

void MatchTableView::onMatchDoubleClicked(const QModelIndex& index)
{
  auto ma = getSelectedMatch();
  if (ma == nullptr) return;

  CourtMngr cm{db};
  MatchMngr mm{db};

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
  auto nextCourt = cm.autoSelectNextUnusedCourt(&err, false);
  if (err == ONLY_MANUAL_COURT_AVAIL)
  {
    QString msg = tr("There are no free courts for automatic match assignment available right now.\n");
    msg += tr("However, there is at least one free court for manual match assignment.\n\n");
    msg += tr("Use this court for running the match?");
    int result = QMessageBox::question(this, tr("Assign match to court"), msg);
    if (result != QMessageBox::Yes)
    {
      return;
    }

    nextCourt = cm.autoSelectNextUnusedCourt(&err, true);
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
    execCall(*ma, *nextCourt);
  }
}

//----------------------------------------------------------------------------

void MatchTableView::onAssignedRefereeTriggered()
{
  auto ma = getSelectedMatch();
  if (ma == nullptr) return;

  DlgSelectReferee dlg{db, *ma, false, this};
  dlg.exec();
}

//----------------------------------------------------------------------------

void MatchTableView::initContextMenu()
{
  // prepare all actions
  actPostponeMatch = new QAction(tr("Postpone"), this);

  // create sub-actions for the walkover-selection
  actWalkoverP1 = new QAction("P1", this);  // this is just a dummy
  actWalkoverP2 = new QAction("P2", this);  // this is just a dummy

  // create the context menu and connect it to the actions
  contextMenu = make_unique<QMenu>();
  walkoverSelectionMenu = contextMenu->addMenu(tr("Walkover for..."));
  walkoverSelectionMenu->addAction(actWalkoverP1);
  walkoverSelectionMenu->addAction(actWalkoverP2);

  // disable the "Postpone"-entry as long as this function
  // is not implemented
  //contextMenu->addAction(actPostponeMatch);

  contextMenu->addSeparator();
  courtSelectionMenu = contextMenu->addMenu(tr("Call match on court..."));

  // submenus for umpire functions
  contextMenu->addSeparator();
  refereeMode_submenu = contextMenu->addMenu(tr("Set umpire mode"));
  auto newAction = refereeMode_submenu->addAction(tr("None"));
  newAction->setData(static_cast<int>(REFEREE_MODE::NONE));
  refereeMode_submenu->addSeparator();
  newAction = refereeMode_submenu->addAction(tr("Pick from all players"));
  newAction->setData(static_cast<int>(REFEREE_MODE::ALL_PLAYERS));
  newAction = refereeMode_submenu->addAction(tr("Pick from recent losers"));
  newAction->setData(static_cast<int>(REFEREE_MODE::RECENT_LOSERS));
  newAction = refereeMode_submenu->addAction(tr("Pick from special team"));
  newAction->setData(static_cast<int>(REFEREE_MODE::SPECIAL_TEAM));
  refereeMode_submenu->addSeparator();
  newAction = refereeMode_submenu->addAction(tr("Manual"));
  newAction->setData(static_cast<int>(REFEREE_MODE::HANDWRITTEN));
  actAssignReferee = new QAction(tr("Assign umpire..."), this);
  contextMenu->addAction(actAssignReferee);

  // connect actions and slots
  connect(actWalkoverP1, SIGNAL(triggered(bool)), this, SLOT(onWalkoverP1Triggered()));
  connect(actWalkoverP2, SIGNAL(triggered(bool)), this, SLOT(onWalkoverP2Triggered()));
  connect(actAssignReferee, SIGNAL(triggered(bool)), this, SLOT(onAssignedRefereeTriggered()));

  updateContextMenu();
}

//----------------------------------------------------------------------------

void MatchTableView::updateContextMenu()
{
  auto ma = getSelectedMatch();

  // completely re-build the list of available courts
  bool isCallPossible = ((ma != nullptr) && (ma->getState() == STAT_MA_READY));
  courtSelectionMenu->setEnabled(isCallPossible);
  if (isCallPossible)
  {
    courtSelectionMenu->clear();

    CourtMngr cm{db};

    QStringList availCourtNum;
    for (auto co : cm.getAllCourts())
    {
      if (co.getState() == STAT_CO_AVAIL)
      {
        availCourtNum << QString::number(co.getNumber());
      }
    }

    availCourtNum.sort();

    for (QString num : availCourtNum)
    {
      courtSelectionMenu->addAction(num);
    }

    courtSelectionMenu->setEnabled(availCourtNum.size() > 0);
  }

  // update the player pair names for the walkover menu
  walkoverSelectionMenu->setEnabled(ma != nullptr);
  if (ma != nullptr)
  {
    if (ma->isWalkoverPossible())
    {
      actWalkoverP1->setText(ma->getPlayerPair1().getDisplayName());
      actWalkoverP2->setText(ma->getPlayerPair2().getDisplayName());
    } else {
      walkoverSelectionMenu->setEnabled(false);
    }
  }

  // update the "postpone" action
  actPostponeMatch->setEnabled(ma != nullptr);
}

//----------------------------------------------------------------------------

void MatchTableView::execWalkover(int playerNum)
{
  auto ma = getSelectedMatch();
  if (ma == nullptr) return; // shouldn't happen
  if ((playerNum != 1) && (playerNum != 2)) return; // shouldn't happen
  GuiHelpers::execWalkover(this, *ma, playerNum);
}

//----------------------------------------------------------------------------

void MatchTableView::execCall(const Match& ma, const Court& co)
{
  MatchMngr mm{db};

  // all necessary pre-checks should have been performed before
  // so that the following call should always yield "ok"
  ERR err = mm.canAssignMatchToCourt(ma, co);
  if (err != OK)
  {
    QString msg = tr("An unexpected error occured.\n");
    msg += tr("Sorry, this shouldn't happen.\n");
    msg += tr("The match cannot be started.");
    QMessageBox::critical(this, tr("Assign match to court"), msg);
    return;
  }

  // prep the call
  QString call = GuiHelpers::prepCall(ma, co);
  int result = QMessageBox::question(this, tr("Assign match to court"), call);

  if (result == QMessageBox::Yes)
  {
    // after all the checks before, the following call
    // should always yield "ok"
    err = mm.assignMatchToCourt(ma, co);
    if (err != OK)
    {
      QString msg = tr("An unexpected error occured.\n");
      msg += tr("Sorry, this shouldn't happen.\n");
      msg += tr("The match cannot be started.");
      QMessageBox::critical(this, tr("Assign match to court"), msg);
    }
    updateSelectionAfterDataChange();
    return;
  }
  QMessageBox::information(this, tr("Assign match to court"), tr("Call cancled, match not started"));
}

//----------------------------------------------------------------------------
    

//----------------------------------------------------------------------------
    

//----------------------------------------------------------------------------
    

//----------------------------------------------------------------------------
    

//----------------------------------------------------------------------------
    

