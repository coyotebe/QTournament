#include <tuple>

#include <QList>

#include "InOutList.h"
#include "SimpleReportGenerator.h"
#include "TableWriter.h"

#include "CatRoundStatus.h"
#include "ui/GuiHelpers.h"
#include "RankingMngr.h"
#include "RankingEntry.h"
#include "KO_Config.h"

namespace QTournament
{


InOutList::InOutList(TournamentDB* _db, const QString& _name, const Category& _cat, int _round)
  :AbstractReport(_db, _name), cat(_cat), round(_round)
{
  if (!isValidCatRoundCombination(_cat, _round))
  {
    throw std::runtime_error("Requested in-out-list for invalid category and/or round");
  }
}

//----------------------------------------------------------------------------

upSimpleReport InOutList::regenerateReport() const
{
  QString repName = cat.getName() + " -- " + tr("Knocked-out players after round ") + QString::number(round);
  upSimpleReport result = createEmptyReport_Portrait();

  setHeaderAndHeadline(result.get(), repName);

  // retrieve the "in and out" lists
  ERR err;
  auto specialCat = cat.convertToSpecializedObject();
  PlayerPairList inList = specialCat->getRemainingPlayersAfterRound(round, &err);
  if (err != OK)
  {
    result->writeLine(tr("An error occurred during report generation"));
    return result;
  }
  PlayerPairList outList = specialCat->getEliminatedPlayersAfterRound(round, &err);
  if (err != OK)
  {
    result->writeLine(tr("An error occurred during report generation"));
    return result;
  }

  if (outList.isEmpty())
  {
    result->writeLine(tr("All players are still in the game, no knock-outs yet."));
  } else {
    printIntermediateHeader(result, tr("Knocked-out players"));
    for (PlayerPair pp : outList)
    {
      result->writeLine(pp.getDisplayName());
    }

    printIntermediateHeader(result, tr("Remaining players"));
    for (PlayerPair pp : inList)
    {
      result->writeLine(pp.getDisplayName());
    }
  }

  return result;

  // set header and footer
  setHeaderAndFooter(result, repName);

  return result;
}

//----------------------------------------------------------------------------

QStringList InOutList::getReportLocators() const
{
  QStringList result;

  QString loc = tr("In-Out-List::");
  loc += cat.getName() + "::";
  loc += tr("after round ") + QString::number(round);

  result.append(loc);

  return result;
}

//----------------------------------------------------------------------------

bool InOutList::isValidCatRoundCombination(const Category& _cat, int _round)
{
  // we must be beyond CONFIG for this report to make any sense at all
  OBJ_STATE catState = _cat.getState();
  if ((catState == STAT_CAT_CONFIG) || (catState == STAT_CAT_FROZEN))
  {
    return false;
  }

  // make sure that the requested round is already finished
  CatRoundStatus crs = _cat.getRoundStatus();
  if (_round > crs.getFinishedRoundsCount())
  {
    return false;
  }

  // if we're in match system "round robin with KO rounds", this report
  // makes only sense after the last round robin round
  MATCH_SYSTEM mSys = _cat.getMatchSystem();
  if (mSys == GROUPS_WITH_KO)
  {
    KO_Config cfg = KO_Config(_cat.getParameter_string(GROUP_CONFIG));
    if (_round < cfg.getNumRounds())
    {
      return false;
    }

    // we're good to go
    return true;
  }

  // for elimination contests, we can always generate in-out-lists
  if (mSys == SINGLE_ELIM)
  {
    return true;
  }

  // a "catch all":
  // if we couldn't validate the request up to here, it's probably invalid
  return false;
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------


}
