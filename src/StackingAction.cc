#include "StackingAction.hh"
#include "G4StackManager.hh"
#include "G4DNAChemistryManager.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4ParticleDefinition.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StackingAction::StackingAction()
    : G4UserStackingAction()
     { }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

StackingAction::~StackingAction()
    { }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* aTrack)
    {
     G4ClassificationOfNewTrack classification = fWaiting;

     const G4ParticleDefinition* particleType = aTrack->GetParticleDefinition();
     G4String particleName = particleType->GetParticleName();
     G4double energy = aTrack->GetKineticEnergy();
     //G4double charge = particleType->GetPDGCharge();
     //G4double mass = particleType->GetAtomicMass();
     
     //Check if the secondary particle is of interest, else kill the particle.
     if ( particleName == "neutron" || particleName == "Au198" || (particleName == "gamma" &&  energy > 9.9*MeV) ) {
        //G4cout << "A(n) " << particleName << " with kinetic energy " << G4BestUnit(energy, "Energy") << " was not killed" << G4endl;
        return classification;
        }

     
     classification = fKill;

     return classification;

     }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void StackingAction::NewStage()
    { }
