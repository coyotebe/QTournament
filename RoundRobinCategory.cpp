/* 
 * File:   RoundRobinCategory.cpp
 * Author: volker
 * 
 * Created on August 25, 2014, 8:34 PM
 */

#include "RoundRobinCategory.h"
#include "KO_Config.h"
#include "Tournament.h"
#include "CatRoundStatus.h"
#include "RankingEntry.h"
#include "RankingMngr.h"
#include "assert.h"
#include "BracketGenerator.h"

#include <QDebug>

using namespace dbOverlay;

namespace QTournament
{

  RoundRobinCategory::RoundRobinCategory(TournamentDB* db, int rowId)
  : Category(db, rowId)
  {
    qDebug() << "!!  RR  !!";
  }

//----------------------------------------------------------------------------

  RoundRobinCategory::RoundRobinCategory(TournamentDB* db, dbOverlay::TabRow row)
  : Category(db, row)
  {
    qDebug() << "!!  RR  !!";
  }

//----------------------------------------------------------------------------

  ERR RoundRobinCategory::canFreezeConfig()
  {
    if (getState() != STAT_CAT_CONFIG)
    {
      return CONFIG_ALREADY_FROZEN;
    }
    
    // make sure there no unpaired players in singles or doubles
    if ((getMatchType() != SINGLES) && (hasUnpairedPlayers()))
    {
      return UNPAIRED_PLAYERS;
    }
    
    // make sure we have a valid group configuration
    PlayerPairList pp = getPlayerPairs();
    KO_Config cfg = KO_Config(getParameter_string(GROUP_CONFIG));
    if (!(cfg.isValid(pp.count())))
    {
      return INVALID_KO_CONFIG;
    }
    
    return OK;
  }

//----------------------------------------------------------------------------

  bool RoundRobinCategory::needsInitialRanking()
  {
    return false;
  }

//----------------------------------------------------------------------------

  bool RoundRobinCategory::needsGroupInitialization()
  {
    return true;
  }

//----------------------------------------------------------------------------

  ERR RoundRobinCategory::prepareFirstRound(ProgressQueue *progressNotificationQueue)
  {
    if (getState() != STAT_CAT_IDLE) return WRONG_STATE;

    auto mm = Tournament::getMatchMngr();
    auto pp = Tournament::getPlayerMngr();

    // make sure we have not been called before; to this end, just
    // check that there have no matches been created for us so far
    auto allGrp = mm->getMatchGroupsForCat(*this);
    // do not return an error here, because obviously we have been
    // called successfully before and we only want to avoid
    // double initialization
    if (allGrp.count() != 0) return OK;

    // alright, this is a virgin category. Generate group matches
    // for each group
    KO_Config cfg = KO_Config(getParameter_string(GROUP_CONFIG));
    if (progressNotificationQueue != nullptr)
    {
      progressNotificationQueue->reset(cfg.getNumGroupMatches());
    }
    for (int grpIndex = 0; grpIndex < cfg.getNumGroups(); ++grpIndex)
    {
      PlayerPairList grpMembers = getPlayerPairs(grpIndex+1);
      ERR e = generateGroupMatches(grpMembers, grpIndex+1, 1, progressNotificationQueue);
      if (e != OK) return e;
    }

    return OK;
  }

//----------------------------------------------------------------------------

  int RoundRobinCategory::calcTotalRoundsCount() const
  {
    OBJ_STATE stat = getState();
    if ((stat == STAT_CAT_CONFIG) || (stat == STAT_CAT_FROZEN))
    {
      return -1;   // category not yet fully configured; can't calc rounds
    }

    // the following call must succeed, since we made it past the
    // configuration point
    KO_Config cfg = KO_Config(getParameter_string(GROUP_CONFIG));

    // the number of rounds is
    // (number of group rounds) + (number of KO rounds)
    int groupRounds = cfg.getNumRounds();

    KO_START startLevel = cfg.getStartLevel();
    int eliminationRounds = 1;  // finals
    if (startLevel != FINAL) ++eliminationRounds; // semi-finals for all, except we dive straight into finals
    if ((startLevel == QUARTER) || (startLevel == L16)) ++eliminationRounds;  // QF and last 16
    if (startLevel == L16) ++eliminationRounds;  // round of last 16

    return groupRounds + eliminationRounds;
  }

//----------------------------------------------------------------------------

