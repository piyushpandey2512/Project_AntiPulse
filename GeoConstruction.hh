#ifndef GeoConstruction_hh
#define GeoConstruction_hh

#include "G4NistManager.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SystemOfUnits.hh"

#include "SensitiveDet.hh"

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
 public:	
	MyDetectorConstruction();
	~MyDetectorConstruction();

	virtual G4VPhysicalVolume *Construct();

	const std::vector<G4ThreeVector>& GetModulePositions() const { return modulePositions; }
    G4double GetModuleHalfY() const { return moduleHalfY; }
    G4double GetScinHalfY() const { return scinHalfY; }
    G4double GetFullScinY() const { return fullScinY; }
    G4double GetGap() const { return gap; }
    const std::vector<G4ThreeVector>& GetSources() const { return sources; }

private:
	G4LogicalVolume* logicBGO;
	G4LogicalVolume* fScinLog = nullptr;
	G4LogicalVolume* fScinLogInModule = nullptr;
	G4LogicalVolume* fScinLogInModule_2 = nullptr;
	G4LogicalVolume* l5TTrapLogic = nullptr;
	G4LogicalVolume* moLogic = nullptr;
	G4LogicalVolume* fScintLogical=nullptr;
 
	std::vector<G4ThreeVector> modulePositions;
    std::vector<G4ThreeVector> sources;
    G4double moduleHalfY;
    G4double scinHalfY;
    G4double fullScinY;
    G4double gap;

	G4LogicalVolume* fWorldLogical = nullptr;
	virtual void ConstructSDandField();
	
	G4Material* fScinMaterial = nullptr;

};

#endif
