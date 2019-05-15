//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2013 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENSE.GPL
//=============================================================================

#include "inspector.h"
#include "inspectorTempoLine.h"
#include "musescore.h"
#include "libmscore/score.h"

namespace Ms {

//---------------------------------------------------------
//   InspectorTextLine
//---------------------------------------------------------

InspectorTempoLine::InspectorTempoLine(QWidget* parent)
   : InspectorTextLineBase(parent)
      {
      tl.setupUi(addWidget());

      const std::vector<InspectorItem> il = {
            { Pid::PLACEMENT,     0, tl.placement,    tl.resetPlacement              },
            { Pid::TEMPO_CHANGE,  0, tl.tempoChange,  tl.resetTempoChange            },
            };
      const std::vector<InspectorPanel> ppList = {
            { tl.title, tl.panel },
            };

      populatePlacement(tl.placement);
      mapSignals(il, ppList);
      }
}

