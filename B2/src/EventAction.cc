/// \file B2/src/EventAction.cc
/// \brief Implementation of the B2::EventAction class

// EventAction.cc：事例动作（单个事例的信号累加）
#include "EventAction.hh"
#include "RunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"

namespace B2
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


EventAction::EventAction(RunAction* runAction)  // ← 构造函数定义开始
: // 成员初始化列表开始
  G4UserEventAction(),              // 调用基类构造函数
  fRunAction(runAction)           // 初始化成员指针
{ // 构造函数体开始
    // 这里可以放复杂的初始化代码
    // 但当前示例中是空的
} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// 事例开始时：将光子数总数置0（避免跨事例数据污染）
void EventAction::BeginOfEventAction(const G4Event*)
{
  fScintPhotonTotal = 0;
  fCerenkovPhotonTotal = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// 事例结束时：可在此将光子数传递给RunAction（如写入ROOT文件）
void EventAction::EndOfEventAction(const G4Event*)
{
  auto man = G4AnalysisManager::Instance();
  // G4cout << "闪烁总数：" << fScintPhotonTotal << G4endl;
  // G4cout << "切伦科夫总数：" << fCerenkovPhotonTotal << G4endl;
  man->FillNtupleIColumn(0, fScintPhotonTotal);
  man->FillNtupleIColumn(1, fCerenkovPhotonTotal);
  man->AddNtupleRow();

}
    
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
