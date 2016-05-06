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

#include "GuiHelpers.h"

#include <QMessageBox>

#include "TournamentDataDefs.h"
#include "MatchMngr.h"

GuiHelpers::GuiHelpers()
{

}

GuiHelpers::~GuiHelpers()
{

}

//----------------------------------------------------------------------------

QString GuiHelpers::groupNumToString(int grpNum)
{
  if (grpNum > 0) return QString::number(grpNum);

  switch (grpNum)
  {
  case GROUP_NUM__ITERATION:
    return "--";
  case GROUP_NUM__L16:
    return tr("L16");
  case GROUP_NUM__QUARTERFINAL:
    return tr("QF");
  case GROUP_NUM__SEMIFINAL:
    return tr("SF");
  case GROUP_NUM__FINAL:
    return tr("FI");
  }

  return "??";

}

//----------------------------------------------------------------------------

QString GuiHelpers::groupNumToLongString(int grpNum)
{
  if (grpNum > 0) return (tr("Group ") + QString::number(grpNum));

  switch (grpNum)
  {
  case GROUP_NUM__ITERATION:
    return "--";
  case GROUP_NUM__L16:
    return tr("Round of Last 16");
  case GROUP_NUM__QUARTERFINAL:
    return tr("Quarter Finals");
  case GROUP_NUM__SEMIFINAL:
    return tr("Semi Finals");
  case GROUP_NUM__FINAL:
    return tr("Finals");
  }

  return "??";

}

//----------------------------------------------------------------------------

/**
 * Generates a text proposal that the user should announce when calling a match
 *
 * This can't be integrated in the Match class, because Match is not derived
 * from QObject, so we don't have tr() available there
 *
 * @param ma the match to call
 * @param co the court the match shall be played on
 *
 * @return a string with the announcement
 */
QString GuiHelpers::prepCall(const QTournament::Match &ma, const QTournament::Court &co, int nCall)
{
  QString call = tr("Please announce:\n\n\n");

  if (nCall == 0)
  {
    call += tr("Next match,\n\n");
  } else {
    call += QString::number(nCall + 1) + ". ";
    call += tr("call for ");
  }

  call += tr("match number ") + QString::number(ma.getMatchNumber()) + tr(" on court number ") + QString::number(co.getNumber());
  call += "\n\n";
  call += ma.getCategory().getName() + ",\n\n";
  call += ma.getPlayerPair1().getCallName(tr("and")) + "\n\n";
  call += tr("        versus\n\n");
  call += ma.getPlayerPair2().getCallName(tr("and")) + ",\n\n";
  call += tr("match number ") + QString::number(ma.getMatchNumber()) + tr(" on court number ") + QString::number(co.getNumber());
  call += ".";

  // add the umpire's name, if necessary
  QTournament::REFEREE_MODE refMode = ma.getRefereeMode();
  if ((refMode != QTournament::REFEREE_MODE::NONE) && ((refMode != QTournament::REFEREE_MODE::HANDWRITTEN)))
  {
    QTournament::upPlayer referee = ma.getAssignedReferee();
    if (referee != nullptr)
    {
      call += "\n\n\n";
      call += tr("Umpire is ");
      call += referee->getDisplayName_FirstNameFirst();
      call += ".";
    }
  }

  // add additional calls, if applicable
  if (nCall > 0)
  {
    call += "\n\n";
    call += tr("THIS IS THE ");
    call += QString::number(nCall + 1) + ". ";
    call += tr("CALL!");
  }
  call += "\n\n\n";
  call += tr("Call executed?");

  return call;
}

//----------------------------------------------------------------------------

void GuiHelpers::drawFormattedText(QPainter *painter, QRect r, const QString &s, int alignmentFlags, bool isBold, bool isItalics, QFont fnt, QColor fntColor, double fntSizeFac)
{
  fnt.setItalic(isItalics);
  fnt.setBold(isBold);
  fnt.setPointSizeF(fnt.pointSizeF() * fntSizeFac);

  painter->save();
  painter->setPen(QPen(fntColor));
  painter->setFont(fnt);
  painter->drawText(r, alignmentFlags, s);
  painter->restore();
}

//----------------------------------------------------------------------------

void GuiHelpers::drawFormattedText(QPainter *painter, QRect r, const QString &s, int alignmentFlags, bool isBold, bool isItalics, double fntSizeFac)
{
  drawFormattedText(painter, r, s, alignmentFlags, isBold, isItalics, QFont(), QColor(0,0,0), fntSizeFac);
}

//----------------------------------------------------------------------------

void GuiHelpers::execWalkover(QWidget* parent, const QTournament::Match& ma, int playerNum)
{
  if ((playerNum != 1) && (playerNum != 2)) return; // shouldn't happen
  if (!(ma.isWalkoverPossible())) return;

  // get a user confirmation
  QString msg = tr("This will be a walkover for\n\n\t");
  if (playerNum == 1)
  {
    msg += ma.getPlayerPair1().getDisplayName();
  } else {
    msg += ma.getPlayerPair2().getDisplayName();
  }
  msg += "\n\n";
  msg += tr("All games will be 21:0.") + "\n\n";
  msg += tr("WARNING: this step is irrevocable!") + "\n\n";
  msg += tr("Proceed?");
  int result = QMessageBox::question(parent, tr("Confirm walkover"), msg);
  if (result != QMessageBox::Yes)
  {
    return;
  }
  QTournament::MatchMngr mm{ma.getDatabaseHandle()};
  mm.walkover(ma, playerNum);
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

