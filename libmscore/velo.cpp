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

/**
 \file
 Implementation of class VeloList.
*/

#include "velo.h"
#include "hairpin.h"
#include "dynamic.h"

namespace Ms {

static const int DEFAULT_VELO = 80;

//---------------------------------------------------------
//   interpolate
//    Interpolate to find the velocity at tick
//---------------------------------------------------------

int VeloList::interpolate(Fraction tick, Fraction duration, int startVel, int endVel, VeloChangeMethod changeMethod)
      {
      // Prevent possible errors
      if (startVel == endVel || duration.ticks() == 0)
            return startVel;

      // Ticks to change expression over
      int exprTicks = duration.ticks();
      int exprDiff = endVel - startVel;
      int ct = tick.ticks();

      // The functions are in the same format as in rendermidi
      switch (changeMethod) {
            case VeloChangeMethod::EXPONENTIAL:
                  if (exprDiff > 0) {
                        return startVel + int(
                              pow(
                                    pow((exprDiff + 1), 1.0 / double(exprTicks)),
                                    double(ct)
                                    ) - 1
                              );
                        }
                  else {
                        return startVel - int(
                              pow(
                                    pow((-exprDiff + 1), 1.0 / double(exprTicks)),
                                    double(ct)
                                    ) + 1
                              );
                        }
            case VeloChangeMethod::EASE_IN_OUT:
                  return startVel + int(
                        (double(exprDiff) / 2.0) * (
                              sin(
                                    double(ct) * (
                                          double(M_PI / double(exprTicks))
                                          ) - double(M_PI / 2.0)
                                    ) + 1
                              )
                        );
            case VeloChangeMethod::EASE_IN:
                  return startVel + int(
                        double(exprDiff) * (
                              sin(
                                    double(ct - double(exprTicks)) * (
                                          double(M_PI / double(2 * exprTicks))
                                          )
                                    ) + 1
                              )
                        );
            case VeloChangeMethod::EASE_OUT:
                  return startVel + int(
                        double(exprDiff) * sin(
                              double(ct) * (
                                    double(M_PI / double(2 * exprTicks))
                                    )
                              )
                        );
            case VeloChangeMethod::NORMAL:
            default:
                  return startVel + int(double(exprDiff) * (double(ct) / double(exprTicks)));
                  break;
            }
      }

//---------------------------------------------------------
//   interpolateDynamic
//    Interpolate a dynamic to find the velocity at tick
//---------------------------------------------------------

int VeloList::interpolateDynamic(Fraction tick, Fraction duration, int startVel, int endVel)
      {
      // Prevent possible errors
      if (startVel == endVel)
            return startVel;

      // Determine how long to 'hold' the initial velocity
      Fraction holdEnd = duration / Fraction(4, 1);

      // If we're in the hold section, return the start velocity
      if (tick < holdEnd)
            return startVel;

      // Otherwise interpolate linearly
      return startVel + ((tick - holdEnd) / (duration - holdEnd)).ticks() * (endVel - startVel);
      }


//---------------------------------------------------------
//   prevVelo
//    return last velocity event before tick position
//---------------------------------------------------------

int VeloList::prevVelo(const Fraction& tick) const
      {
      if (empty())
            return DEFAULT_VELO;

      VeloList::const_iterator i = lowerBound(tick);
      if (i == begin())
            return DEFAULT_VELO;

      i--;
      if (i != begin()) {
            const VeloEvent& event = i.value();

            // Special case, check if we're looking after a hairpin/dynamic with an end velocity
            if (event.type == VeloType::RAMP && event.val2 != -1 && tick >= i.key() + event.duration)
                  return event.val2;
            else if (event.type == VeloType::DYNAMIC && tick >= i.key() + event.duration)
                  return event.val2;

            if (event.val != -1)
                  return event.val;
            else
                  return prevVelo(i.key());
            }
      else
            return DEFAULT_VELO;
      }

//---------------------------------------------------------
//   nextVelo
//    return next velocity event after tick position
//---------------------------------------------------------

int VeloList::nextVelo(const Fraction& tick) const
      {
      if (empty())
            return DEFAULT_VELO;

      VeloList::const_iterator i = upperBound(tick);
      if (i != end() && i != begin()) {
            const VeloEvent& event = i.value();
            if (event.val != -1)
                  return event.val;
            else
                  return nextVelo(i.key());
            }
      else
            return DEFAULT_VELO;
      }

//---------------------------------------------------------
//   velo
//    return velocity at tick position
//---------------------------------------------------------

int VeloList::velo(const Fraction& tick) const
      {
      qDebug() << "looking for velo at" << tick;
      if (empty())
            return DEFAULT_VELO;

      // Check if no velocity set before tick
      VeloList::const_iterator i = upperBound(tick);
      i--;
      if (i == begin()) {
            qDebug("at beginning, returning default");
            return DEFAULT_VELO;
            }

      if (i.value().type == VeloType::FIXED) {
            qDebug("fixed, val %d", i.value().val);
            return i.value().val;
            }
      
      auto const& event = i.value();
      Fraction eventTick = i.key();
      Fraction tickProg = tick - eventTick;
      Fraction dur = event.duration;
      int start = event.val;
      int end = event.val2;

      qDebug() << "hairpin or dynamic at" << eventTick;
      if (start == -1)
            start = prevVelo(eventTick);
      if (end == -1)
            end = nextVelo(eventTick);
      qDebug("s: %d, e: %d", start, end);
      qDebug() << "tick prog" << tickProg;

      if (i.value().type == VeloType::DYNAMIC) {
            int val = interpolateDynamic(tickProg, dur, start, end); 
            qDebug("interpolating dynamic, %d", val);
            return val;
            }

      VeloChangeMethod method = event.method;

      int val = interpolate(tickProg, dur, start, end, method);
      qDebug("interpolating hairpin, %d", val);
      return val;
      }

//---------------------------------------------------------
//   findOverlapping
//    return next velocity event after tick position
//---------------------------------------------------------

std::vector<Fraction> VeloList::findOverlapping(Fraction& tick1, Fraction& tick2) const
      {
      std::vector<Fraction> matches;

      for (VeloList::const_iterator iter = begin(); iter != end(); ++iter) {
            Fraction st = iter.key();
            Fraction et = iter.key() + iter.value().duration;
            if (st > tick2)
                  break;

            if ((st <= tick1 && et >= tick1) || (st >= tick1 && st <= tick2))
                  matches.push_back(st);
            }

      return matches;
      }

//---------------------------------------------------------
//   setVelo - TODO phase out
//---------------------------------------------------------

void VeloList::setVelo(Fraction tick, VeloEvent ve)
      {
      insert(tick, ve);
      }

//---------------------------------------------------------
//   setDynamic
//---------------------------------------------------------

// Static dynamic
void VeloList::setDynamic(Fraction tick, int velocity)
      {
      qDebug() << "want to add dynamic at" << tick << ", with vel" << velocity;
      std::vector<Fraction> overlap = findOverlapping(tick, tick);
      for (Fraction& st : overlap) {
            // Don't add a dynamic if it overlaps a hairpin end/middle
            // but set a hairpin end if this is on a dynamic end
            VeloEvent& event = find(st).value();
            if (event.type == VeloType::RAMP && st + event.duration == tick) {
                  qDebug("setting end of hairpin from fixed");
                  event.val2 = velocity;
                  return;
                  }
            else if (event.type == VeloType::RAMP && st < tick) {
                  qDebug("hairpin starts before this dynamic");
                  return;
                  }

            // Or if it overlaps another dynamic, don't add
            if (event.type == VeloType::FIXED) {
                  qDebug("dynamic overlaps another fixed");
                  return;
                  }

            // Set hairpin start velocity
            if (event.type == VeloType::RAMP) {
                  qDebug("setting start of hairpin from fixed");
                  event.val = velocity;
                  }

            // End previous dynamic sooner
            else if (event.type == VeloType::DYNAMIC) {
                  qDebug("ending prev dynamic sooner");
                  event.duration = tick - st - Fraction::fromTicks(1); 
                  }
            }
      insert(tick, VeloEvent(velocity));
      }

// Changing dynamic
void VeloList::setDynamic(Fraction tick, Fraction duration, int startVelocity, int endVelocity)
      {
      qDebug() << "want to add changing dynamic at" << tick << "to" << tick+duration << ", with vels" << startVelocity << "," << endVelocity;
      Fraction etick = tick + duration;
      std::vector<Fraction> overlap = findOverlapping(tick, etick);
      for (Fraction& st : overlap) {
            VeloEvent& event = find(st).value();
            // Don't add a dynamic if it overlaps a hairpin middle
            if (event.type == VeloType::RAMP && st < tick && st + event.duration > tick) {
                  qDebug("dynamic overlaps hairpin middle");
                  return;
                  }

            // Don't add if we already have a fixed dynamic
            if (event.type == VeloType::FIXED) {
                  qDebug("dynamic overlaps fixed dynamic");
                  return;
                  }

            if (event.type == VeloType::RAMP) {
                  // Nudge hairpin up so that it comes after dynamic
                  Fraction newTick = tick + duration + Fraction::fromTicks(1);
                  qDebug() << "nudge hairpin up from" << st << "to" << newTick;
                  event.duration -= duration + Fraction::fromTicks(1);
                  event.val = endVelocity;
                  if (find(newTick) == end())
                        insert(newTick, event);
                  }

            // End previous dynamic sooner
            else if (event.type == VeloType::DYNAMIC) {
                  event.duration = tick - st - Fraction::fromTicks(1); 
                  qDebug() << "ending previous dynamic at" << st << "sooner";
                  //return;
                  }
            }
      insert(tick, VeloEvent(startVelocity, endVelocity, duration));
      }

//---------------------------------------------------------
//   setHairpin
//---------------------------------------------------------

void VeloList::setHairpin(Fraction tick, Fraction duration, VeloChangeMethod method, int startVelocity /*=-1*/, int endVelocity /*=-1*/)
      {
      qDebug() << "want to add hairpin at" << tick << "to" << tick+duration << ", with vels" << startVelocity << "," << endVelocity;
      Fraction etick = tick + duration;
      std::vector<Fraction> overlap = findOverlapping(tick, etick);
      for (Fraction& st : overlap) {
            VeloEvent& event = find(st).value();

            // Don't allow overlapping of any hairpin
            if (event.type == VeloType::RAMP) {
                  qDebug("overlaps hairpin");
                  return;
                  }

            // If fixed dynamic here, set the hairpin's start velocity
            // or if it overlaps at the end, the end velocity
            if (event.type == VeloType::FIXED) {
                  if (st == tick) {
                        qDebug("setting start %d from dynamic...", event.val);
                        startVelocity = event.val;
                        }
                  else if (st == etick) {
                        qDebug("setting end %d from dynamic...", event.val);
                        endVelocity = event.val;
                        }
                  
                  // hairpin overlaps dynamic in the middle
                  else {
                        qDebug() << "dynamic at" << st << "overlaps in the middle?!?";
                        }

                  
                  qDebug("... then removing that dynamic");
                  remove(st);
                  }

            // If there is a changing dynamic, put the hairpin after it
            else if (event.type == VeloType::DYNAMIC) {
                  Fraction tickDiff = event.duration - (tick - st);
                  qDebug() << "Dynamic with a tickdiff" << tickDiff;

                  // Don't allow a changing dynamic overlapping inside hairpin
                  if (tickDiff.ticks() < 0) {
                        qDebug("changing dynamic overlaps potential hairpin");
                        return;
                        }

                  tick = tick + tickDiff + Fraction::fromTicks(1);
                  // Recurse, since overlapping may have changed now
                  if ((duration - event.duration).ticks() > 0) {
                        qDebug() << "recursively adding hairpin at" << tick;
                        setHairpin(tick, duration - event.duration - Fraction::fromTicks(1), method, event.val2, endVelocity);
                        }
                  return;
                  }
            }

      insert(tick, VeloEvent(startVelocity, endVelocity, duration, method));
      }

void VeloList::debug() const
      {
      qDebug("\n=== BEGIN VELOCITY MAP");
      for (VeloList::const_iterator i = begin(); i != end(); i++) {
            const VeloEvent& event = i.value(); 
            if (event.type == VeloType::FIXED) {
                  qDebug() << "Fixed dynamic at" << i.key().ticks() << ", vel:" << event.val;
                  }
            else if (event.type == VeloType::RAMP) {
                  int startVel = event.val;
                  int endVel   = event.val2;
                  if (startVel == -1)
                        startVel = velo(i.key());
                  if (endVel == -1) {
                        Fraction endTick = i.key() + event.duration;
                        endVel = velo(endTick);
                        }

                  qDebug() << "Hairpin from" << i.key().ticks() << "to" << (i.key() + event.duration).ticks() << ", vel:" << startVel << (event.val == -1 ? "(-1)" : "") << "to" << endVel << (event.val2 == -1 ? "(-1)" : "");
                  }
            else if (event.type == VeloType::DYNAMIC) {
                  qDebug() << "Dynamic from" << i.key().ticks() << "to" << (i.key() + event.duration).ticks() << ", vel:" << event.val << "to" << event.val2;
                  }
            }
      qDebug("\n=== END VELOCITY MAP\n");
      }
}

