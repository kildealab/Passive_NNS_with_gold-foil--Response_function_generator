//

//User defined classes
#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "PhysicsList.hh"
#include "ImportanceDetectorConstruction.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "G4UIcommand.hh"
#include <string>
#include "G4ScoringManager.hh"
#include "Randomize.hh"

#include "G4VisExecutive.hh"

#include "G4UIExecutive.hh"


//classes for importance biasing
#include "G4GeometrySampler.hh"
#include "G4IStore.hh"
#include "G4ImportanceBiasing.hh"
#include "G4ParallelWorldPhysics.hh"
#include "G4GeometryManager.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " NNSResponse [-m macro ] [-u UIsession] [-t nThreads] [-shell nShells] [-scorer 1]" << G4endl;
    G4cerr << " -scorer 1 sets the cellflux scorer. By default (-scorer 0) it is nCapture scorer" <<G4endl
           <<"   note: -t option is available only for multi-threaded mode."
           << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


int main(int argc,char** argv)
{  
  // Evaluate arguments
  //
  if ( argc > 15 ) {
    PrintUsage();
    return 1;
  }
  if (argc < 3){
    G4cerr <<" Include the number of shells of NNS (0 is bare gold), and the macro file as in the following usage"<<G4endl;
    PrintUsage();
    return 1; }


  G4String macro;
  G4String session;
  G4int shells = 0;
  G4int scorer = 0;
  G4String filename;


#ifdef G4MULTITHREADED
  G4int nThreads = 2;
#endif
  for ( G4int i=1; i<argc; i=i+2 ) {
    if      ( G4String(argv[i]) == "-m" ) macro = argv[i+1];
    else if ( G4String(argv[i]) == "-u" ) session = argv[i+1];
    else if ( G4String(argv[i]) == "-shell") {
            shells = G4UIcommand::ConvertToInt(argv[i+1]);
            }
    else if ( G4String(argv[i]) == "-scorer" ){
            scorer = G4UIcommand::ConvertToInt(argv[i+1]);
            }
    else if ( G4String(argv[i]) == "-t" ) {
      nThreads = G4UIcommand::ConvertToInt(argv[i+1]);
    }
//Setting the filename according to the scorer used.
G4String midCap = G4UIcommand::ConvertToString(shells);
G4String midFlu = macro.substr(5,1); //takes the 6th character from the name of macro file. (shell#.mac)
if(scorer == 0){
  filename = "Captures with "+midCap+" shells.txt";
  } 
else if(scorer == 1){
  filename = "CellFlux with "+midFlu+" shells.txt";
  }
else if(scorer == 2){
  filename = "Fluence for "+midFlu+" shell config.txt";
  }
else {
      PrintUsage();
      return 1;
    }
  }  
  G4ScoringManager * scManager = G4ScoringManager::GetScoringManager();
      scManager->SetVerboseLevel(1); 

  G4UIExecutive* ui = nullptr;
  if ( ! macro.size() ) {
    ui = new G4UIExecutive(argc, argv, session);
  }

  // Choose the Random engine
  //
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  
  // Construct the MT run manager
  //
#ifdef G4MULTITHREADED
  auto runManager = new G4MTRunManager;
  if ( nThreads > 0 ) { G4cerr <<" The number of threads passed ="<<nThreads <<G4endl;
    runManager->SetNumberOfThreads(nThreads);
   }  
#else
  auto runManager = new G4RunManager;
#endif

  DetectorConstruction* detector = new DetectorConstruction();
  runManager->SetUserInitialization(detector);
  detector->SetNumberOfShells(shells);
  detector->SetScorer(scorer);
  detector->SetFileName(filename);

  G4String parallelName("ParallelBiasingWorld");
  ImportanceDetectorConstruction* pdet = new ImportanceDetectorConstruction(parallelName);
  detector->RegisterParallelWorld(pdet);
  pdet->SetNumberOfShells(shells);
  pdet->SetScorer(scorer);

  G4GeometrySampler pgs(pdet->GetWorldVolume(),"neutron");

  pgs.SetParallel(true);

  PhysicsList* physicsList = new PhysicsList(); // QGSP_BIC_HP //FTFP_BERT_HP

  physicsList->RegisterPhysics(new G4ImportanceBiasing(&pgs,parallelName));
  physicsList->RegisterPhysics(new G4ParallelWorldPhysics(parallelName));

  runManager->SetUserInitialization(physicsList); // Set mandatory initialization classes
 

  auto actionInitialization = new ActionInitialization();
  runManager->SetUserInitialization(actionInitialization);

  runManager->Initialize();

  pdet->CreateImportanceStore();

  
  // Initialize visualization
  auto visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if ( macro.size() ) {
    // batch mode
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+macro);
  }
  else  {  
    // interactive mode : define UI session
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    if (ui->IsGUI()) {
      UImanager->ApplyCommand("/control/execute gui.mac");
    }
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted 
  // in the main() program !
  
  G4GeometryManager::GetInstance()->OpenGeometry();

  delete visManager;
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
