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

#include "score.h"
#include "segment.h"
#include "tempoline.h"
#include "tempo.h"
#include "textlinebase.h"

namespace Ms {

static const ElementStyle tempoLineStyle {
      { Sid::tempoLineFontFace,                   Pid::BEGIN_FONT_FACE         },
      { Sid::tempoLineFontFace,                   Pid::CONTINUE_FONT_FACE      },
      { Sid::tempoLineFontFace,                   Pid::END_FONT_FACE           },
      { Sid::tempoLineFontSize,                   Pid::BEGIN_FONT_SIZE         },
      { Sid::tempoLineFontSize,                   Pid::CONTINUE_FONT_SIZE      },
      { Sid::tempoLineFontSize,                   Pid::END_FONT_SIZE           },
      { Sid::tempoLineFontStyle,                  Pid::BEGIN_FONT_STYLE        },
      { Sid::tempoLineFontStyle,                  Pid::CONTINUE_FONT_STYLE     },
      { Sid::tempoLineFontStyle,                  Pid::END_FONT_STYLE          },
      { Sid::tempoLineTextAlign,                  Pid::BEGIN_TEXT_ALIGN        },
      { Sid::tempoLineTextAlign,                  Pid::CONTINUE_TEXT_ALIGN     },
      { Sid::tempoLineTextAlign,                  Pid::END_TEXT_ALIGN          },
      { Sid::tempoLinePlacement,                  Pid::PLACEMENT               },
      };

//---------------------------------------------------------
//   TempoLineSegment
//---------------------------------------------------------

TempoLineSegment::TempoLineSegment(Spanner* sp, Score* s)
   : TextLineBaseSegment(sp, s, ElementFlag::MOVABLE | ElementFlag::ON_STAFF | ElementFlag::SYSTEM)
      {
      }

//---------------------------------------------------------
//   propertyDelegate
//---------------------------------------------------------

static const std::array<Pid, 2> pids { {
      Pid::TEMPO_CHANGE,
      Pid::A_TEMPO,
      } };

Element* TempoLineSegment::propertyDelegate(Pid pid)
      {
      for (Pid id : pids) {
            if (pid == id)
                  return spanner();
            }
      return TextLineBaseSegment::propertyDelegate(pid);
      }

//---------------------------------------------------------
//   layout
//---------------------------------------------------------

void TempoLineSegment::layout()
      {
      TextLineBaseSegment::layout();
      autoplaceSpannerSegment();
      }

//---------------------------------------------------------
//   TempoLine
//---------------------------------------------------------

TempoLine::TempoLine(Score* s)
   : TextLineBase(s, ElementFlags(ElementFlag::SYSTEM))
      {
      initElementStyle(&tempoLineStyle);

      setBeginText("");
      setContinueText("");
      setEndText("");
      setBeginTextOffset(QPointF(0, 0));
      setContinueTextOffset(QPointF(0, 0));
      setEndTextOffset(QPointF(1.5 * SPATIUM20, 0));

      setLineVisible(true);
      setLineStyle(Qt::DashLine);

      setBeginHookType(HookType::NONE);
      setEndHookType(HookType::NONE);
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
//   updateTempoMap
//---------------------------------------------------------

void TempoLine::updateTempoMap(TempoMap* tmap)
      {
#if 0 // NOTE:JT todo
      qDebug("Tempo line: update map");
      if (startSegment() && endSegment()) {
            int stick = tick().ticks();
            int etick = tick2().ticks(); // endSegment()->next1() ? endSegment()->next1()->tick().ticks() : score()->endTick().ticks();

            tmap->setTempo(etick, _tempoChange, stick);

            // Add other events, linked to the first event
            if (endSegment()) {
                  /*if (_aTempo) {
                        score()->tempomap()->setTempo(endTick, startTempo, false, stick);
                        }*/
                  }
            }
      qDebug("Tempo line: finished update");
#endif
      }

void TempoLine::updateScore()
      {
      qDebug("Tempo line: update score and fix ticks");
      updateTempoMap(score()->tempomap());
      score()->fixTicks();
      }

//---------------------------------------------------------
//   write
//---------------------------------------------------------

void TempoLine::write(XmlWriter& xml) const
      {
      if (!xml.canWrite(this))
            return;
      xml.stag(this);
      writeProperties(xml);
      xml.etag();
      }

//---------------------------------------------------------
//   writeProperties
///   write properties different from prototype
//---------------------------------------------------------

void TempoLine::writeProperties(XmlWriter& xml) const
      {
      for (Pid pid : pids) {
            if (!isStyled(pid))
                  writeProperty(xml, pid);
            }
      TextLineBase::writeProperties(xml);
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
                  _tempoChange = v.toReal();
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
                  return QVariant(_tempoChange);
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

