
/*******************************************************************************
The nFluence scorer is used to count the number of neutrons that are entering
the gold foil. A hit is registered only if the particle is a parent neutron and
if its entering the goldfoil.
********************************************************************************/

#include "nFluenceScorer.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Neutron.hh"
#include "G4VProcess.hh"

G4int count = 0;
nFluenceScorer::nFluenceScorer(G4String name, G4int depth)
  : G4VPrimitiveScorer(name,depth),
    HCID(-1),
    FluenceMap(0)
   { }

nFluenceScorer::nFluenceScorer(G4String name, const G4String& unit, G4int depth)
  : G4VPrimitiveScorer(name,depth),
    HCID(-1),
    FluenceMap(0)
   { }

nFluenceScorer::~nFluenceScorer()
   { ; }

G4bool nFluenceScorer::ProcessHits(G4Step* aStep,G4TouchableHistory*)
   {
    //=====================================================================================================
    //The preStepPoint->GetStepStatus() checks if the particle is entering the volume of interest ( here the SD).
    // If postStepPoint was used instead, the particle leaving the volume would be analyzed. 
    // For every parent neutron entering the SD, an entry is made in the Fluence map. The value passed in each
    // hit is the weight of the neutron entering the foil.
    //=====================================================================================================
    G4StepPoint* prePoint = aStep->GetPreStepPoint();
    
    if (prePoint->GetStepStatus() != fGeomBoundary)return FALSE; // returns if the particle is not entering the sensitive volume
        
    if(aStep->GetTrack()->GetDefinition()->GetParticleName()  != "neutron") return FALSE; //returns if the parent particle is not neutron. 
    
                  //G4double dm = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetMass();
                  //G4cout<<"THE mass = "<<dm<<G4endl;
                  //G4double energy = prePoint->GetKineticEnergy();
                  
                  //dm *= prePoint->GetWeight();
                  G4double value = aStep->GetPreStepPoint()->GetWeight();
                  count ++; // a count is passed instead of an index because, the index is same for the gold foil and this results in loss of counts when two interactions occur simulatneously.
                  FluenceMap->add(count,value);  // registers the mass of the gold foil.
         
    return TRUE;
   }

void nFluenceScorer::Initialize(G4HCofThisEvent* HCE)
  {
   FluenceMap = new G4THitsMap<G4double>(GetMultiFunctionalDetector()->GetName(), GetName());
   if(HCID < 0) {HCID = GetCollectionID(0);}
   HCE->AddHitsCollection(HCID, (G4VHitsCollection*)FluenceMap);
  }

void nFluenceScorer::EndOfEvent(G4HCofThisEvent*)
  { ; }

void nFluenceScorer::clear()
  {
   FluenceMap->clear();
  }

void nFluenceScorer::DrawAll()
  { ; }

void nFluenceScorer::PrintAll()
  {
   G4cout << " MultiFunctionalDet  " << detector->GetName() << G4endl;
   G4cout << " PrimitiveScorer " << GetName() << G4endl;
   G4cout << " Number of entries " << FluenceMap->entries() << G4endl;
   std::map<G4int,G4double*>::iterator itr = FluenceMap->GetMap()->begin();
   for(; itr != FluenceMap->GetMap()->end(); itr++) {
        G4cout << "  copy no.: " << itr->first
	           << "  Particle weight: " 
	           << *(itr->second)
	           << G4endl;
       }
  }

