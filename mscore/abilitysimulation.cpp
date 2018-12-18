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
      connect(partsList,            SIGNAL(itemSelectionChanged()),     SLOT(updatePartDisplay()));
      connect(useAbilitySimulation, SIGNAL(toggled(bool)),              SLOT(useAbilityChanged(bool)));
      connect(abilityLevel,         SIGNAL(currentIndexChanged(int)),   SLOT(updateAbilityLevel(int)));
      connect(applyToAll,           SIGNAL(clicked()),                    SLOT(doApplyToAll()));

      updatePartsList();
      updateDisabled();

      MuseScore::restoreGeometry(this);
      }

//---------------------------------------------------------
//   updatePartsList
//---------------------------------------------------------

void AbilitySimulation::updatePartsList()
      {
      partsList->clear();
      for (Part* p : _score->parts()) {
            partsList->addItem(p->partName());
            }
      }

//---------------------------------------------------------
//   useAbilityChanged
//---------------------------------------------------------

void AbilitySimulation::useAbilityChanged(bool checked)
      {
      _score->setUseAbilitySimulation(checked);
      updateDisabled();
      }

//---------------------------------------------------------
//   updateAbilityLevel
//---------------------------------------------------------

void AbilitySimulation::updateAbilityLevel(int ind)
      {
      // If everything is configured correctly, this will work
      AbilityLevel al = (AbilityLevel)ind;

      int currentPartInd = partsList->row(partsList->currentItem());
      Part* p = _score->parts()[currentPartInd];

      p->setAbilityLevel(al);
      updateDisabled();
      }

//---------------------------------------------------------
//   doApplyToAll
//---------------------------------------------------------

void AbilitySimulation::doApplyToAll()
      {
      int ind = abilityLevel->currentIndex();
      AbilityLevel al = (AbilityLevel)ind;
      for (Part* p : _score->parts()) {
            p->setAbilityLevel(al);
            }
      }

//---------------------------------------------------------
//   updatePartDisplay
//---------------------------------------------------------

void AbilitySimulation::updatePartDisplay()
      {
      int currentPartInd = partsList->row(partsList->currentItem());
      Part* p = _score->parts()[currentPartInd];

      int abilityInd = (int)p->abilityLevel();

      abilityLevel->setCurrentIndex(abilityInd);
      updateDisabled();
      }

//---------------------------------------------------------
//   updateDisabled
//---------------------------------------------------------

void AbilitySimulation::updateDisabled()
      {
      if (_score->useAbilitySimulation()) {
            useAbilitySimulation->setChecked(true);
            qDebug("using ability sim");
            }
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

//---------------------------------------------------------
//   showAbilitySimulation
//---------------------------------------------------------

void MuseScore::showAbilitySimulation(bool value)
      {
      if (abilitySimulation == 0) {
            abilitySimulation = new AbilitySimulation(currentScore()->masterScore(), 0);
            }

      if (value) {
            abilitySimulation->setScore(currentScore()->masterScore());
            abilitySimulation->updateDisabled();
            abilitySimulation->updatePartsList();
            abilitySimulation->show();
            abilitySimulation->raise();
            }
      else {
            abilitySimulation->hide();
            }
      }

}  // namespace Ms
