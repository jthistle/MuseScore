//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2019 Werner Schweer and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENSE.GPL
//=============================================================================

#include "tempoline.h"
#include "tempo.h"
#include "textlinebase.h"

namespace Ms {

//---------------------------------------------------------
//   TempoLineSegment
//---------------------------------------------------------

TempoLineSegment::TempoLineSegment(Spanner* sp, Score* s)
   : TextLineBaseSegment(sp, s, ElementFlag::MOVABLE | ElementFlag::ON_STAFF | ElementFlag::SYSTEM)
      {
      setPlacement(Placement::ABOVE);
      }

//---------------------------------------------------------
//   layout
//---------------------------------------------------------

void TempoLineSegment::layout()
      {
      TextLineBaseSegment::layout();
      autoplaceSpannerSegment(styleP(Sid::textLineMinDistance));
      }

//---------------------------------------------------------
//   TempoLine
//---------------------------------------------------------

TempoLine::TempoLine(Score* s)
   : TextLineBase(s, ElementFlags(ElementFlag::SYSTEM))
      {
      setPlacement(Placement::ABOVE);
      setBeginText("");
      setContinueText("");
      setEndText("");
      setBeginTextOffset(QPointF(0,0));
      setContinueTextOffset(QPointF(0,0));
      setEndTextOffset(QPointF(0,0));
      setLineVisible(true);

      setBeginHookType(HookType::NONE);
      setEndHookType(HookType::NONE);
      setBeginHookHeight(Spatium(1.5));
      setEndHookHeight(Spatium(1.5));

      resetProperty(Pid::BEGIN_TEXT_PLACE);
      resetProperty(Pid::CONTINUE_TEXT_PLACE);
      resetProperty(Pid::END_TEXT_PLACE);
      }

TempoLine::TempoLine(const TempoLine& tl)
   : TextLineBase(tl)
      {
      }

//---------------------------------------------------------
//   createLineSegment
//---------------------------------------------------------

LineSegment* TempoLine::createLineSegment()
      {
      TempoLineSegment* seg = new TempoLineSegment(this, score());
      seg->setTrack(track());
      if (anchor() == Spanner::Anchor::NOTE)
            seg->setFlag(ElementFlag::ON_STAFF, false);
      return seg;
      }

//---------------------------------------------------------
//   setProperty
//---------------------------------------------------------

bool TempoLine::setProperty(Pid id, const QVariant& v)
      {
      switch (id) {
            /*case Pid::TEMPO:
                  _tempo = v.toReal();
                  break;*/
            case Pid::TEMPO_CHANGE:
                  _tempoChange = v.toReal() / 60.0;
                  break;
            case Pid::A_TEMPO:
                  _aTempo = v.toBool();
                  break;
            default:
                  return TextLineBase::setProperty(id, v);
            }
      triggerLayout();
      return true;
      }

//---------------------------------------------------------
//   getProperty
//---------------------------------------------------------

QVariant TempoLine::getProperty(Pid id) const
      {
      switch (id) {
            /*case Pid::TEMPO:
                  return QVariant(_tempo);*/
            case Pid::TEMPO_CHANGE:
                  return QVariant(_tempoChange * 60.0);
            case Pid::A_TEMPO:
                  return QVariant(_aTempo);
            default:
                  return TextLineBase::getProperty(id);
            }
      }

//---------------------------------------------------------
//   getProperty
//---------------------------------------------------------

QVariant TempoLine::propertyDefault(Pid id) const
      {
      switch (id) {
            /*case Pid::TEMPO:
                  return QVariant(TempoMap::DEFAULT_TEMPO);*/
            case Pid::TEMPO_CHANGE:
                  return QVariant(0.0);
            case Pid::A_TEMPO:
                  return QVariant(true);
            default:
                  return TextLineBase::propertyDefault(id);
            }
      }

}

