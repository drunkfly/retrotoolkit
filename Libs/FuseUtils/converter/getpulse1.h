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

#ifndef GETPULSE1_H
#define GETPULSE1_H

#include "romloader.h"
#include "romloaderstate.h"

class getpulse1 : public romloaderstate {
  public:
    static getpulse1* instance();
    void handle_pulse( romloader* loader, double tstates,
                       unsigned int pulse_length );
  private:
    getpulse1() {}
    static getpulse1* unique_instance;
};

#endif /* #define GETPULSE1_H */
