//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2009-2011 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#ifndef __VELO_H__
#define __VELO_H__

#include "fraction.h"

/**
 \file
 Definition of classes VeloList.
*/

namespace Ms {

//---------------------------------------------------------
///   VeloEvent
///   item in VeloList
//---------------------------------------------------------

enum class VeloChangeMethod : signed char;

enum class VeloType : signed char {
      FIXED,
      RAMP,
      DYNAMIC     // changing dynamic only
      };

struct VeloEvent {
      VeloType type;
      int val;
      int val2;
      Fraction duration;
      VeloChangeMethod method;
      VeloEvent() {}
      VeloEvent(int v) : val(v) { type = VeloType::FIXED; }
      VeloEvent(int v1, int v2, Fraction d, VeloChangeMethod m) : val(v1), val2(v2), duration(d), method(m) { type = VeloType::RAMP; }
      VeloEvent(int v1, int v2, Fraction d) : val(v1), val2(v2), duration(d) { type = VeloType::DYNAMIC; }
      };

//---------------------------------------------------------
///  VeloList
///  List of note velocity changes
//---------------------------------------------------------

class VeloList : public QMap<Fraction, VeloEvent> {
   public:
      VeloList() {}
      int velo(const Fraction& tick) const;
      int prevVelo(const Fraction& tick) const;
      int nextVelo(const Fraction& tick) const;
      std::vector<Fraction> findOverlapping(Fraction& tick1, Fraction& tick2) const;
      void setVelo(Fraction tick, VeloEvent velo);
      void setDynamic(Fraction tick, int velocity);
      void setDynamic(Fraction tick, Fraction duration, int startVelocity, int endVelocity);
      void setHairpin(Fraction tick, Fraction duration, VeloChangeMethod method, int startVelocity = -1, int endVelocity = -1);

      void debug() const;

      static int interpolate(Fraction tick, Fraction duration, int startVel, int endVel, VeloChangeMethod changeMethod);
      static int interpolateDynamic(Fraction tick, Fraction duration, int startVel, int endVel);
      };


}     // namespace Ms
#endif

