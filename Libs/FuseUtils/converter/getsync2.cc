/* getsync2.h: ROM loader state looking for the sync2 pulse following the
               sync1 pulse
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
#include "getsync2.h"

getsync2* getsync2::unique_instance;

getsync2::getsync2()
{
  first_tstates = 0;
  first_pulse = 0;
}

getsync2*
getsync2::instance()
{
  if( unique_instance == 0 ) {
    unique_instance = new getsync2();
  }
  return unique_instance;
}

void
getsync2::handle_pulse( romloader* loader, double tstates,
                        unsigned int pulse_length )
{
  if ( first_pulse + pulse_length < loader->SYNC_TOTAL_MAX ) {
    loader->set_sync1_pulse( first_pulse );
    loader->set_sync2_pulse( pulse_length );
    loader->change_state( getpulse1::instance() );
  } else {
    // go back to looking for pilot tones
    loader->reset_pilot_count();
    loader->change_state( findpilot::instance() );
  }
}

void
getsync2::set_first_pulse ( double tstates, int pulse )
{
  first_tstates = tstates;
  first_pulse = pulse;
}
