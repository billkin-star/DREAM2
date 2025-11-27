//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B1/src/DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

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

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  // Envelope parameters
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_Galactic");
  G4Material* bar_mat   = nist->FindOrBuildMaterial("G4_Galactic"); // G4_AIR?
  G4Material* csi_mat   = nist->FindOrBuildMaterial("G4_CESIUM_IODIDE");

  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //
  // World
  //
  G4double world_sizeXY = 2.0*m;
  G4double world_sizeZ  = 2.0*m;

  auto solidWorld = new G4Box("World_S",                           // its name
    0.5 * world_sizeXY, 0.5 * world_sizeXY, 0.5 * world_sizeZ);  // its size

  auto logicWorld = new G4LogicalVolume(solidWorld,  // its solid
    world_mat,                                       // its material
    "World_L");                                        // its name

  auto physWorld = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),                           // at (0,0,0)
    logicWorld,                                // its logical volume
    "World_PV",                                   // its name
    nullptr,                                   // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    checkOverlaps);                            // overlaps checking

  //
  // CrystalBarrel
  //
  G4double r_in = 0.8*m;
  G4double r_out = 1.1*m;
  G4double z_length = 2.0*m;
  G4double phi_start = 0.*deg;
  G4double phi_delta = 360.*deg;

  auto solidCalo = new G4Tubs("CrystalBarrel_S",                    // its name
    r_in, r_out, 0.5 * z_length, phi_start, phi_delta);  // its size

  auto logicCalo = new G4LogicalVolume(solidCalo,  // its solid
    bar_mat,                                     // its material
    "CrystalBarrel_L");                                 // its name

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),          // at (0,0,0)
    logicCalo,                 // its logical volume
    "CrystalBarrel_PV",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking

  //
  // SingleCrystal
  //
  const G4int nPhi = 20; // phi 向 20 组
  const G4int nZ   = 20; // z  向 20 组

  G4double crystal_z_length = z_length / nZ; // 每个晶体在 z 方向的长度
  G4double crystal_phi_delta = phi_delta / nPhi; // 每个晶体在 phi 方向的角度跨度
  
  auto solidCrystal = new G4Tubs("SingleCrystal_S",                    // its name
    r_in, r_out, 0.5 * crystal_z_length, phi_start, crystal_phi_delta);  // its size

  auto logicCrystal = new G4LogicalVolume(solidCrystal,  // its solid
    csi_mat,                                     // its material
    "SingleCrystal_L");                                 // its name

  for (G4int iz = 0; iz < nZ; iz++) {

    G4double z_pos = -0.5 * z_length + (iz + 0.5) * crystal_z_length;

    for (G4int iphi = 0; iphi < nPhi; iphi++) {

      G4double phi_pos = phi_start + iphi * crystal_phi_delta;
      auto rot = new G4RotationMatrix();
      rot->rotateZ(phi_pos);

      new G4PVPlacement(rot,               //rotation
        G4ThreeVector(0, 0, z_pos),              // its position
        logicCrystal,                        // its logical volume
        "SingleCrystal_PV",                     // its name
        logicCalo,                           // its mother  volume
        false,                               // no boolean operation
        iz * nPhi + iphi,                    // copy number
        checkOverlaps);                      // overlaps checking
    }
  }

  //
  //always return the physical World
  //
  return physWorld;
}

// Magnetic Field

void DetectorConstruction::ConstructSDandField()
{ 
  auto magField = new G4UniformMagField(G4ThreeVector(0., 0., 0.5 * tesla));

  auto transportationManager =
    G4TransportationManager::GetTransportationManager();

  auto fieldMgr = transportationManager->GetFieldManager();
  fieldMgr->SetDetectorField(magField);
  fieldMgr->CreateChordFinder(magField);
}

}

