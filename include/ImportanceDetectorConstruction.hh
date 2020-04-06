

#ifndef ImportanceDetectorConstruction_hh 
#define ImportanceDetectorConstruction_hh  ImportanceDetectorConstruction_hh 

#include "globals.hh"
#include <map>
#include <vector>
#include "G4GeometryCell.hh"
#include "PVolumeStore.hh"

#include "G4VUserParallelWorld.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4VIStore;
class G4VSolid;


class ImportanceDetectorConstruction : public G4VUserParallelWorld
{
public:
  ImportanceDetectorConstruction(G4String worldName);
  virtual ~ImportanceDetectorConstruction();


  G4VPhysicalVolume &GetWorldVolumeAddress() const;

  G4String GetCellName(G4int i);
  G4GeometryCell GetGeometryCell(G4int i);

  G4VPhysicalVolume* GetWorldVolume();
  
  void SetNumberOfShells (G4int Shells) {fShellnumber = Shells;};
  void SetScorer (G4int Scorer) {fScorerNumber = Scorer;};

  virtual void Construct();
  virtual void ConstructSD();

  G4VIStore* CreateImportanceStore();
    // create an importance store, caller is responsible for deleting it


    

private:
  PVolumeStore fPVolumeStore;

  G4int fShellnumber;
  G4int fScorerNumber;
  //  std::vector< G4VPhysicalVolume * > fPhysicalVolumeVector;
  std::vector< G4LogicalVolume * > fLogicalVolumeVector;
  std::vector< G4VSolid * > fTubVector;
  std::vector<G4double> fNum;
  //  G4VPhysicalVolume *fWorldVolume;

  G4VPhysicalVolume* fGhostWorld;

};

#endif
