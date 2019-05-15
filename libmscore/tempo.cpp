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
//   TEvent
//---------------------------------------------------------

TEvent::TEvent()
      {
      type     = TempoType::INVALID;
      tempo    = 0.0;
      pause    = 0.0;
      startTick = -1;
      }

TEvent::TEvent(const TEvent& e)
      {
      type  = e.type;
      tempo = e.tempo;
      pause = e.pause;
      time  = e.time;
      startTick = e.startTick;
      }

TEvent::TEvent(qreal t, qreal p, TempoType tp, int s)
      {
      type  = tp;
      tempo = t;
      pause = p;
      time  = 0.0;
      startTick = s;
      }

bool TEvent::valid() const
      {
      return !(!type);
      }

//---------------------------------------------------------
//   TempoMap
//---------------------------------------------------------

TempoMap::TempoMap()
      {
      _tempo    = DEFAULT_TEMPO;        // default fixed tempo in beat per second
      _tempoSN  = 1;
      _relTempo = 1.0;
      }

//---------------------------------------------------------
//   setPause
//---------------------------------------------------------

void TempoMap::setPause(int tick, qreal pause)
      {
      auto e = find(tick);
      if (e != end()) {
            e->second.pause = pause;
            e->second.type |= TempoType::PAUSE;
            }
      else {
            qreal t = tempo(tick);
            insert(std::pair<const int, TEvent> (tick, TEvent(t, pause, TempoType::PAUSE, -1)));
            }
      normalize();
      }

//---------------------------------------------------------
//   setTempo
//---------------------------------------------------------

void TempoMap::setTempo(int tick, qreal tempo, int startTick /*=-1*/)
      {
      qDebug("Before adding:");
      dump();
      qDebug("setting tempo %f at %d, startTick %d", tempo, tick, startTick);
      TempoType ttype = startTick > -1 ? TempoType::RAMP : TempoType::FIX;

      auto e = find(tick);
      if (e != end()) {
            qDebug("There is already an event here");
            e->second.tempo = tempo;
            e->second.type |= ttype;
            if (startTick > -1)
                  e->second.startTick = startTick; 
            }
      else
            insert(std::pair<const int, TEvent> (tick, TEvent(tempo, 0.0, ttype, startTick)));

      normalize();
      qDebug("Now is:");
      dump();
      }

//---------------------------------------------------------
//   TempoMap::normalize
//---------------------------------------------------------

void TempoMap::normalize()
      {
      qreal time  = 0;
      int tick    = 0;
      qreal tempo = DEFAULT_TEMPO;
      for (auto e = begin(); e != end(); ++e) {
            // entries that represent a pause *only* (not tempo change also)
            // need to be corrected to continue previous tempo
            if (!(e->second.type & (TempoType::FIX|TempoType::RAMP)))
                  e->second.tempo = tempo;
            int delta = e->first - tick;
            if (e->second.isRamp()) {
                  // Create a temporary event to pass needed parameters
                  TEvent tempEvent = TEvent(tempo, 0.0, TempoType::FIX, -1);
                  time += rampTime(e->first - tick, e->first, e->second, tick, tempEvent);
                  }
            else
                  time += qreal(delta) / (MScore::division * tempo * _relTempo);

            time += e->second.pause;
            e->second.time = time;
            tick = e->first;
            if (e->second.isRamp()) {
                  tempo += e->second.tempo;
                  }
            else {
                  tempo = e->second.tempo;
                  }
            }
      ++_tempoSN;
      }

//---------------------------------------------------------
//   TempoMap::rampTime
///   interpolates the time for a ramp
//---------------------------------------------------------

qreal TempoMap::rampTime(int delta, int etick, TEvent& e, int stick, TEvent& pe) const
      {
      // stick - the tick of the first tempo event before the ramp event
      // etick - the tick of the ramp event
      // delta - the progress since stick in ticks
      // e     - the tempo event reference
      // pe    - the previous tempo event reference
      // TODO - first interpolate linearly up until start tick

      int rampStick = e.startTick;
      if (stick + delta <= rampStick) {
            // linearly interpolate
            return qreal(delta) / (MScore::division * pe.tempo * _relTempo);
            }
      
      // The time taken on the no tempo change section
      qreal linearTime = qreal(rampStick - stick) / (MScore::division * pe.tempo * _relTempo);

      // New delta - we're calculating changing section
      qreal useTempo = MScore::division * pe.tempo * _relTempo;
      // Here, the e.tempo is a tempo change rather than an absolute tempo
      qreal finalUseTempo = MScore::division * (pe.tempo + e.tempo) * _relTempo;

      int searchTick = stick + delta; 
      // We can visualise time taken as a graph of 1/tempo against delta. The time taken
      // is the area under the line, hence this equation.
      qreal totalTime = linearTime + 0.5 * (1/finalUseTempo + 1/useTempo) * qreal(searchTick - rampStick);

      return totalTime;
      }

