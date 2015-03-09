/* 
 * File:   PlayerTableModel.h
 * Author: volker
 *
 * Created on March 17, 2014, 7:51 PM
 */

#ifndef PLAYERTABLEMODEL_H
#define	PLAYERTABLEMODEL_H

#include <QAbstractTableModel>

#include "DbTab.h"
#include "TournamentDB.h"
#include "Player.h"

using namespace QTournament;

class PlayerTableModel : public QAbstractTableModel
{
  Q_OBJECT
  
public:
  static constexpr int COLUMN_COUNT = 4;  // number of columns in the model

  PlayerTableModel (TournamentDB* _db);
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  int columnCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  
  private:
    TournamentDB* db;
    dbOverlay::DbTab playerTab;
    dbOverlay::DbTab teamTab;
    dbOverlay::DbTab catTab;
    
public slots:
  void onBeginCreatePlayer();
  void onEndCreatePlayer(int newPlayerSeqNum);
  void onPlayerRenamed(const Player& p);
  void onTeamRenamed(int teamSeqNum);
  void onPlayerStatusChanged(int playerId, int playerSeqNum);

};

#endif	/* PLAYERTABLEMODEL_H */

