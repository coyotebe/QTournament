/* 
 * File:   PlayerTableView.h
 * Author: volker
 *
 * Created on March 17, 2014, 8:19 PM
 */

#ifndef COURTTABLEVIEW_H
#define	COURTTABLEVIEW_H

#include <memory>

#include "Tournament.h"
//#include "delegates/MatchItemDelegate.h"

#include <QTableView>
#include <QSortFilterProxyModel>

using namespace QTournament;

class CourtTableView : public QTableView
{
  Q_OBJECT
  
public:
  //enum class FilterType : std::int8_t { IDLE = 1, STAGED = 2, NONE = 0 };

  CourtTableView (QWidget* parent);
  virtual ~CourtTableView ();
  unique_ptr<Court> getSelectedCourt();
  
public slots:
  void onTournamentClosed();
  void onTournamentOpened(Tournament* tnmt);

private slots:
  void onSelectionChanged(const QItemSelection&selectedItem, const QItemSelection&deselectedItem);
  
private:
  Tournament* tnmt;
  QStringListModel* emptyModel;
  QSortFilterProxyModel* sortedModel;
  //MatchItemDelegate* itemDelegate;

};

#endif	/* COURTTABLEVIEW_H */

