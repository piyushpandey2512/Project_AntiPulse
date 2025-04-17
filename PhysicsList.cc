#include "PhysicsList.hh"
#include "G4EmStandardPhysics.hh" 
#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4DecayPhysics.hh"
// #include "G4HadronPhysicsFTFP_INCLXX.hh"
// #include "G4EmStandardPhysics_option3.hh"
// #include "G4EmStandardPhysics_option4.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronPhysicsINCLXX.hh"


// #include "G4QGSP_INCLXX.hh"
// #include "G4QGSP_INCLXX_HP.hh"
// #include "G4FTFP_INCLXX.hh"
// #include "G4FTFP_INCLXX_HP.hh"

MyPhysicsList::MyPhysicsList():G4VModularPhysicsList()
{
	RegisterPhysics(new G4EmStandardPhysics());  // For electromagnetic interactions (ionization, bremstrahlung and pair production for photons, electrons, positrons and charged particles)
    RegisterPhysics(new G4HadronPhysicsFTFP_BERT());
	RegisterPhysics(new G4DecayPhysics());
	// RegisterPhysics(new G4EmStandardPhysics_option3());  // Additional hadronic model for proton-, neutron- and pion-induced reactions below ~3 GeV.
	// RegisterPhysics(new G4EmStandardPhysics_option4());
	RegisterPhysics(new G4HadronPhysicsINCLXX());
	// RegisterPhysics(new G4HadronPhysicsFTFP_INCLXX());
	// RegisterPhysics(new G4HadronElasticPhysicsHP());
	

	// RegisterPhysics(new G4QGSP_INCLXX());
	// RegisterPhysics(new G4QGSP_INCLXX_HP());
	// RegisterPhysics(new G4FTFP_INCLXX());
	// RegisterPhysics(new G4FTFP_INCLXX_HP());
}

MyPhysicsList::~MyPhysicsList(){}

void MyPhysicsList::SetCuts()
{
	G4VUserPhysicsList::SetCuts();
}



/*

*****Physics Lists*****

1. G4EmStandardPhysics():- For electromagnetic interactions (ionization, bremstrahlung and pair production for photons, electrons, positrons and charged particles)
2. G4HadronPhysicsFTFP_BERT():- For hadronic interactions that combines high-energy and low-energy models
	a. FTFP:- Fritiof with Precompound handles high-energy interactions for hadrons and nuclei, typically above a few GeV.
	b. BERT:- Bertini cascade handles low- to intermediate-energy hadronic interactions (below ~ 10 GeV), including intranuclear cascades.
3. G4DecayPhysics():- For particle decay processes which includes decay of unstable particles likes muons, pions, kaons and radiactive nuclei.
4. 

*/