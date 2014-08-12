/* 
 * File:   TournamentDataDefs.h
 * Author: nyoknvk1
 *
 * Created on 18. Februar 2014, 12:48
 */

#ifndef TOURNAMENTDATADEFS_H
#define	TOURNAMENTDATADEFS_H

#include <QString>

namespace QTournament
{
#define DB_VERSION 1
#define MIN_REQUIRED_DB_VERSION 1

//----------------------------------------------------------------------------

#define GENERIC_NAME_FIELD_NAME QString("Name")
#define MAX_NAME_LEN 50
  
#define GENERIC_STATE_FIELD_NAME QString("ObjState")
  
#define GENERIC_SEQNUM_FIELD_NAME QString("SequenceNumber")

//----------------------------------------------------------------------------
    
  
//----------------------------------------------------------------------------
    

//----------------------------------------------------------------------------
    

//----------------------------------------------------------------------------
    
    
#define DB_NULL QVariant::Int

//----------------------------------------------------------------------------
    
#define TAB_CFG QString("Config")
#define CFG_KEY_DB_VERSION QString("DatabaseVersion")
#define CFG_KEY_TNMT_NAME QString("TournamentName")
#define CFG_KEY_TNMT_ORGA QString("OrganizingClub")
#define CFG_KEY_USE_TEAMS QString("UseTeams")
//#define CFG_KEY_ QString("")
//#define CFG_KEY_ QString("")
//#define CFG_KEY_ QString("")
//#define CFG_KEY_ QString("")
//#define CFG_KEY_ QString("")
//#define CFG_KEY_ QString("")

//----------------------------------------------------------------------------
    
#define TAB_PLAYER QString("Player")
#define PL_FNAME QString("FirstName")
#define PL_LNAME QString("LastName")
#define PL_TEAM_REF QString("TeamRefId")
#define PL_SEX QString("Sex")
//#define PL_ QString("")
//#define PL_ QString("")
//#define PL_ QString("")
//#define PL_ QString("")
//#define PL_ QString("")
//#define PL_ QString("")
//#define PL_ QString("")
//#define PL_ QString("")
//#define PL_ QString("")
//#define PL_ QString("")
//#define PL_ QString("")
//#define PL_ QString("")
//#define PL_ QString("")
//#define PL_ QString("")
//#define PL_ QString("")
//#define PL_ QString("")
//#define PL_ QString("")
//#define PL_ QString("")

//----------------------------------------------------------------------------
    
#define TAB_TEAM QString("Team")
//#define TE_ QString("")

//----------------------------------------------------------------------------
    
#define TAB_CATEGORY QString("Category")
#define CAT_MATCH_TYPE QString("MatchType")
#define CAT_SEX QString("Sex")
#define CAT_SYS QString("System")
#define CAT_ACCEPT_DRAW QString("AcceptDraw")
#define CAT_WIN_SCORE QString("WinScore")
#define CAT_DRAW_SCORE QString("DrawScore")
//#define CAT_ QString("")
//#define CAT_ QString("")
//#define CAT_ QString("")
//#define CAT_ QString("")
//#define CAT_ QString("")
//#define CAT_ QString("")
//#define CAT_ QString("")
//#define CAT_ QString("")
//#define CAT_ QString("")
//#define CAT_ QString("")
//#define CAT_ QString("")
//#define CAT_ QString("")
//#define CAT_ QString("")
  
//----------------------------------------------------------------------------

#define TAB_P2C QString("Player2Category")
#define P2C_PLAYER_REF QString("PlayerRefId")
#define P2C_CAT_REF QString("CategoryRefId")
  
//----------------------------------------------------------------------------

#define TAB_PAIRS QString("PlayerPairs")
#define PAIRS_PLAYER1_REF QString("Player1RefId")
#define PAIRS_PLAYER2_REF QString("Player2RefId")
#define PAIRS_CAT_REF QString("CategoryRefId")
  
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

  enum CAT_PARAMETER {
    ALLOW_DRAW,
    WIN_SCORE,
    DRAW_SCORE
    
  };
  
//----------------------------------------------------------------------------

  enum CAT_ADD_STATE {
    WRONG_SEX,         // Player may not join because the sex doesn't fit
    CAN_JOIN,          // Player can join the category now and participate in matches
    CAT_CLOSED,        // Player WOULD fit, but the category doesn't accept any more players
    ALREADY_MEMBER     // Player is already assigned to this category
  };
  
//----------------------------------------------------------------------------

  enum OBJ_STATE {
    STAT_PL_IDLE,
    STAT_CAT_CONFIG
  };
  
//----------------------------------------------------------------------------
  
  enum MATCH_TYPE {
    SINGLES,
    DOUBLES,
    MIXED
  };  
  
//----------------------------------------------------------------------------
  enum SEX
  {
    M,
    F,
    DONT_CARE
  };  
  
//----------------------------------------------------------------------------
    
  enum MATCH_SYSTEM {
    SWISS_LADDER,
    GROUPS_WITH_KO,
    RANDOMIZE,
    RANKING
  };
  
//----------------------------------------------------------------------------

  class TournamentSettings
  {
  public:
    QString tournamentName;
    QString organizingClub;
    bool useTeams;
  } ;

//----------------------------------------------------------------------------

  enum KO_START {
    SEMI,
    QUARTER,
    L16
  };  

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
    

//----------------------------------------------------------------------------
    

//----------------------------------------------------------------------------
    
}

#endif	/* TOURNAMENTDATADEFS_H */

