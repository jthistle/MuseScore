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

#ifndef __TEMPOLINE_H__
#define __TEMPOLINE_H__

#include "tempo.h"
#include "textlinebase.h"

namespace Ms {

//---------------------------------------------------------
//   TempoLineSegment
//---------------------------------------------------------

class TempoLineSegment final : public TextLineBaseSegment {
   public:
      TempoLineSegment(Spanner* sp, Score* s);
      virtual ElementType type() const override          { return ElementType::TEMPOLINE_SEGMENT; }
      virtual TempoLineSegment* clone() const override   { return new TempoLineSegment(*this); }
      virtual void layout() override;
      virtual Element* propertyDelegate(Pid pid) override;
      };

//---------------------------------------------------------
//   TempoLine
//---------------------------------------------------------

class TempoLine final : public TextLineBase {
   private:
      qreal _tempoChange   { 0.0 };    // in beats per second
      bool  _aTempo        { true };

   public:
      TempoLine(Score* s);
      TempoLine(const TempoLine& tl);

      void updateScore();
      void updateTempoMap(TempoMap* tmap);

      virtual ElementType type() const override    { return ElementType::TEMPOLINE; }
      virtual TempoLine* clone() const override     { return new TempoLine(*this); }
      virtual LineSegment* createLineSegment() override;

      virtual QVariant getProperty(Pid id) const override;
      virtual bool setProperty(Pid propertyId, const QVariant&) override;
      virtual QVariant propertyDefault(Pid id) const override;

      virtual void write(XmlWriter& xml) const override;
      virtual void writeProperties(XmlWriter& xml) const override;
      };

}      // namespace Ms

#endif