/// \file B2/src/SteppingAction.cc
/// \brief Implementation of the B2::SteppingAction class

// SteppingAction.cc：步长动作（精准统计核心）
#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "CLHEP/Random/RandPoisson.h" // 泊松统计头文件

#include "CLHEP/Units/SystemOfUnits.h" // 单位头文件CLHEP
namespace B2
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
SteppingAction::SteppingAction(EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void SteppingAction::UserSteppingAction(const G4Step* step)
{
  // 1. 获取得分体积（闪烁光纤+切伦科夫光纤）
  auto detConst = (DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  G4LogicalVolume* scintVol = detConst->GetScoringVolume();
  G4LogicalVolume* cerenkovVol = detConst->GetScoringVolumeCerenkov();

  // 2. 获取当前步长的核心信息
  G4LogicalVolume* currentVol = step->GetPreStepPoint()->GetTouchable()->GetVolume()->GetLogicalVolume();
  G4double edep = step->GetTotalEnergyDeposit(); // 沉积能量（闪烁光子计算用）
  G4Track* track = step->GetTrack(); // 粒子轨迹（切伦科夫光子计算用）
  G4double stepLength = step->GetStepLength(); // 步长长度（切伦科夫光子计算用）

  // ====================== 闪烁光子数计算 ======================
//   if (currentVol == scintVol) {
//     fEventAction->AddEdep(edep); // 无论能量多小都记录

//     if (edep > 0) {
//         G4double meanPhotons = edep * fScintillationYield * fCollectionEfficiency;
//         G4int nPhotons = CLHEP::RandPoisson::shoot(meanPhotons);
//         if (nPhotons > 0) {
//             fEventAction->AddScintPhotons(nPhotons);
//         }
//     }
//  }
  if (currentVol == scintVol && edep > 0) // 仅在闪烁光纤内且有能量沉积时计算
  {
    // 步骤1：计算平均光子数
    G4double meanPhotons = edep * fScintillationYield * fCollectionEfficiency;
    // G4cout << "平均光子数meanPhotons：" << meanPhotons << G4endl; 
    // G4cout << "当前能量沉积edep（默认MeV）：" << edep << G4endl;
    // G4cout << "闪烁产额fScintillationYield：" << fScintillationYield << G4endl;
    // G4cout << "收集效率fCollectionEfficiency：" << fCollectionEfficiency << G4endl;

    // 步骤2：泊松抽样得到实际光子数
    G4int nPhotons = CLHEP::RandPoisson::shoot(meanPhotons);

    // 步骤3：传递给EventAction累加
    if (nPhotons > 0) {
      fEventAction->AddScintPhotons(nPhotons);
    }
  }

  // ====================== 切伦科夫光子数计算 ======================
  if (currentVol == cerenkovVol) // 仅在切伦科夫光纤内计算
  {
    // 步骤1：判断粒子是否为带电粒子（切伦科夫效应仅对带电粒子有效）
    const G4ParticleDefinition* particle = track->GetParticleDefinition();
    if (!particle->GetPDGCharge()) { // 中性粒子：直接返回0
      return;
    }

    // 步骤2：计算粒子的β值（v/c，速度与光速的比值）
    G4ThreeVector momentum = track->GetMomentum(); // 获取粒子动量（矢量，含大小和方向）
    G4double momentumMag = momentum.mag(); // 获取动量的大小
    G4double totalEnergy = track->GetTotalEnergy(); // 获取粒子相对论总能量
    G4double beta = momentumMag / totalEnergy; // 计算β值

    if (beta <= fBetaThreshold) { // 低于阈值：不产生光子
      return;
    }

    // 步骤3：计算单位长度光子数（每厘米）
    G4double dNdL = 369.0 * (1.0 - 1.0/(beta*beta*fRefIndex*fRefIndex));

    // 步骤4：计算平均光子数（考虑步长长度和收集效率）
    G4double meanPhotons = dNdL * (stepLength / CLHEP::cm) * fCollectionEfficiency;
    // G4cout << "平均切伦科夫光子数meanPhotons：" << meanPhotons << G4endl; 

    // 步骤5：泊松抽样得到实际光子数
    G4int nPhotons = CLHEP::RandPoisson::shoot(meanPhotons);

    // 步骤6：传递给EventAction累加
    if (nPhotons > 0) {
      fEventAction->AddCerenkovPhotons(nPhotons);
    }
  }
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}




