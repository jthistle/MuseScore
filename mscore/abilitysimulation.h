#ifndef ABILITYSIMULATION_H
#define ABILITYSIMULATION_H

#include "libmscore/score.h"
#include "ui_abilitysimulation.h"

namespace Ms {

class MasterScore;

class AbilitySimulation : public QDialog, private Ui::AbilitySimulation{
      Q_OBJECT
      MasterScore* _score;

      virtual void hideEvent(QHideEvent*);

private slots:
      void useAbilityChanged(bool);
      void updateAbilityLevel(int);
      void doApplyToAll();
      void updatePartDisplay();

public:
      AbilitySimulation(MasterScore* s, QWidget *parent = 0);

      void updatePartsList();
      void updateDisabled();

      void setScore(MasterScore* s)	{ _score = s; }
      };

} // namespace Ms
#endif // ABILITYSIMULATION_H
