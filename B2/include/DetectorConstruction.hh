/// \file B2/include/DetectorConstruction.hh
/// \brief Definition of the B2::DetectorConstruction class

#ifndef B2DetectorConstruction_h
#define B2DetectorConstruction_h 1
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4NistManager.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4UniformMagField;
class G4FieldManager;
class G4GlobalMagFieldMessenger;

namespace B2
{

/// Detector construction class to define materials and geometry.
/// and global uniform magnetic field.

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction() = default;
    ~DetectorConstruction() override = default;

    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override {};

    G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }
    G4LogicalVolume* GetScoringVolumeCerenkov() const { return fScoringVolumeCerenkov; }

  private:
    G4LogicalVolume* CreateSingleCuRodLogical(G4NistManager* nist); // 声明封装函数 
    G4LogicalVolume* fScoringVolume = nullptr;
    G4LogicalVolume* fScoringVolumeCerenkov = nullptr;
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
