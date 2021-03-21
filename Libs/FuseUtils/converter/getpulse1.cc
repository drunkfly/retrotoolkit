/* getpulse1.h: ROM loader state looking for the first part of a data
                pulse pair
   Copyright (c) 2008 Fredrick Meunier

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

   Author contact information:

   E-mail: fredm@spamcop.net

*/

#include "findpilot.h"
#include "getpulse1.h"
#include "getpulse2.h"

getpulse1* getpulse1::unique_instance;

getpulse1*
getpulse1::instance()
{
  if( unique_instance == 0 ) {
    unique_instance = new getpulse1();
  }
  return unique_instance;
}

void
getpulse1::handle_pulse( romloader* loader, double tstates,
                         unsigned int pulse_length )
{
  getpulse2::instance()->set_first_pulse(tstates, pulse_length);
  loader->change_state(getpulse2::instance());
}
