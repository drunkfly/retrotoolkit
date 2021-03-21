/* findsync1.h: ROM loader state searching for the sync1 pulse following
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

#ifndef FINDSYNC1_H
#define FINDSYNC1_H

#include "romloader.h"
#include "romloaderstate.h"

class findsync1 : public romloaderstate {
  public:
    static findsync1* instance();
    void handle_pulse( romloader* loader, double tstates,
                       unsigned int pulse_length );
  private:
    findsync1() {}
    static findsync1* unique_instance;
};

#endif /* #define FINDSYNC1_H */
