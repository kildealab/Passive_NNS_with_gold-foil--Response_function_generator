#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4GeneralParticleSource;
class G4Event;


class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
 {
  public:
     PrimaryGeneratorAction();    
     virtual ~PrimaryGeneratorAction();

     virtual void GeneratePrimaries(G4Event* event);
  
     // set methods
     void SetRandomFlag(G4bool value);
 
  private:
     G4GeneralParticleSource*  fParticleGun; // G4 particle source
     
    
 };

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
