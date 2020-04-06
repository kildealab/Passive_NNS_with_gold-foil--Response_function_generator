
/******************************************************************************************
Primary generator action class is used to set the initial particle characteristics.
The information regarding the particle name, initial momentum, particle direction etc.
need to be set in this class.
Here, instead of G4ParticleGun class the G4GeneralParticleSource is used. Its more extensive
with many features that can be tweaked. But the parctle information can only be set in
the macro file, not in the PrimaryGeneratorAction class.  
*******************************************************************************************/
#include "PrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction(),
   fParticleGun(nullptr)
   
 {
  fParticleGun = new G4GeneralParticleSource();
 }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
 {
  delete fParticleGun;
 }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
 {
  fParticleGun->GeneratePrimaryVertex(anEvent);
 }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


