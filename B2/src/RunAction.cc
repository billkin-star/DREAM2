/// \file B2/src/RunAction.cc
/// \brief Implementation of the B2::RunAction class

// RunAction.cc：运行动作（整体数据管理与输出）

#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
// #include "Run.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"

namespace B2
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction() : G4UserRunAction()
{
  // 关键：启用G4AnalysisManager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetDefaultFileType("root");
  analysisManager->SetVerboseLevel(1);

  analysisManager->SetNtupleMerging(true); //否则每个 worker 线程都会各自写一个 ROOT 文件

  // 创建Ntuple（对应你的PhotonTree）
  analysisManager->CreateNtuple("PhotonTree", "闪烁/切伦科夫光子数数据");
  analysisManager->CreateNtupleIColumn("ScintPhoton");  // 对应ScintPhoton/I
  analysisManager->CreateNtupleIColumn("CerenkovPhoton");  // 对应CerenkovPhoton/I
  analysisManager->FinishNtuple();  
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run*)
{
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->OpenFile(); 

  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Reset();


  // inform the runManager to save random number seed
  // G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  // reset accumulables to their initial values
  // G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  // accumulableManager->Reset();
  // fFile = new TFile("PhotonData.root", "RECREATE");
  // fPhotonTree = new TTree("PhotonTree", "闪烁/切伦科夫光子数数据");
  // fPhotonTree->Branch("ScintPhoton", &fScintPhoton, "ScintPhoton/I");
  // fPhotonTree->Branch("CerenkovPhoton", &fCerenkovPhoton, "CerenkovPhoton/I");

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* run)
{
  // 关键：写入并关闭文件（与宏文件/analysis/file/close功能一致）
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();

  // fPhotonTree->Write();
  // fFile->Close();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......




}
