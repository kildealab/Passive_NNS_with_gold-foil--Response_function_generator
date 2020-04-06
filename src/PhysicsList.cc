
/*In this physics list, I am modifying the the QGSP_BIC_HP to include the neutron thermal scatter below 4 eV.
The QGSP_BIC_HP is called in the physics list header file. */

#include "PhysicsList.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include "G4HadronicInteractionRegistry.hh"
#include "G4HadronicProcess.hh"
#include "G4HadronicProcessStore.hh"
#include "G4HadronicProcessType.hh"
#include "G4Neutron.hh"
#include "G4NeutronHPThermalScattering.hh"
#include "G4NeutronHPThermalScatteringData.hh"
#include <assert.h>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void PhysicsList::ConstructProcess() {
  G4VModularPhysicsList::ConstructProcess();

  // Get the elastic scattering process for neutrons
  G4ParticleDefinition* nPD = G4Neutron::Definition();// We want to keep the thermal scattering for other particles untouched and modify only for neutrons

  G4ProcessVector* pvec = nPD->GetProcessManager()->GetProcessList();
  G4HadronicProcess* nElastic = 0;
  for (G4int i=0; i<pvec->size(); i++) {
    if ((*pvec)[i]->GetProcessSubType() != fHadronElastic) continue;
    nElastic = dynamic_cast<G4HadronicProcess*>((*pvec)[i]);
    break;
  }
  assert(nElastic != 0);

  // Get the "regular" HP elastic scattering model, exclude thermal scattering region
  G4HadronicInteraction* nElasticHP = G4HadronicInteractionRegistry::Instance()->FindModel("NeutronHPElastic");
  assert(nElasticHP != 0);
  nElasticHP->SetMinEnergy(4.*eV); //set the min energy for the neutron elastic scattering below which the thermal scattering needed 

  // Attach HP thermal scattering model and data files to process
  nElastic->RegisterMe(new G4NeutronHPThermalScattering); //thermal scattering model
  nElastic->AddDataSet(new G4NeutronHPThermalScatteringData); // thermal scattering cross sections.
}
