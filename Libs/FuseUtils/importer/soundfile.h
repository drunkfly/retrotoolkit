/* soundfile.h: Convert audio files (wav, voc, etc.) to lists of pulses
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

#ifndef SOUNDFILE_H
#define SOUNDFILE_H

#include <list>
#include <string>
#include <memory>
#include <vector>

#include <libspectrum.h>

#include "trigger.h"

typedef std::list<unsigned int> pulse_list;

#if __cplusplus >= 201103L
  typedef std::unique_ptr<trigger> trigger_ptr;
#else
  typedef std::auto_ptr<trigger> trigger_ptr;
#endif

class soundfile {

  public:
    soundfile( std::string filename, trigger* edge_detector,
               double source_machine_hz, bool show_stats = false );
    virtual ~soundfile();

    const pulse_list& get_pulse_list();

    void display_stats();

    void get_tape_block( libspectrum_tape *tape, double start_tstates,
                         double end_tstates );

    double getSampleRate() {
      return sample_rate;
    }

  private:
    char low;
    char high;
    pulse_list pulses;
    double source_machine_hz;
    double sample_rate;

    trigger_ptr edge_detector;
    std::vector<unsigned int> amplitude_frequency_table;
};

#endif /* #define SOUNDFILE_H */
