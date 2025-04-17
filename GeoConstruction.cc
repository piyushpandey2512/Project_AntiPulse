#include "GeoConstruction.hh"
#include "G4RotationMatrix.hh"
#include "G4VisAttributes.hh"
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <iostream>
#include <iomanip>
#include <vector>

// CADMesh header
#include "CADMesh.hh"

using namespace std;
const G4bool checkOverlaps = false;

MyDetectorConstruction::MyDetectorConstruction()
: fScintLogical(0) // initialize member pointer (assumed declared in header)
{}

MyDetectorConstruction::~MyDetectorConstruction()
{}

G4VPhysicalVolume* MyDetectorConstruction::Construct()
{
  // --- Material and Element Definitions ---
  G4NistManager *nist = G4NistManager::Instance();

  // Copy number for J-PET modules
  G4int cpnJPET = 11;


  // Elements
  G4Element* elH  = nist->FindOrBuildElement("H");
  G4Element* elC  = nist->FindOrBuildElement("C");
  G4Element* elO  = nist->FindOrBuildElement("O");
  G4Element* elSi = nist->FindOrBuildElement("Si");
  G4Element* elMn = nist->FindOrBuildElement("Mn");
  G4Element* elCr = nist->FindOrBuildElement("Cr");
  G4Element* elNi = nist->FindOrBuildElement("Ni");
  G4Element* elFe = nist->FindOrBuildElement("Fe");

  // Materials
  G4Material *volMatWorld = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* Gal_mat     = nist->FindOrBuildMaterial("G4_Galactic");

  // Define Stainless steel 304
  G4double density = 7.999*g/cm3;
  G4Material* mat304steel = new G4Material("Stainless steel 304", density, 6);
  mat304steel->AddElement(elMn, 0.02);
  mat304steel->AddElement(elSi, 0.01);
  mat304steel->AddElement(elCr, 0.19);
  mat304steel->AddElement(elNi, 0.10);
  mat304steel->AddElement(elFe, 0.6792);
  mat304steel->AddElement(elC, 0.0008);

  // Define plastic scintillator material (vinyl-toluene)
  G4Material* fScinMaterial = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

  // Colors for visualization
  G4VisAttributes* blue = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
  G4VisAttributes* visAttScin = new G4VisAttributes(G4Colour(0.105, 0.210, 0.810));
  visAttScin->SetForceWireframe(true);
  visAttScin->SetForceSolid(true);

  // --- World Volume ---
  G4double wRadius = 300*cm;
  G4double wLength = 530*cm;
  G4String wName = "World";

  G4VSolid* wSolid = new G4Tubs(wName, 0, wRadius, wLength/2, 0, 360);
  G4LogicalVolume* wLogic = new G4LogicalVolume(wSolid, Gal_mat, "World");
  G4bool overlapCheck = true;
  G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), wLogic, "World", 0, false, 0, overlapCheck);

  // --- Define Scintillator (single) ---
  // Dimensions (half-lengths)
  G4double scinHalfX = 2.5*cm / 2.0;   // 1.25 cm
  G4double scinHalfY = 0.6*cm / 2.0;    // 0.3 cm
  G4double scinHalfZ = 50.0*cm / 2.0;   // 25.0 cm

  G4Box* scinBox = new G4Box("ScintillatorBox", scinHalfX, scinHalfY, scinHalfZ);
  fScintLogical = new G4LogicalVolume(scinBox, fScinMaterial, "ScintillatorLV");
  fScintLogical->SetVisAttributes(visAttScin);

  // --- Arrange Scintillators as Modules ---
  // Each module consists of 13 scintillators arranged vertically.
  // Compute vertical arrangement parameters:
  G4double gap = 0.1*cm;
  G4double fullScinY = 2*scinHalfY;  // 0.6 cm (full height)
  G4double moduleTotalY = 13*(fullScinY + gap) - gap;  // e.g. 9.0 cm total height
  G4double moduleHalfY = moduleTotalY/2.0;  // 4.5 cm

  // Define module positions according to the requirements:
  // First pair (z = +45 cm): module centers at x = 15 cm and 23 cm.
  // Second pair (z = -45 cm): module centers at x = 15 cm and 23 cm.
  std::vector<G4ThreeVector> modulePositions;
  modulePositions.push_back(G4ThreeVector(15*cm, 0, 45*cm));
  modulePositions.push_back(G4ThreeVector(23*cm, 0, 45*cm));
  modulePositions.push_back(G4ThreeVector(15*cm, 0, -45*cm));
  modulePositions.push_back(G4ThreeVector(23*cm, 0, -45*cm));

  // Loop over each module and place 13 scintillators within.
  for (size_t m = 0; m < modulePositions.size(); m++) {
    G4ThreeVector modCenter = modulePositions[m];
    // For each module, arrange scintillators vertically:
    for (G4int j = 0; j < 13; j++) {
      // Compute local y offset so that scintillators are centered.
      G4double localY = -moduleHalfY + scinHalfY + j * (fullScinY + gap);
      // Global position = module center + local offset along y.
      G4ThreeVector scintPos = modCenter + G4ThreeVector(0, localY, 0);
      // Place scintillator into the world (directly, without an intermediate module volume).
      new G4PVPlacement(0, scintPos, fScintLogical, "Scintillator", wLogic, false, m*100 + j, overlapCheck);
    }
  }

  // --- STL Geometry (CADMesh) ---
  // G4String stlFile = "/home/jlabind/softwares/jpet/geant4/MyProjects/Project_AntiPulse/TotalGravModuleV2.stl";
  G4String stlFile = "/home/piyushp/Desktop/PhD_Work/Trento_Project/Project_AntiPulse/stl_geometry/TotalGravModuleV2.stl";
  CADMesh *cadMesh = new CADMesh(const_cast<char*>(stlFile.c_str()));
  cadMesh->SetScale(1.0);
  G4VSolid *stlSolid = cadMesh->TessellatedMesh();
  
  G4LogicalVolume* stlLogical = new G4LogicalVolume(stlSolid, mat304steel, "STLVolume");
  stlLogical->SetVisAttributes(gray);

  // Rotate and place the STL volume.
  G4RotationMatrix* rotation = new G4RotationMatrix();
  rotation->rotateX(90.0*deg);
  new G4PVPlacement(rotation,
                    G4ThreeVector(-8.0*cm, 3.5*cm, 8.0*cm),
                    stlLogical,
                    "STLVolume",
                    wLogic,
                    false,
                    0,
                    overlapCheck);

  return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
  // sensitive detector instance for the scintillators.
  MySensitiveDetector *scintSD = new MySensitiveDetector("ScintillatorSD");
  // Assign the sensitive detector to the scintillator logical volume.
  if (fScintLogical)
    fScintLogical->SetSensitiveDetector(scintSD);
}
