/* getpulse2.h: ROM loader state looking for the second part of a data
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

#ifndef GETPULSE2_H
#define GETPULSE2_H

#include "romloader.h"
#include "romloaderstate.h"

class getpulse2 : public romloaderstate {
  public:
    static getpulse2* instance();
    void handle_pulse( romloader* loader, double tstates,
                       unsigned int pulse_length );

    void set_first_pulse( double tstates, int pulse );

  private:
    int first_pulse;
    double first_tstates;
    getpulse2();
    static getpulse2* unique_instance;
};

#endif /* #define GETPULSE2_H */
