#include "abilitysimulation.h"
#include "ui_abilitysimulation.h"

#include "musescore.h"
#include "scoreview.h"
#include "libmscore/score.h"
#include "libmscore/part.h"

namespace Ms {

AbilitySimulation::AbilitySimulation(MasterScore* s, QWidget *parent) : QDialog(parent)
      {
      setObjectName("AbilitySimulation");
      setupUi(this);
      _score = s;

      //connect(this, SIGNAL(accepted()), SLOT(saveValues()));
      connect(partsList, SIGNAL(itemSelectionChanged()), SLOT(updateDisabled()));
      connect(useAbilitySimulation, SIGNAL(toggled(bool)), SLOT(useAbilityChanged(bool)));

      updateParts();
      updateDisabled();

      MuseScore::restoreGeometry(this);
      }

void AbilitySimulation::updateParts()
      {
      partsList->clear();
      for (Part* p : _score->parts()) {
            partsList->addItem(p->partName());
            }
      }

void AbilitySimulation::useAbilityChanged(bool checked)
      {
      qDebug() << "chk is " << checked;
      _score->setUseAbilitySimulation(checked);
      updateDisabled();
      }

void AbilitySimulation::updateDisabled()
      {
      if (_score->useAbilitySimulation())
            useAbilitySimulation->setChecked(true);
      else
            useAbilitySimulation->setChecked(false);

      if (!useAbilitySimulation->isChecked() || partsList->selectedItems().count() != 1) {
            abilityLevelLabel->setDisabled(true);
            abilityLevel->setDisabled(true);
            applyToAll->setDisabled(true);
            highlightDifficult->setDisabled(true);
            }
      else {
            abilityLevelLabel->setDisabled(false);
            abilityLevel->setDisabled(false);
            applyToAll->setDisabled(false);
            highlightDifficult->setDisabled(false);
            }
      }

//---------------------------------------------------------
//   hideEvent
//---------------------------------------------------------

void AbilitySimulation::hideEvent(QHideEvent* event)
      {
      QAction* a = getAction("toggle-ability-sim");
      a->setChecked(false);   // hack
      MuseScore::saveGeometry(this);
      QDialog::hideEvent(event);
      }

void MuseScore::showAbilitySimulation(bool value)
      {
      if (abilitySimulation == 0) {
            abilitySimulation = new AbilitySimulation(currentScore()->masterScore(), 0);
            }

      if (value) {
            abilitySimulation->updateDisabled();
            abilitySimulation->updateParts();
            abilitySimulation->show();
            abilitySimulation->raise();
            }
      else {
            abilitySimulation->hide();
            }
      }

}  // namespace Ms
