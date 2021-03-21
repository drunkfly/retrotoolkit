/* findsync1.cc: ROM loader state searching for the sync1 pulse following
                 the initial pilot pulses
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

#include <iostream>

#include "findpilot.h"
#include "findsync1.h"
#include "getsync2.h"

findsync1* findsync1::unique_instance;

findsync1*
findsync1::instance()
{
  if( unique_instance == 0 ) {
    unique_instance = new findsync1();
  }
  return unique_instance;
}

void
findsync1::handle_pulse( romloader* loader, double tstates,
                         unsigned int pulse_length )
{
  // look for pulse no longer than SYNC_TOTAL_MAX, but less than SYNC1_MAX
  if( pulse_length > loader->SYNC_TOTAL_MAX ) {
    // go back to looking for pilot tones
    loader->reset_pilot_count();
    loader->change_state( findpilot::instance() );
  } else if ( pulse_length < loader->SYNC1_MAX ) {
    // get sync2
    getsync2::instance()->set_first_pulse( tstates, pulse_length );
    loader->change_state( getsync2::instance() );
  } else {
    loader->add_pilot_pulse( tstates, pulse_length );
  }
}
