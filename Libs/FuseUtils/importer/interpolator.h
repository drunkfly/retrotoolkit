/* interpolator.h: Does a linear interpolation between the points in the
                   supplied samples to a TZX-friendly 3.5MHz
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

#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include <math.h>

#include <libspectrum.h>

#include "importer/soundfile.h"

class interpolator {
public:
  interpolator( libspectrum_byte *samples, size_t samples_length, double rate,
                double source_rate ) :
    source_sample_rate(source_rate),
    sample_rate(rate),
    pulses(samples),
    source_length(samples_length) {
      length = floor( source_length * source_sample_rate/sample_rate );
    }

  double get_length() { return length; }

  libspectrum_byte get_sample( double tstates ) {
    // convert tstates to samples offsets
    double tstate_offset = tstates * sample_rate/source_sample_rate;
    size_t offset1 = (size_t)floor(tstate_offset);
    size_t offset2 = (size_t)ceil(tstate_offset);

    double distance = tstate_offset - offset1;

    // get samples
    libspectrum_byte sample1 = pulses[offset1];
    if( offset2 >= source_length ) offset2 = source_length - 1;
    libspectrum_byte sample2 = pulses[offset2];

    // if we have two samples, allocate final value based on
    // distance between both samples
    int difference = sample2 - sample1;

    return (libspectrum_byte) (sample1 + (difference * distance));
  }

private:
  double source_sample_rate;
  double sample_rate;
  double length;
  libspectrum_byte *pulses;
  size_t source_length;
};

#endif /* #define INTERPOLATOR_H */