  // this return a function that should return true if "a" goes before "b" when sorting. Read:
  // return a function that return true true if the score of "a" is better than "b"
  std::function<bool (RankingEntry& a, RankingEntry& b)> RoundRobinCategory::getLessThanFunction()
  {
    return [](RankingEntry& a, RankingEntry& b) {
      // first criteria: won matches
      tuple<int, int, int, int> matchStatsA = a.getMatchStats();
      tuple<int, int, int, int> matchStatsB = b.getMatchStats();
      if ((get<0>(matchStatsA)) > (get<0>(matchStatsB))) return true;

      // second criteria: won games
      tuple<int, int, int> gameStatsA = a.getGameStats();
      tuple<int, int, int> gameStatsB = b.getGameStats();
      if ((get<0>(gameStatsA)) > (get<0>(gameStatsB))) return true;

      // third criteria: more points
      tuple<int, int> pointStatsA = a.getPointStats();
      tuple<int, int> pointStatsB = b.getPointStats();
      if ((get<0>(pointStatsA)) > (get<0>(pointStatsB))) return true;

      // TODO: add a direct comparison as additional criteria?

      // Default: "a" is not strictly better than "b", so we return false
      return false;
    };
  }

//----------------------------------------------------------------------------

  ERR RoundRobinCategory::onRoundCompleted(int round)
  {
    // determine the number of group rounds.
    //
    // The following call must succeed, since we made it past the
    // configuration point
    KO_Config cfg = KO_Config(getParameter_string(GROUP_CONFIG));
    int groupRounds = cfg.getNumRounds();

    RankingMngr* rm = Tournament::getRankingMngr();
    ERR err;

    // if we are still in group rounds, simply calculate the
    // new ranking
    if (round <= groupRounds)
    {
      rm->createUnsortedRankingEntriesForLastRound(*this, &err);
      if (err != OK) return err;  // shouldn't happen
      rm->sortRankingEntriesForLastRound(*this, &err);
      if (err != OK) return err;  // shouldn't happen
    }

    // if this was the last round in group rounds,
    // we need to wait for user input (seeding)
    // before we can enter the KO rounds
    if (round == groupRounds)
    {
      Tournament::getCatMngr()->switchCatToWaitForSeeding(*this);
    }

    // Actions for KO rounds
    if (round > groupRounds)
    {
      // create ranking entries for everyone who played.
      // this is only to get the accumulated values for the finalists right
      PlayerPairList ppList;
      ppList = this->getRemainingPlayersAfterRound(round - 1, &err);
      if (err != OK) return err;
      rm->createUnsortedRankingEntriesForLastRound(*this, &err, ppList);
      if (err != OK) return err;

      // there's nothing to do for us except after the last round.
      // after the last roound, we have to create final ranking entries
      CatRoundStatus crs = getRoundStatus();
      int lastFinishedRound = crs.getFinishedRoundsCount();
      if (lastFinishedRound != calcTotalRoundsCount())
      {
        return OK;
      }

      // set the ranks for the winner / losers of the finals
      MatchMngr* mm = Tournament::getMatchMngr();
      for (MatchGroup mg : mm->getMatchGroupsForCat(*this, lastFinishedRound))
      {
        for (Match ma : mg.getMatches())
        {
          auto winner = ma.getWinner();
          assert(winner != nullptr);
          auto re = rm->getRankingEntry(*winner, lastFinishedRound);
          assert(re != nullptr);
          int winnerRank = ma.getWinnerRank();
          assert(winnerRank > 0);
          rm->forceRank(*re, winnerRank);

          auto loser = ma.getLoser();
          assert(loser != nullptr);
          re = rm->getRankingEntry(*loser, lastFinishedRound);
          assert(re != nullptr);
          int loserRank = ma.getLoserRank();
          assert(loserRank > 0);
          rm->forceRank(*re, loserRank);
        }
      }
    }
    return OK;
  }

//----------------------------------------------------------------------------

