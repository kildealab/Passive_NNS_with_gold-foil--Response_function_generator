
/*******************************************************************************************************
RunAction class is used to control and modify informations after each run. Run wise information is collected
in this class. Usually with the AnalysisManager class, the informations can be stored into a tuple or a
histogram and can be output in different file formats.
The G4AccumulableManager class is used to sum up the data in differnt threads in multithreaded mode
to get a cumilative score at the end of each run.
********************************************************************************************************/
#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4AccumulableManager.hh"
#include "G4GeneralParticleSource.hh"
// For randomizing the seed
#include "Randomize.hh"
#include <time.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int runID = 0; // Initializing a global variable to register RunID.

RunAction::RunAction()
  : G4UserRunAction(),
    fNnCcounts(0),
    fcellFlux(0),
    fNneutrons(0),
    fgoldMass(0),
    fgoldArea(0)
   {
    //The following is implemented to make sure that the particles are generated randomly. Without it, the result will be
    // the same between multiple runs with the same settings.
    G4Random::setTheEngine;       //selection of a random engine
    G4long seed=time(0);          //returns time in seconds as an integer
    G4Random::setTheSeed(seed);   //changes the seed of the random engine
    G4Random::showEngineStatus(); //shows the actual seed


    //Registering the scores into the accumulable manager
    G4AccumulableManager* accMan = G4AccumulableManager::Instance();
    accMan->RegisterAccumulable(fNnCcounts);
    accMan->RegisterAccumulable(fcellFlux);
    accMan->RegisterAccumulable(fNneutrons);
   }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



RunAction::~RunAction()
   {
    // Have to manually delete the created analysis manager
   // delete G4AnalysisManager::Instance();
   }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



void RunAction::BeginOfRunAction(const G4Run* run)
   {
    runID = run->GetRunID();
    G4cout << "### Run " << runID << " start." << G4endl;

    G4AccumulableManager* accMan = G4AccumulableManager::Instance();
    accMan->Reset(); // Reset the accumulable manager in the begining of each run
   }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* run)
   {
    G4int nbEvents = run->GetNumberOfEvent();
    if (nbEvents == 0) return;

    G4AccumulableManager* accMan = G4AccumulableManager::Instance();
    accMan->Merge(); // combines the data in differnt threads.

    if(isMaster){
                 //================================================================================================
                 // Information from the dectector contruction class
                 //================================================================================================

                 const DetectorConstruction* detectorConstruction
                           = static_cast< const DetectorConstruction*>
                             (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
                 fgoldArea = detectorConstruction->GetFoilArea() ; //This is the total surface area of the gold foil
                 fgoldArea = fgoldArea / cm2 ; // converting the area into the cm2 unit.

                 fgoldMass = detectorConstruction->GetFoilMass() ; //This is the total mass of the gold foil
                 fgoldMass = fgoldMass / g ; // converting the mass to grams.

                 //=================================================================================================
                 // Data from the accumulable manager
                 //=================================================================================================

                 G4double UnitCapture      = (fNnCcounts.GetValue()/fgoldMass ); //Neutron Capture per unit foil mass.
                 G4double TotalFluence     = (fNneutrons.GetValue()/fgoldArea ); //Total neutron fluence of Goldfoil. Neutrons entering per unit area of the foil


                 //=======================================================================================================
                 // Storing the data into a CSV file.
                 // Data appending is not possible with the analysis manager. It can only overwrite.
                 // Ofstream method is used to append the data into a file after each run for 52 different neutron energies
                 //=======================================================================================================
                 std::ofstream myfile;
                 G4String filename = detectorConstruction->GetFileName(); // file name will be different according to the number of shells used for the simulation.
                 myfile.open(filename,std::ofstream::app);

                 if (filename.find("Captures") == 0)
                        {
                         G4cout << "Total number of nCapture interactions: " << fNnCcounts.GetValue() << G4endl; //terminal output

                         // For the first run, the entry names are written to the CSV file
                         if (runID == 0){
                                         myfile<<"\"Total nCaptures\""
                                               <<","<<"\"Capture per unit mass [n/g]\""<<"\n";
                                        }

                         // entering the data to the CSV file
                         myfile<<std::setw(7)  <<fNnCcounts.GetValue() //total number of capture reactions.
                                               <<","<<UnitCapture <<"\n";  // Capture per unit area. = Total capture/total foil area.
                        }




                 //Write the following if cell flux scorer is used.
                 if (filename.find("CellFlux") == 0)
                        {
                         G4cout  << "Total CellFlux: "<< fcellFlux.GetValue() << G4endl; //terminal output

                         // For the first run, the entry names are written to the CSV file
                         if (runID == 0){
                                         myfile<<"\"CellFlux\""
                                               <<"\n";
                                         }

                         // entering the data to the CSV file
                         myfile<<std::setw(7)  <<fcellFlux.GetValue() // total cell flux.
                                                <<"\n";
                        }


                 //Write the following if fluence scorer is used.
                 if (filename.find("Fluence") == 0)
                        {
                         G4cout  << "Total number of neutrons entering the Goldfoil: "<< fNneutrons.GetValue() << G4endl; //terminal output

                         // For the first run, the entry names are written to the CSV file
                         if (runID == 0){
                                         myfile<<"\"Neutrons entering the foil\""
                                               <<","<<"\"Neutron Fluence [n/cm2]\""<<"\n";
                                         }

                         // entering the data to the CSV file
                         myfile<<std::setw(7)  <<fNneutrons.GetValue() // total number of neutron entering the gold foil.
                                               <<","<<TotalFluence <<"\n";  // Total neutron Fluence.
                        }


                }


   }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
