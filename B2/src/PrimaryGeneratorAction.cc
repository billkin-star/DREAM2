/// \file B2/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the B2::PrimaryGeneratorAction class

// PrimaryGeneratorAction.cc：初级粒子产生器（粒子源定义）
#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Event.hh"

namespace B2
{

class PrimaryGeneratorAction;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  // 1. 实例化G4ParticleGun（参数：每个事例发射的粒子数，此处为1个π介子）
  fParticleGun = new G4ParticleGun(1);

  // 2. 配置π介子（核心：选择粒子类型，按需切换pi+ / pi-）
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName = "pi-"; // 正π介子；负π介子改为"pi-"
  G4ParticleDefinition* particle = particleTable->FindParticle(particleName);
  if (!particle) { // 容错：若粒子名称错误，给出提示
    G4cerr << "错误：无法找到粒子 " << particleName << G4endl;
    return;
  }
   fParticleGun->SetParticleDefinition(particle);

  // 3. 配置入射方向（沿z轴正方向垂直入射）
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1));

  // 4. 配置入射位置（探测器前方2m处，避免初始位置在探测器内）
  fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, -2.0*m));

  // 5. 配置默认能量（可被宏文件覆盖，对应你的8个能量点需求）
  fParticleGun->SetParticleEnergy(20*GeV);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// 析构函数：必须释放fParticleGun的内存，避免内存泄漏
PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //this function is called at the begining of ecah event

  // 发射粒子（完成单个事例的初级粒子生成） 
  fParticleGun->GeneratePrimaryVertex(anEvent); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}