  PlayerPairList RoundRobinCategory::getRemainingPlayersAfterRound(int round, ERR* err) const
  {
    // we can only determine remaining players after completed rounds
    CatRoundStatus crs = getRoundStatus();
    if (round > crs.getFinishedRoundsCount())
    {
      if (err != nullptr) *err = INVALID_ROUND;
      return PlayerPairList();
    }

    // the following call must succeed since we finished at least one round
    KO_Config cfg = KO_Config(getParameter_string(GROUP_CONFIG));
    int numGroupRounds = cfg.getNumRounds();

    // three cases for the list of remaining players:
    //   1) before the last round robing round (==> all players)
    //   2) after the last RR round and before the first KO round (==> qualified players)
    //   3) in KO rounds (==> survivors)

    if (round < numGroupRounds)
    {
      if (err != nullptr) *err = OK;
      return getPlayerPairs();
    }

    if (round == numGroupRounds)
    {
      if (err != nullptr) *err = OK;
      return getQualifiedPlayersAfterRoundRobin_sorted();
    }

    if (round > numGroupRounds)
    {
      //
      // TODO: this is redundant code. See EliminationCategory
      //

      // get the list for the previous round
      PlayerPairList result;
      ERR e;
      result = this->getRemainingPlayersAfterRound(round-1, &e);
      if (e != OK)
      {
        if (err != nullptr) *err = INVALID_ROUND;
        return PlayerPairList();
      }

      // get the match losers of this round
      // and remove them from the list of the previous round
      //
      // exception: losers in semi-finals will continue in the
      // match for 3rd place
      if (round == (calcTotalRoundsCount() - 1))  // semi-finals
      {
        if (err != nullptr) *err = OK;
        return result;
      }
      MatchMngr* mm = Tournament::getMatchMngr();
      for (MatchGroup mg : mm->getMatchGroupsForCat(*this, round))
      {
        for (Match ma : mg.getMatches())
        {
          auto loser = ma.getLoser();
          if (loser == nullptr) continue;   // shouldn't happen
          result.removeAll(*loser);
        }
      }

      if (err != nullptr) *err = OK;
      return result;
    }

    // we should never reach this point
    if (err != nullptr) *err = INVALID_ROUND;
    return PlayerPairList();
  }

//----------------------------------------------------------------------------

  PlayerPairList RoundRobinCategory::getPlayerPairsForIntermediateSeeding() const
  {
    if (getState() != STAT_CAT_WAIT_FOR_INTERMEDIATE_SEEDING)
    {
      return PlayerPairList();
    }

    return getQualifiedPlayersAfterRoundRobin_sorted();
  }

//----------------------------------------------------------------------------

  ERR RoundRobinCategory::resolveIntermediateSeeding(const PlayerPairList& seed, ProgressQueue* progressNotificationQueue) const
  {
    if (getState() != STAT_CAT_WAIT_FOR_INTERMEDIATE_SEEDING)
    {
      return CATEGORY_NEEDS_NO_SEEDING;
    }

    // make sure that the required player pairs and the
    // provided player pairs are identical
    PlayerPairList controlList = getPlayerPairsForIntermediateSeeding();
    for (PlayerPair pp : seed)
    {
      if (!(controlList.contains(pp)))
      {
        return INVALID_SEEDING_LIST;
      }
      controlList.removeAll(pp);
    }
    if (!(controlList.isEmpty()))
    {
      return INVALID_SEEDING_LIST;
    }

    // okay, the list is valid. Now lets generate single-KO matches
    // for the second phase of the tournament
    KO_Config cfg = KO_Config(getParameter_string(GROUP_CONFIG));
    int numGroupRounds = cfg.getNumRounds();
    return generateBracketMatches(BracketGenerator::BRACKET_SINGLE_ELIM, seed, numGroupRounds+1, progressNotificationQueue);
  }

//----------------------------------------------------------------------------

  PlayerPairList RoundRobinCategory::getQualifiedPlayersAfterRoundRobin_sorted() const
  {
    // have we finished the round robin phase?
    CatRoundStatus crs = getRoundStatus();
    KO_Config cfg = KO_Config(getParameter_string(GROUP_CONFIG));
    int numGroupRounds = cfg.getNumRounds();
    if (crs.getFinishedRoundsCount() < numGroupRounds)
    {
      return PlayerPairList();  // nope, we're not there yet
    }

    // get the ranking after the last round-robin round
    RankingEntryListList rll = Tournament::getRankingMngr()->getSortedRanking(*this, numGroupRounds);
    assert(rll.size() == cfg.getNumGroups());

    PlayerPairList result;
    for (RankingEntryList rl : rll)
    {
      // this should be guaranteed by the minimum group size of three
      assert(rl.size() >= 2);

      // the first in each group is always qualified
      auto qualifiedPP = rl.at(0).getPlayerPair();
      assert(qualifiedPP != nullptr);
      result.push_front(*qualifiedPP);

      // maybe the second qualifies as well
      if (cfg.getSecondSurvives())
      {
        qualifiedPP = rl.at(1).getPlayerPair();
        assert(qualifiedPP != nullptr);
        result.push_back(*qualifiedPP);
      }
    }

    // the list entries for the first-ranked players are in reverse order, e.g.:
    // the first of group 8 is at index 0, the first of group 7 is at index 1, ...
    //
    // let's fix that (for cosmetic reasons)
    //std::reverse(result.begin(), result.begin() + cfg.getNumGroups() + 1);  // +1 because the last element is not included in the reversing operation
    auto firstItem = std::begin(result);
    auto lastItem = firstItem;
    std::advance(lastItem, cfg.getNumGroups());
    std::reverse(firstItem, lastItem);

    return result;
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


}
