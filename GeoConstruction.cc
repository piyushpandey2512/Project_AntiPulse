#include "GeoConstruction.hh"
#include "G4RotationMatrix.hh"
#include "G4VisAttributes.hh"
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <vector>
#include "CADMesh.hh"

G4bool overlapCheck = false;

// Toggle these to select the setup you want
bool useSTLGeometry = true;
<<<<<<< HEAD
bool useFourModuleSetup = false;
bool useFourModuleSetupNewFEE = true;
=======
bool useFourModuleSetup = true;
>>>>>>> 6019b63 (new update)
bool useTestScintillator = false;

MyDetectorConstruction::MyDetectorConstruction() {}

MyDetectorConstruction::~MyDetectorConstruction() {}

G4VPhysicalVolume* MyDetectorConstruction::Construct()
{
    // --- World Volume ---
    wRadius = 300*cm;
    wLength = 530*cm;
    wName = "World";

    // Materials and colors
    DefineMaterials();

    G4VSolid* wSolid = new G4Tubs(wName, 0, wRadius, wLength/2, 0, 360);
    G4LogicalVolume* wLogic = new G4LogicalVolume(wSolid, Gal_mat, "World");
    G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), wLogic, "World", 0, false, 0, overlapCheck);

    // --- STL Geometry ---
    if (useSTLGeometry) {
        G4String stlFile = "/home/piyush/Desktop/PhD_Work/Trento_Project/Project_AntiPulse/stl_geometry/TotalGravModuleV2.stl";
        CADMesh *cadMesh = new CADMesh(const_cast<char*>(stlFile.c_str()));
        cadMesh->SetScale(1.0);
        cadMesh->SetReverse(false);

        G4VSolid *stlSolid = cadMesh->TessellatedMesh();
        G4VisAttributes* gray = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5));
        G4LogicalVolume* stlLogical = new G4LogicalVolume(stlSolid, mat304steel, "STLVolume");
        stlLogical->SetVisAttributes(gray);

        G4RotationMatrix* rotation = new G4RotationMatrix();
        rotation->rotateX(90.0*deg);
        G4ThreeVector stlPosition(-8.0*cm, 3.5*cm, 8.0*cm);
        new G4PVPlacement(rotation, stlPosition, stlLogical, "STLVolume", wLogic, false, 0, overlapCheck);
    }

    // --- Four Module Setup ---
    if (useFourModuleSetup) {
        G4double scinHalfX = 2.5*cm / 2.0;
        G4double scinHalfY = 0.6*cm / 2.0;
        G4double scinHalfZ = 50.0*cm / 2.0;

        G4Box* scinBox = new G4Box("ScintillatorBox", scinHalfX, scinHalfY, scinHalfZ);
        fScintLogical = new G4LogicalVolume(scinBox, fScinMaterial, "ScintillatorLV");
        fScintLogical->SetVisAttributes(visAttScin);

        G4double gap = 0.1*cm;
        G4double fullScinY = 2*scinHalfY;
        G4double moduleTotalY = 13*(fullScinY + gap) - gap;
        G4double moduleHalfY = moduleTotalY/2.0;

        std::vector<G4ThreeVector> modulePositions = {
<<<<<<< HEAD
            G4ThreeVector(15.8*cm, 0, 45*cm),  // Right Front starts at x = 14.55 cm and ends at x = 17.05 cm
            G4ThreeVector(23.8*cm, 0, 45*cm),  // Right Front starts at x = 22.55 cm and ends at x = 25.05 cm
            G4ThreeVector(15.8*cm, 0, -45*cm), // Left Front starts at x = 14.55 cm and ends at x = 17.05 cm
            G4ThreeVector(23.8*cm, 0, -45*cm)  // Left Front starts at x = 22.55 cm and ends at x = 25.05 cm
        };

        for (size_t m = 0; m < modulePositions.size(); m++) {
            G4ThreeVector modCenter = modulePositions[m];
            for (G4int j = 0; j < 13; j++) {
                G4double localY = -moduleHalfY + scinHalfY + j * (fullScinY + gap);
                G4ThreeVector scintPos = modCenter + G4ThreeVector(0, localY, 0);
                new G4PVPlacement(0, scintPos, fScintLogical, "Scintillator", wLogic, false, m*100 + j, overlapCheck);
            }
        }
    }

    // --- Four Module Setup (Modified with new FEE) ---
    if (useFourModuleSetupNewFEE) {
        G4double scinHalfX = 2.5*cm / 2.0;
        G4double scinHalfY = 0.6*cm / 2.0;
        G4double scinHalfZ = 50.0*cm / 2.0;

        G4Box* scinBox = new G4Box("ScintillatorBox", scinHalfX, scinHalfY, scinHalfZ);
        fScintLogical = new G4LogicalVolume(scinBox, fScinMaterial, "ScintillatorLV");
        fScintLogical->SetVisAttributes(visAttScin);

        G4double gap = 0.1*cm;
        G4double fullScinY = 2*scinHalfY;
        G4double moduleTotalY = 13*(fullScinY + gap) - gap;
        G4double moduleHalfY = moduleTotalY/2.0;

        std::vector<G4ThreeVector> modulePositions = {
            G4ThreeVector(15.8*cm, 0, 27*cm),  // Right Front starts at x = 14.55 cm and ends at x = 17.05 cm
            G4ThreeVector(25.8*cm, 0, 27*cm),  // Right Front starts at x = 22.55 cm and ends at x = 25.05 cm
            G4ThreeVector(15.8*cm, 0, -27*cm), // Left Front starts at x = 14.55 cm and ends at x = 17.05 cm
            G4ThreeVector(25.8*cm, 0, -27*cm)  // Left Front starts at x = 22.55 cm and ends at x = 25.05 cm
=======
            G4ThreeVector(15.8*cm, 0, 45*cm),
            G4ThreeVector(23.8*cm, 0, 45*cm),
            G4ThreeVector(15.8*cm, 0, -45*cm),
            G4ThreeVector(23.8*cm, 0, -45*cm)
>>>>>>> 6019b63 (new update)
        };

        for (size_t m = 0; m < modulePositions.size(); m++) {
            G4ThreeVector modCenter = modulePositions[m];
            for (G4int j = 0; j < 13; j++) {
                G4double localY = -moduleHalfY + scinHalfY + j * (fullScinY + gap);
                G4ThreeVector scintPos = modCenter + G4ThreeVector(0, localY, 0);
                new G4PVPlacement(0, scintPos, fScintLogical, "Scintillator", wLogic, false, m*100 + j, overlapCheck);
            }
        }
    }

    // --- Single Test Scintillator ---
    if (useTestScintillator) {
        G4double scinHalfX = 2.5*cm / 2.0;
        G4double scinHalfY = 0.6*cm / 2.0;
        G4double scinHalfZ = 50.0*cm / 2.0;

        G4Box* oneScinBox = new G4Box("ScintillatorBox", scinHalfX, scinHalfY, scinHalfZ);
        oneScinLogical = new G4LogicalVolume(oneScinBox, fScinMaterial, "ScintillatorSingleLV");
        oneScinLogical->SetVisAttributes(visAttScin);

        G4ThreeVector oneScinPos1(10*cm, 0, 0);
        G4ThreeVector oneScinPos2(20*cm, 0, 0);

        new G4PVPlacement(0, oneScinPos1, oneScinLogical, "OneScintillator1", wLogic, false, 0, overlapCheck);
        new G4PVPlacement(0, oneScinPos2, oneScinLogical, "OneScintillator2", wLogic, false, 0, overlapCheck);
    }

    return physWorld;
}


