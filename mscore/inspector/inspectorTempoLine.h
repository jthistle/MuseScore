//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2013 Werner Schweer and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENSE.GPL
//=============================================================================

#ifndef __INSPECTOR_TEMPOLINE_H__
#define __INSPECTOR_TEMPOLINE_H__

#include "inspectorTextLineBase.h"
#include "ui_inspector_tempoline.h"

namespace Ms {

//---------------------------------------------------------
//   InspectorTempoLine
//---------------------------------------------------------

class InspectorTempoLine : public InspectorTextLineBase {
      Q_OBJECT

      Ui::InspectorTempoLine tl;

   public:
      InspectorTempoLine(QWidget* parent);
      };

} // namespace Ms
#endif

