/// \file B2/include/SteppingAction.hh
/// \brief Definition of the B2::SteppingAction class

#ifndef B2SteppingAction_h
#define B2SteppingAction_h 1
#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "CLHEP/Units/SystemOfUnits.h" // 单位头文件CLHEP

class G4LogicalVolume;

namespace B2
{

class EventAction;

/// Stepping action class

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(EventAction* eventAction);
    ~SteppingAction() override = default;

    // method from the base class
    void UserSteppingAction(const G4Step*) override;

  private:
    EventAction* fEventAction = nullptr;
  
    // 固定参数
    const G4double fCollectionEfficiency = 0.9; // 光子收集效率
    const G4double fScintillationYield = 10000.0 / CLHEP::MeV; // 闪烁产额
    const G4double fRefIndex = 1.458; // 切伦科夫效应折射率
    const G4double fBetaThreshold = 1.0 / fRefIndex; // 切伦科夫阈值β
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