void MyDetectorConstruction::DefineMaterials()
{
    // --- Material and Element Definitions ---
  G4NistManager *nist = G4NistManager::Instance();


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
  Gal_mat     = nist->FindOrBuildMaterial("G4_Galactic");

  // Define Stainless steel 304
  G4double density = 7.999*g/cm3;
  mat304steel = new G4Material("Stainless steel 304", density, 6);
  mat304steel->AddElement(elMn, 0.02);
  mat304steel->AddElement(elSi, 0.01);
  mat304steel->AddElement(elCr, 0.19);
  mat304steel->AddElement(elNi, 0.10);
  mat304steel->AddElement(elFe, 0.6792);
  mat304steel->AddElement(elC, 0.0008);

  // Define plastic scintillator material (vinyl-toluene)
    fScinMaterial = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");    

    // Colors for visualization
    G4VisAttributes* blue = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
    visAttScin = new G4VisAttributes(G4Colour(0.105, 0.210, 0.810));
    visAttScin->SetForceWireframe(true);
    visAttScin->SetForceSolid(true);
}



void MyDetectorConstruction::ConstructSDandField()
{
  // sensitive detector instance for the scintillators.

  MySensitiveDetector *scintSD = new MySensitiveDetector("ScintillatorSD");
  if (fScintLogical != NULL)
    fScintLogical->SetSensitiveDetector(scintSD);

  MySensitiveDetector *oneScinSD = new MySensitiveDetector("oneScinSD");
  if (oneScinLogical != NULL)
    oneScinLogical->SetSensitiveDetector(oneScinSD);
}
