#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "globals.hh"

class G4Run;


class RunAction : public G4UserRunAction
 {
  public:
    RunAction();
    virtual ~RunAction();

    //virtual G4Run* GenerateRun();
    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);

    void WeightedCaptureEvents(G4double nCCounts) { fNnCcounts += nCCounts; };
    void TotalCellFlux(G4double cellFlux) {fcellFlux += cellFlux;};
    void TotalNeutronsIN(G4double nNeutrons)    { fNneutrons += nNeutrons; };

  private:
    //PrimaryGeneratorAction* fPrimary;

    G4Accumulable<G4double>  fNnCcounts;
    G4Accumulable<G4double> fcellFlux;
    G4Accumulable<G4double> fNneutrons;

    G4double fgoldArea;
    G4double fgoldMass;
 };

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

