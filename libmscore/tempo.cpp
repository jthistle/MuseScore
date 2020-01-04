//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2002-2011 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#include "tempo.h"
#include "xml.h"

namespace Ms {

//---------------------------------------------------------
//   tick2time
//---------------------------------------------------------

qreal TempoMap::tick2time(Fraction tick, qreal time, int* sn) const
      {
      return 0;
      // return (*sn == _tempoSN) ? time : tick2time(tick, sn);
      }

//---------------------------------------------------------
//   time2tick
//    return cached value t if list did not change
//---------------------------------------------------------

Fraction TempoMap::time2tick(qreal time, Fraction t, int* sn) const
      {
      return Fraction();
      // return (*sn == _tempoSN) ? t : time2tick(time, sn);
      }

//---------------------------------------------------------
//   tick2time -- NOTE:JT todo ramp
//---------------------------------------------------------

qreal TempoMap::tick2time(Fraction tick, int* sn) const
      {
      return 0;
#if 0
      qreal time  = 0.0;
      qreal delta = qreal(tick);
      qreal tempo = Score::defaultTempo();

      if (!empty()) {
            int ptick  = 0;
            auto e = lower_bound(tick);
            if (e == end()) {
                  auto pe = e;
                  --pe;
                  ptick = pe->first;
                  tempo = pe->second.tempo;
                  time  = pe->second.time;
                  }
            else if (e->first == tick) {
                  ptick = tick;
                  tempo = e->second.tempo;
                  time  = e->second.time;
                  }
            else if (e != begin()) {
                  auto pe = e;
                  --pe;
                  ptick = pe->first;
                  tempo = pe->second.tempo;
                  time  = pe->second.time;
                  }
            delta = qreal(tick - ptick);
            }
      else
            qDebug("TempoMap: empty");
      if (sn)
            *sn = _tempoSN;
      time += delta / (MScore::division * tempo * _relTempo);
      return time;
#endif
      }

//---------------------------------------------------------
//   time2tick
//---------------------------------------------------------

Fraction TempoMap::time2tick(qreal time, int* sn) const
      {
      return Fraction();
#if 0
      int tick     = 0;
      qreal delta = time;
      qreal tempo = _tempo;

      delta = 0.0;
      tempo = Score::defaultTempo();
      for (auto e = begin(); e != end(); ++e) {
            // if in a pause period, wait on previous tick
            if ((time <= e->second.time) && (time > e->second.time - e->second.pause)) {
                  delta = (time - (e->second.time - e->second.pause) + delta);
                  break;
                  }
            if (e->second.time >= time)
                  break;
            delta = e->second.time;
            tick  = e->first;
            tempo = e->second.tempo;
            }
      delta = time - delta;
      tick += lrint(delta * _relTempo * MScore::division * tempo);
      if (sn)
            *sn = _tempoSN;
      return tick;
#endif
      }

}

