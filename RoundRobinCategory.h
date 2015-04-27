/* 
 * File:   RoundRobinCategory.h
 * Author: volker
 *
 * Created on August 25, 2014, 8:34 PM
 */

#ifndef ROUNDROBINCATEGORY_H
#define	ROUNDROBINCATEGORY_H

#include "Category.h"
#include "ThreadSafeQueue.h"
#include "RankingEntry.h"


using namespace dbOverlay;

namespace QTournament
{

  class RoundRobinCategory : public Category
  {
    friend class Category;

  public:
    virtual ERR canFreezeConfig() override;
    virtual bool needsInitialRanking() override;
    virtual bool needsGroupInitialization() override;
    virtual ERR prepareFirstRound(ProgressQueue* progressNotificationQueue=nullptr) override;
    virtual int calcTotalRoundsCount() const override;
    virtual std::function<bool(RankingEntry& a, RankingEntry& b)> getLessThanFunction() override;
    virtual ERR onRoundCompleted(int round) override;
    virtual PlayerPairList getRemainingPlayersAfterRound(int round, ERR *err) const override;
    virtual PlayerPairList getPlayerPairsForIntermediateSeeding() const override;
    virtual ERR resolveIntermediateSeeding(const PlayerPairList& seed, ProgressQueue* progressNotificationQueue=nullptr) const override;

    PlayerPairList getQualifiedPlayersAfterRoundRobin_sorted() const;

    
  private:
    RoundRobinCategory (TournamentDB* db, int rowId);
    RoundRobinCategory (TournamentDB* db, dbOverlay::TabRow row);

  } ;
}

#endif	/* ROUNDROBINCATEGORY_H */

