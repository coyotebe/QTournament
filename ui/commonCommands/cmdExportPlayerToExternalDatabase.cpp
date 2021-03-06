/*
 *    This is QTournament, a badminton tournament management program.
 *    Copyright (C) 2014 - 2017  Volker Knollmann
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

#include <QObject>
#include <QMessageBox>

#include "cmdExportPlayerToExternalDatabase.h"
#include "PlayerMngr.h"

cmdExportPlayerToExternalDatabase::cmdExportPlayerToExternalDatabase(QWidget* p, const Player& _pl)
  :AbstractCommand(_pl.getDatabaseHandle(), p), pl(_pl)
{

}

//----------------------------------------------------------------------------

ERR cmdExportPlayerToExternalDatabase::exec()
{
  // make sure we have an external database open
  PlayerMngr pm{db};
  if (!(pm.hasExternalPlayerDatabaseAvailable()))
  {
    QString msg = tr("No valid database for player export available.\n\n");
    msg +=tr("Is the database configured and the file existing?");
    QMessageBox::warning(parentWidget, tr("Export player"), msg);
    return EPD__NOT_OPENED;
  }
  ERR e = pm.openConfiguredExternalPlayerDatabase();
  if (!(pm.hasExternalPlayerDatabaseOpen()) || (e != OK))
  {
    QString msg = tr("Could not open database for player export!");
    QMessageBox::warning(parentWidget, tr("Export player"), msg);
    return EPD__NOT_OPENED;
  }

  ERR err = pm.exportPlayerToExternalDatabase(pl);
  if (err == OK) return OK;

  QString msg;
  switch (err)
  {
  case EPD__CREATION_FAILED:
    msg = tr("Could not export the player data to the database.");
    break;

  default:
    msg = tr("An undefined error occurred. The player has not\n");
    msg += tr("been exported to the external database");
  }

  QMessageBox::warning(parentWidget, tr("Export player"), msg);
  return err;
}

