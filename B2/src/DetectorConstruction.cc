/// \file B2/src/DetectorConstruction.cc
/// \brief Implementation of the B2::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4SystemOfUnits.hh" 
#include "G4SubtractionSolid.hh" // 用于布尔减运算

namespace B2
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// 定义尺寸参数
const G4double CuRod_length = 2.0 * m;
const G4double CuRod_x = 4.0 * mm;
const G4double CuRod_y = 4.0 * mm;
const G4double CuRod_holeR = 2.5 * mm / 2; // 中心孔径半径
const G4double Fiber_d = 0.8 * mm; // 光纤直径
const G4int nScintFiber = 3; // 闪烁光纤数
const G4int nCerenkovFiber = 4; // 切伦科夫光纤数

// 阵列参数（避免几何重叠，预留合理间距）
const G4int RodPerTower = 16;        // 每个tower的铜棒数（16×16）
const G4int TowerTotal = 16;         // 总tower数，4×4
const G4double CuRod_spacing = 4.0025 * mm;  // 铜棒间距
const G4double Tower_spacing = 64.04 * mm; // tower间距
// const G4double CuRod_spacing = 4.1 * mm;  // 铜棒间距
// const G4double Tower_spacing = 66* mm; // tower间距

// 封装：创建单根铜棒逻辑体（铜棒+光纤逻辑）
G4LogicalVolume* DetectorConstruction::CreateSingleCuRodLogical(G4NistManager *nist)
{
  // 1. 获取材料
  G4Material* Air = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* Cu = nist->FindOrBuildMaterial("G4_Cu"); // 铜
  G4Material* ScintFiber = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"); // 塑料闪烁体
  G4Material* CerenkovFiber = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE"); // 石英

  // 2. 铜棒固体+逻辑体
  G4Box* solidFullRod = new G4Box("CuRod", CuRod_x/2, CuRod_y/2, CuRod_length/2); //BOX参数：长宽高/2
  G4LogicalVolume* logicCuRod = new G4LogicalVolume(solidFullRod, Cu, "LogicCuRod");

  // 3. 挖孔
  G4Tubs* solidHole = new G4Tubs("Hole", 0, CuRod_holeR, CuRod_length/2, 0, 360*deg);//TUBS参数：内半径、外半径、长度/2、起始角度、跨越角度
  // G4SubtractionSolid* solidCuRod = new G4SubtractionSolid(
  //     "CuRodWithHole",    // 名称
  //     solidFullRod,       // 被减物体（铜棒）
  //     solidHole,          // 要减去的物体（孔）
  //     0,                  // 旋转矩阵（无旋转）
  //     G4ThreeVector(0,0,0) // 位置（中心对齐）
  //   );
  G4LogicalVolume* logicHole = new G4LogicalVolume(solidHole, Air, "LogicHole");
  new G4PVPlacement(0, G4ThreeVector(0,0,0), logicHole, "PhysHole", logicCuRod, false, 0);

  const G4double rt3_2 = 0.866025 * Fiber_d;
  // 4. 闪烁光纤
  G4Tubs* solidScintFiber = new G4Tubs("ScintFiber", 0, Fiber_d/2, CuRod_length/2, 0, 360*deg);
  G4LogicalVolume* logicScintFiber = new G4LogicalVolume(solidScintFiber, ScintFiber, "LogicScintFiber");
  // 闪烁光纤1：顶部（对应图中上方蓝色S）
  new G4PVPlacement(0, G4ThreeVector(0, 0.8*mm, 0), 
                  logicScintFiber, "PhysScintFiber_0", logicHole, false, 0);
  // 闪烁光纤2：底部左侧（对应图中下方左蓝色）
  new G4PVPlacement(0, G4ThreeVector(-rt3_2, -0.4*mm, 0), 
                  logicScintFiber, "PhysScintFiber_1", logicHole, false, 1);
  // 闪烁光纤3：底部右侧（对应图中下方右蓝色）
  new G4PVPlacement(0, G4ThreeVector(rt3_2, -0.4*mm, 0), 
                  logicScintFiber, "PhysScintFiber_2", logicHole, false, 2);
  fScoringVolume = logicScintFiber;

  // 5. 切伦科夫光纤
  G4Tubs* solidCerenkovFiber = new G4Tubs("CerenkovFiber", 0, Fiber_d/2, CuRod_length/2, 0, 360*deg);
  G4LogicalVolume* logicCerenkovFiber = new G4LogicalVolume(solidCerenkovFiber, CerenkovFiber, "LogicCerenkovFiber");
  // 光纤0：左侧上方
  new G4PVPlacement(0, G4ThreeVector(-rt3_2, 0.4*mm, 0), 
                    logicCerenkovFiber, "PhysCerenkovFiber_0", logicHole, false, 0);
  // 光纤1：右侧上方
  new G4PVPlacement(0, G4ThreeVector(rt3_2, 0.4*mm, 0), 
                    logicCerenkovFiber, "PhysCerenkovFiber_1", logicHole, false, 1);
  // 光纤2：中心
  new G4PVPlacement(0, G4ThreeVector(0*mm, 0*mm, 0), 
                    logicCerenkovFiber, "PhysCerenkovFiber_2", logicHole, false, 2);
  // 光纤3：右侧最右方（下方）
  new G4PVPlacement(0, G4ThreeVector(0*mm, -0.8*mm, 0), 
                    logicCerenkovFiber, "PhysCerenkovFiber_3", logicHole, false, 3);
  fScoringVolumeCerenkov = logicCerenkovFiber;

  return logicCuRod;
}
G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager 获取材料管理器实例
  G4NistManager* nist = G4NistManager::Instance();
  // 定义材料
  G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");

  // 定义世界体尺寸
  G4double world_xy = TowerTotal * Tower_spacing + 10.0 * cm;
  G4double world_z = CuRod_length + 5.0 * m;

  // 1. 世界体创建
  G4Box* solidWorld = new G4Box("World", world_xy/2, world_xy/2, world_z/2);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, worldMat, "LogicWorld");
  G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(0,0,0), logicWorld, "PhysWorld",
                                                   0, false, 0, true);

  // 2.单根铜棒逻辑体（复用封装函数）
  G4LogicalVolume* logicSingleCuRod = CreateSingleCuRodLogical(nist);

  // 3. 探测器旋转
  G4RotationMatrix* detRot = new G4RotationMatrix();
  detRot->rotateY(2*deg);
  detRot->rotateX(0.7*deg);

  // 4. Tower 和 CopperRod 放置
  for (G4int towerID = 0; towerID < TowerTotal; towerID++)  // 外层循环：16个tower
  {
    G4int towerX_ID = towerID % 4;  // x方向4个tower(0-1-2-3)
    G4int towerY_ID = towerID / 4;  // y方向4个tower(0-1-2-3)
    G4double tower_x_pos = (towerX_ID - 1.5) * Tower_spacing;
    G4double tower_y_pos = (towerY_ID - 1.5) * Tower_spacing;

    // 内层双层循环：搭建单个tower的16×16铜棒
    for (G4int i = 0; i < RodPerTower; i++)  // x方向16根铜棒
    {
      for (G4int j = 0; j < RodPerTower; j++)  // y方向16根铜棒
      {
        // 计算单根铜棒在tower内的相对位置
        G4double rodX = (i - RodPerTower/2 + 0.5) * CuRod_spacing;
        G4double rodY = (j - RodPerTower/2 + 0.5) * CuRod_spacing;
        G4ThreeVector rodPos(rodX + tower_x_pos, rodY + tower_y_pos, 0);

        // 放置单根铜棒（关联tower位置+探测器旋转，无几何重叠）
        G4String rodName = "PhysCuRod_Tower" + std::to_string(towerID) + "_" + std::to_string(i) + "_" + std::to_string(j);
        new G4PVPlacement(detRot, rodPos, logicSingleCuRod, rodName,
                          logicWorld, false, towerID*RodPerTower*RodPerTower + i*RodPerTower + j, true);
      }
    }
  }
  //
  //always return the physical World
  //
  return physWorld;
}

}

