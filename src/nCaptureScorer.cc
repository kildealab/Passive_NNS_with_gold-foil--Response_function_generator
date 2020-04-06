
/***********************************************************************************
nCapture scorer can count the total number of neutron capture interactions in the 
sensitive volume and also can give the species wise netron capture interactions.
************************************************************************************/

#include "nCaptureScorer.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Neutron.hh"
#include "G4VProcess.hh"


nCaptureScorer::nCaptureScorer(G4String name, G4int depth)
  : G4VPrimitiveScorer(name,depth),
    HCID(-1),
    CaptureMap(0)
   { }

nCaptureScorer::nCaptureScorer(G4String name, const G4String& unit,G4int depth)
  : G4VPrimitiveScorer(name,depth),
    HCID(-1),
    CaptureMap(0)
   { }

nCaptureScorer::~nCaptureScorer()
   { ; }

G4bool nCaptureScorer::ProcessHits(G4Step* aStep,G4TouchableHistory*)
   {
    //=============================================================================================
    // For each step, if the interaction is an nCapture reaction, the secondary particles that
    //  are getting created in each interactions is analyzed. In Geant4, the new isotopes formed 
    // as a result of an interaction is also treated as a secondary particle. Therefore, the atomic
    // mass checked to see if the neutron capture interaction happened to the gold atom. The weight 
    // of each particle satisfying these conditions are passed to the Capture map and they are summed
    // over in the event action to get the weighted sum of nCapture interaction by gold atoms.
    //==============================================================================================
    G4StepPoint* posPoint = aStep->GetPostStepPoint();
    if (aStep->GetTrack()->GetDefinition()!= G4Neutron::Definition() ) return FALSE; // returns if the particle is no a neutron
    
        G4String procName = posPoint->GetProcessDefinedStep()->GetProcessName();
        // G4cout<<"The name of the process is  "<<procName<<G4endl;
        if (procName != "nCapture") return FALSE; //returns if the interaction is not neutron capture
        //G4cout<<"Its is a capture"<<G4endl;
           for (auto iter : *aStep->GetSecondaryInCurrentStep()) //This will give only the secondaries that are created in the current step. (Not all secondaries present along the current track)
               {
                const auto def = iter->GetDefinition();
                //const auto name = def->GetParticleName();
                G4double AtomicMass = def->GetAtomicMass(); // returns the atomic mass of the secondary particle created.
                //G4double ene = iter->GetTotalEnergy();
        
                //G4cout<<name<<G4endl;
                
                if (AtomicMass == 198.)
                      {
                       G4int  index = GetIndex(aStep);
                       G4double value = aStep->GetPreStepPoint()->GetWeight();
                       CaptureMap->add(index,value);  // registers the weight of the neutron in each nCapture interaction
                      }
                 
               }  
 
    return TRUE;
   }

void nCaptureScorer::Initialize(G4HCofThisEvent* HCE)
  {
   CaptureMap = new G4THitsMap<G4double>(GetMultiFunctionalDetector()->GetName(),GetName());
   if(HCID < 0) {HCID = GetCollectionID(0);}
   HCE->AddHitsCollection(HCID, (G4VHitsCollection*)CaptureMap);
  }

void nCaptureScorer::EndOfEvent(G4HCofThisEvent*)
  { ; }

void nCaptureScorer::clear()
  {
   CaptureMap->clear();
  }

void nCaptureScorer::DrawAll()
  { ; }

void nCaptureScorer::PrintAll()
  {
   G4cout << " MultiFunctionalDet  " << detector->GetName() << G4endl;
   G4cout << " PrimitiveScorer " << GetName() << G4endl;
   G4cout << " Number of entries " << CaptureMap->entries() << G4endl;
   std::map<G4int,G4double*>::iterator itr = CaptureMap->GetMap()->begin();
   for(; itr != CaptureMap->GetMap()->end(); itr++) {
      G4cout << "  copy no.: " << itr->first
             << "  Particle weight: " 
             << *(itr->second)
             << G4endl;
      }
  }

