//

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4GlobalMagFieldMessenger;
class G4LogicalVolume;


class DetectorConstruction : public G4VUserDetectorConstruction
 {
  public:
     DetectorConstruction();
     virtual ~DetectorConstruction();

  public:
     virtual G4VPhysicalVolume* Construct();
     

     G4double GetFoilArea() const{return fGoldArea;};
     G4double GetFoilMass() const{return fGoldMass;};

     void SetNumberOfShells (G4int Shells) {fShellnumber = Shells;};
     void SetScorer (G4int Scorer) {fScorerNumber = Scorer;};
     void SetFileName (G4String FileName) {fFileName = FileName;};
     G4String GetFileName() const{return fFileName;};

     G4VPhysicalVolume* GetWorldVolume();

  private:
     // methods
     //
     void DefineMaterials();
     G4VPhysicalVolume* DefineVolumes();
  
     // data members
     G4double fGoldArea;
     G4double fGoldMass;

     G4bool  fCheckOverlaps; // option to activate checking of volumes overlaps
    
     G4int fShellnumber;
     G4int fScorerNumber;
     G4String fFileName;
     
     G4VPhysicalVolume* fWorldVolume;
 };

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

