/// \file B2/include/EventAction.hh
/// \brief Definition of the B2::EventAction class

#ifndef B2EventAction_h
#define B2EventAction_h 1
#include "G4UserEventAction.hh"
#include "globals.hh"

namespace B2
{

class RunAction;

/// Event action class

class EventAction : public G4UserEventAction
{
  public:
    EventAction(RunAction* runAction); //构造函数（接收RunAction实例）
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;  // 事例开始时初始化数据
    void EndOfEventAction(const G4Event* event) override;  // 事例结束时传递数据

    // 闪烁光子数累加接口（供SteppingAction调用）
    void AddScintPhotons(G4int nPhoton) { fScintPhotonTotal += nPhoton; }
    // 切伦科夫光子数累加接口（供SteppingAction调用）
    void AddCerenkovPhotons(G4int nPhoton) { fCerenkovPhotonTotal += nPhoton; }

    // 获取累加后的总光子数（供RunAction调用）
    G4int GetScintPhotonTotal() const { return fScintPhotonTotal; }
    G4int GetCerenkovPhotonTotal() const { return fCerenkovPhotonTotal; }

  private:
    RunAction* fRunAction = nullptr;  // 指向RunAction，用于传递数据
    G4int fScintPhotonTotal = 0;    // 单个事例闪烁光子总数
    G4int fCerenkovPhotonTotal = 0; // 单个事例切伦科夫光子总数
    const G4double fCollectionEfficiency = 0.9;  // 固定参数（收集效率，也可作为全局参数定义）
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


