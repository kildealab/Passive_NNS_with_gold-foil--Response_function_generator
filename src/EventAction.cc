
/*************************************************************************************
The Event Action class manages each event. The hit map in each event are obtained and 
analysed here. The Primitive scorer class returns the hit map and its analysed for each 
event.
**************************************************************************************/
#include "EventAction.hh"
#include "RunAction.hh"


#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4RunManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4THitsMap.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VSensitiveDetector.hh"
#include "G4VPrimitiveScorer.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String ScorerName = "none";
//G4VPrimitiveScorer* Scorer; 
EventAction::EventAction(RunAction* RuAct)
  : G4UserEventAction(),
    fRunAction(RuAct),
    fnHCID(-1)
  {
   G4RunManager::GetRunManager()->SetPrintProgress(0);
  }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


EventAction::~EventAction()
  { }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void EventAction::BeginOfEventAction(const G4Event*)
  { 
  	G4SDManager* SDman = G4SDManager::GetSDMpointer();
  	G4MultiFunctionalDetector* MFD = (G4MultiFunctionalDetector*)(SDman->FindSensitiveDetector("mfDetector"));
  	G4VPrimitiveScorer* Scorer = MFD->GetPrimitive(0);
  	ScorerName = Scorer->GetName(); // get the name of the scorer we use

    if ( fnHCID==-1 ){
        fnHCID = SDman->GetCollectionID("mfDetector/"+ScorerName);
        }
  }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void EventAction::EndOfEventAction(const G4Event* event)
  {
   //initializing the variables.
   G4double nCcounts  = 0;
   G4double cellFlux = 0.;
   G4double neutronINcount = 0;

   // Get the hits collection of the event
   G4HCofThisEvent* hce = event->GetHCofThisEvent();


   std::map<G4int,G4double*>::iterator itr;
   
   //======================================================================================================
   // Iterating over the nCaptureMap, the hit map from the nCapture scorer.
   // nCapture map has as many entries as the total nCapture interactions. Therefore the number of entries
   // in the nCapture map gives the total number of capture interactions. The each entry in the nCapture map
   // is the atomic mass of the secondary particle created. This enables us to count the capture interactions
   // of individual species seperately.  
   //=======================================================================================================
   if(ScorerName == "nCapture")
   	  {   
   	   G4THitsMap<G4double>* nCaptureMap = (G4THitsMap<G4double>*)(hce->GetHC(fnHCID));
       for (itr = nCaptureMap->GetMap()->begin(); itr != nCaptureMap->GetMap()->end(); itr++) {
            nCcounts += *(itr->second);// This will give the sum of all the weights. ie, the weighted number of nCapture interactions.
            }
       // feed the data obtained from the event to RunAction class.
       fRunAction->WeightedCaptureEvents(nCcounts);
       }

  
   if(ScorerName == "CellFlux")
   	  { 	   
       G4THitsMap<G4double>* FluxMap = (G4THitsMap<G4double>*)(hce->GetHC(fnHCID));
       for (itr = FluxMap->GetMap()->begin(); itr != FluxMap->GetMap()->end(); itr++) {
            cellFlux += *(itr->second);
            }
       // feed the data obtained from the event to RunAction class.           
       fRunAction->TotalCellFlux(cellFlux);
      }  


   //====================================================================================
   // Iterating over the nFluenceMap, the hit map from the nFluence scorer. The mass of the gold foil
   // is registered in the map when a neutron is entered to the foil. So, the total number of entrie in
   // nFluenceMap is the total number of neutron entering the gold foil. 
   //====================================================================================
   if(ScorerName == "nFluence")
   	  { 	   
       G4THitsMap<G4double>* nFluenceMap = (G4THitsMap<G4double>*)(hce->GetHC(fnHCID));
       for (itr = nFluenceMap->GetMap()->begin(); itr != nFluenceMap->GetMap()->end(); itr++) {
            neutronINcount += *(itr->second); //weighted sum of neutrons entering the foil
            }
       // feed the data obtained from the event to RunAction class.           
       fRunAction->TotalNeutronsIN(neutronINcount);
      }

  }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


