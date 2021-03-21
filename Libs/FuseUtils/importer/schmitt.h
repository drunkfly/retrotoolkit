/* schmitt.h: Schmitt trigger class for detecting the transition between a
              level 0 and 1
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

#ifndef SCHMITT_H
#define SCHMITT_H

#include <libspectrum.h>

#include "audio2tape.h"
#include "importer/trigger.h"

class schmitt : public trigger {
  public:
    schmitt( libspectrum_byte low, libspectrum_byte high ) : 
    low_threshold(low),
    high_threshold(high) {
      if( low > high )
        throw audio2tape_exception("invalid schmitt trigger values");
      current_level = 0;
    }

    ~schmitt() {}

    libspectrum_byte get_current_level() {
      return current_level;
    }

    void set_current_level( libspectrum_byte val ) {
      current_level = val;
    }

    libspectrum_byte get_level(libspectrum_byte b) {
      if ( current_level == 0 ) {
        if( b > high_threshold ) {
          current_level = 1;
        }
      } else {
        if( b < low_threshold ) {
          current_level = 0;
        }
      }
      return current_level;
    }

  private:
    libspectrum_byte low_threshold;
    libspectrum_byte high_threshold;
    libspectrum_byte current_level;
};

#endif /* #ifndef SCHMITT_H */
