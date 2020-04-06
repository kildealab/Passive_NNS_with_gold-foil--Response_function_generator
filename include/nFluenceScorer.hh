#ifndef nFluenceScorer_h
#define nFluenceScorer_h 1

#include "G4VPrimitiveScorer.hh"
#include "G4THitsMap.hh"

////////////////////////////////////////////////////////////////////////////////
// Description:
//   This is a primitive scorer class for scoring the number of parent neutrons
// entering the SD volume.
///////////////////////////////////////////////////////////////////////////////

class nFluenceScorer : public G4VPrimitiveScorer
{
 
 public: // with description
      nFluenceScorer(G4String name, G4int depth=0); // default unit
      nFluenceScorer(G4String name, const G4String& unit, G4int depth=0);
      virtual ~nFluenceScorer();

 protected: // with description
      virtual G4bool ProcessHits(G4Step*,G4TouchableHistory*);

 public: 
      virtual void Initialize(G4HCofThisEvent*);
      virtual void EndOfEvent(G4HCofThisEvent*);
      virtual void clear();
      virtual void DrawAll();
      virtual void PrintAll();

     

  private:
      G4int HCID;
      G4THitsMap<G4double>* FluenceMap;
  
};
#endif
