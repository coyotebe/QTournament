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

#ifndef CMDASSIGNREFEREETOMATCH_H
#define CMDASSIGNREFEREETOMATCH_H

#include <QObject>

#include "AbstractCommand.h"
#include "Match.h"

using namespace QTournament;

class cmdAssignRefereeToMatch : public QObject, AbstractCommand
{
  Q_OBJECT

public:
  cmdAssignRefereeToMatch(QWidget* p, const Match& _ma, bool _isMatchCall);
  virtual ERR exec() override;
  virtual ~cmdAssignRefereeToMatch() {}

protected:
  Match ma;
  bool isMatchCall;
};

#endif // CMDASSIGNREFEREETOMATCH_H