//---------------------------------------------------------
//   TempoMap::dump
//---------------------------------------------------------

void TempoMap::dump() const
      {
      qDebug("\nTempoMap:");
      for (auto i = begin(); i != end(); ++i)
            qDebug("%6d type: %2d tempo (change): %f pause: %f time: %f start tick: %d",
               i->first, static_cast<int>(i->second.type), i->second.tempo, i->second.pause, i->second.time, i->second.startTick);
      }

//---------------------------------------------------------
//   clear
//---------------------------------------------------------

void TempoMap::clear()
      {
      qDebug("clearing entire map");
      std::map<int,TEvent>::clear();
      ++_tempoSN;
      }

//---------------------------------------------------------
//   clearRange
//    Clears the given range, start tick included, end tick
//    excluded.
//---------------------------------------------------------

void TempoMap::clearRange(int tick1, int tick2)
      {
      qDebug("clearing %d - %d", tick1, tick2);
      iterator first = lower_bound(tick1);
      iterator last = lower_bound(tick2);
      if (first == last)
            return;
      erase(first, last);
      ++_tempoSN;
      }

//---------------------------------------------------------
//   tempo
//---------------------------------------------------------

qreal TempoMap::tempo(int tick) const
      {
      qDebug("searching tempo at %d", tick);
      if (empty())
            return DEFAULT_TEMPO;
      auto i = lower_bound(tick);
      if (i == end()) {
            --i;
            qDebug("At end, returning %f", i->second.tempo);
            return i->second.tempo;
            }
      qDebug("(i tempo = %f)", i->second.tempo);
      if (i->first == tick) {
            qDebug("Exact (%d == %d) - returning %f", i->first, tick, i->second.tempo);
            return i->second.tempo;
            }
      if (i == begin()) {
            qDebug("At start - returning default");
            return DEFAULT_TEMPO;
            }
      // Check if the event after tick is a ramp event
      bool ramp = false;
      int  stick = -1;
      int  etick = -1;
      qreal tempoChange = -1;
      if (i->second.isFix()) {
            qDebug("event at %d, is fix", i->first);
            }
      else if (i->second.isRamp() && i->second.startTick <= tick) {
            ramp = true;
            stick = i->second.startTick;
            etick = i->first;
            tempoChange = i->second.tempo;
            qDebug("It's a ramp! etick %d, delta tempo %f", etick, tempoChange);
            }

      // Go back the the event before tick
      --i;

      // Linearly interpolate if necessary
      if (ramp) {
            qreal progress = qreal(tick - stick)/(etick - stick);
            qDebug("Ramp: progress %f gives %f", progress, i->second.tempo + tempoChange * progress);
            return i->second.tempo + tempoChange * progress;
            }

      qDebug("return normal");
      return i->second.tempo;
      }

//---------------------------------------------------------
//   del
//---------------------------------------------------------

void TempoMap::del(int tick)
      {
      auto e = find(tick);
      if (e == end()) {
            qDebug("TempoMap::del event at (%d): not found", tick);
            // abort();
            return;
            }
      // don't delete event if still being used for pause
      if (e->second.type & TempoType::PAUSE)
            e->second.type = TempoType::PAUSE;
      else {
            erase(e);
            }
      normalize();
      }

//---------------------------------------------------------
//   setRelTempo
//---------------------------------------------------------

void TempoMap::setRelTempo(qreal val)
      {
      _relTempo = val;
      normalize();
      }

//---------------------------------------------------------
//   delTempo
//---------------------------------------------------------

void TempoMap::delTempo(int tick)
      {
      del(tick);
      ++_tempoSN;
      }

//---------------------------------------------------------
//   tick2time
//---------------------------------------------------------

qreal TempoMap::tick2time(int tick, qreal time, int* sn) const
      {
      return (*sn == _tempoSN) ? time : tick2time(tick, sn);
      }

//---------------------------------------------------------
//   time2tick
//    return cached value t if list did not change
//---------------------------------------------------------

int TempoMap::time2tick(qreal time, int t, int* sn) const
      {
      return (*sn == _tempoSN) ? t : time2tick(time, sn);
      }

//---------------------------------------------------------
//   tick2time -- NOTE:JT todo ramp
//---------------------------------------------------------

qreal TempoMap::tick2time(int tick, int* sn) const
      {
      qreal time  = 0.0;
      qreal delta = qreal(tick);
      qreal tempo = DEFAULT_TEMPO;

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
      }

//---------------------------------------------------------
//   time2tick
//---------------------------------------------------------

int TempoMap::time2tick(qreal time, int* sn) const
      {
      int tick     = 0;
      qreal delta = time;
      qreal tempo = _tempo;

      delta = 0.0;
      tempo = DEFAULT_TEMPO;
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
      }

}

