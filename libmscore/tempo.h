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

#ifndef __AL_TEMPO_H__
#define __AL_TEMPO_H__

#include "changeMap.h"

namespace Ms {

class XmlWriter;

class TempoEvent : ChangeEvent {
   public:
      TempoEvent() : ChangeEvent() {}

      friend class ChangeMap<TempoEvent>;
      };


//---------------------------------------------------------
//   Tempomap
//---------------------------------------------------------

class TempoMap : public ChangeMap<TempoEvent> {
      qreal _relTempo   { 1.0 };

   public:
      TempoMap() {}

      int tempo(Fraction tick)      { return val(tick); }

      qreal relTempo() const  { return _relTempo; }
      void setRelTempo(qreal val)   { _relTempo = val; }

      qreal tick2time(Fraction tick, qreal time, int* sn) const;
      Fraction time2tick(qreal time, Fraction t, int* sn) const;
      qreal tick2time(Fraction tick, int* sn = 0) const;
      Fraction time2tick(qreal time, int* sn = 0) const;
      };

}     // namespace Ms
#endif
