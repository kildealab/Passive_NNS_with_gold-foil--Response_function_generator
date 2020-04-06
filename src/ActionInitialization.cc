/* **********************************************************************************
ActionInitialization class 'activates' the classes that need to be called before
a Run is initiated. It's essential to set the user action classes seperately
for the master as in the BuildForMaster function. This enables the "ifMaster" analysis
performed in the RunAction class.
*************************************************************************************/


#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "StackingAction.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ActionInitialization::ActionInitialization()
 : G4VUserActionInitialization()
 {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ActionInitialization::~ActionInitialization()
 {;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::BuildForMaster() const
 {
  RunAction*               RuAct = new RunAction;
  
  SetUserAction(RuAct); //set RunAction class for Master thread.
 }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::Build() const
 {
  PrimaryGeneratorAction*  prim    = new PrimaryGeneratorAction;
  RunAction*               RuAct   = new RunAction;
  EventAction*             EvAct   = new EventAction(RuAct);
  StackingAction*          StacAct = new StackingAction;

  SetUserAction(prim);    // set PrimaryGerneratorAction class.
  SetUserAction(RuAct);   // set RunAction class.
  SetUserAction(EvAct);   // Set EventAction class. 
  SetUserAction(StacAct); // Set StackingAction class.
 }  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
