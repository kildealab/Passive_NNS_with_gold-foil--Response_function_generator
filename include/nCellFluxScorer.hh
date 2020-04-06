
#ifndef nCellFluxScorer_h
#define nCellFluxScorer_h 1

#include "G4VPrimitiveScorer.hh"
#include "G4THitsMap.hh"

class G4VSolid;




class nCellFluxScorer : public G4VPrimitiveScorer
{
   public: // with description
      nCellFluxScorer(G4String name, G4int depth=0);
      nCellFluxScorer(G4String name, const G4String& unit, G4int depth=0);
      virtual ~nCellFluxScorer();



  protected: // with description
      virtual G4bool ProcessHits(G4Step*,G4TouchableHistory*);

      virtual G4double ComputeVolume(G4Step*, G4int idx);

  public: 
      virtual void Initialize(G4HCofThisEvent*);
      virtual void EndOfEvent(G4HCofThisEvent*);
      virtual void clear();
      virtual void DrawAll();
      virtual void PrintAll();



  private:
      G4int HCID;
      G4THitsMap<G4double>* EvtMap;


};
#endif

