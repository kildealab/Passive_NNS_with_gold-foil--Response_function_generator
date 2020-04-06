
#include "nCellFluxScorer.hh"

#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VPVParameterisation.hh"
#include "G4UnitsTable.hh"
#include "G4Neutron.hh"

nCellFluxScorer::nCellFluxScorer(G4String name, G4int depth)
    :G4VPrimitiveScorer(name,depth),
    HCID(-1),
    EvtMap(0)
    { }

nCellFluxScorer::nCellFluxScorer(G4String name, const G4String& unit, G4int depth)
    :G4VPrimitiveScorer(name,depth),
    HCID(-1),
    EvtMap(0)
   { }

nCellFluxScorer::~nCellFluxScorer()
   {;}

G4bool nCellFluxScorer::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{ 
  if (aStep->GetTrack()->GetDefinition()!= G4Neutron::Definition() ) return FALSE; // returns if the particle is no a neutron
  
  G4double stepLength = aStep->GetStepLength();
  if ( stepLength == 0. ) return FALSE;

  G4int idx = ((G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable()))->GetReplicaNumber(indexDepth);
  
  G4double cubicVolume = ComputeVolume(aStep, idx)*0.001; //multiplied with 0.001 to make the unit cm3. The value is return as mm3 by default. 

  G4double CellFlux = stepLength / cubicVolume;
  CellFlux *= aStep->GetPreStepPoint()->GetWeight(); 
  G4int index = GetIndex(aStep);
  EvtMap->add(index,CellFlux);

  return TRUE;
}

void nCellFluxScorer::Initialize(G4HCofThisEvent* HCE)
{
  EvtMap = new G4THitsMap<G4double>(detector->GetName(),GetName());
  if ( HCID < 0 ) HCID = GetCollectionID(0);
  HCE->AddHitsCollection(HCID,EvtMap);
}

void nCellFluxScorer::EndOfEvent(G4HCofThisEvent*)
{;}

void nCellFluxScorer::clear(){
  EvtMap->clear();
}

void nCellFluxScorer::DrawAll()
{;}

void nCellFluxScorer::PrintAll()
{
  G4cout << " MultiFunctionalDet  " << detector->GetName() << G4endl;
  G4cout << " PrimitiveScorer " << GetName() <<G4endl; 
  G4cout << " Number of entries " << EvtMap->entries() << G4endl;
  std::map<G4int,G4double*>::iterator itr = EvtMap->GetMap()->begin();
  for(; itr != EvtMap->GetMap()->end(); itr++) {
    G4cout << "  copy no.: " << itr->first
	   << "  cell flux : " << *(itr->second)/GetUnitValue() 
	   << " [" << GetUnit() << "]"
	   << G4endl;
  }
}


G4double nCellFluxScorer::ComputeVolume(G4Step* aStep, G4int idx){

  G4VPhysicalVolume* physVol = aStep->GetPreStepPoint()->GetPhysicalVolume();
  G4VPVParameterisation* physParam = physVol->GetParameterisation();
  G4VSolid* solid = 0;
  if(physParam)
  { // for parameterized volume
    if(idx<0)
    {
      G4ExceptionDescription ED;
      ED << "Incorrect replica number --- GetReplicaNumber : " << idx << G4endl;
      G4Exception("G4PSCellFlux::ComputeVolume","DetPS0001",JustWarning,ED);
    }
    solid = physParam->ComputeSolid(idx, physVol);
    solid->ComputeDimensions(physParam,idx,physVol);
  }
  else
  { // for ordinary volume
    solid = physVol->GetLogicalVolume()->GetSolid();
  }
  
  return solid->GetCubicVolume();
}
