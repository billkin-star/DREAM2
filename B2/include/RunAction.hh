/// \file B2/include/RunAction.hh
/// \brief Definition of the B2::RunAction class

#ifndef B2RunAction_h
#define B2RunAction_h 1
#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "globals.hh"
#include "TTree.h"
#include "TFile.h"

class G4Run;

namespace B2
{

/// Run action class
///
/// In EndOfRunAction(), it calculates the dose in the selected volume
/// from the energy deposit accumulated via stepping and event actions.
/// The computed dose is then printed on the screen.

class RunAction : public G4UserRunAction
{
  public:
    RunAction();
    ~RunAction() override = default;

    void BeginOfRunAction(const G4Run*) override;
    void   EndOfRunAction(const G4Run*) override;


    void FillPhotonTree(G4int scint, G4int cerenkov) {
      fScintPhoton = scint;
      fCerenkovPhoton = cerenkov;
      fPhotonTree->Fill();
    }

  private:
    TFile* fFile;
    TTree* fPhotonTree;
    G4int fScintPhoton;
    G4int fCerenkovPhoton;
};

}

#endif

