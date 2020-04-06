

#ifndef PVolumeStore_hh
#define PVolumeStore_hh PVolumeStore_hh

#include "globals.hh"
#include <set>
#include "G4GeometryCell.hh"
#include "G4GeometryCellComp.hh"

typedef std::set< G4GeometryCell, G4GeometryCellComp > SetGeometryCell;

class PVolumeStore {
public:
  PVolumeStore();
  ~PVolumeStore();
  
  void AddPVolume(const G4GeometryCell &cell);
  const G4VPhysicalVolume *GetPVolume(const G4String &name) const;


private:
  SetGeometryCell fSetGeometryCell;
};

#endif
