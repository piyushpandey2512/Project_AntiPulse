#include<iostream>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4AnalysisManager.hh"
#include "GeoConstruction.hh"
#include "PhysicsList.hh"
#include "UserActions.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"


//#include "PrimaryGeneratorAction.hh"

<<<<<<< HEAD
=======

>>>>>>> 3ac4172 (Initial fresh commit)
int main(int argc, char** argv)
{
	G4RunManager *runManager = new G4RunManager();
        runManager->SetUserInitialization(new MyDetectorConstruction());
		
		// Create the physics list using G4PhysListFactory
  		G4int verbose = 1;
   		G4PhysListFactory factory;
    	G4VModularPhysicsList* physlist = factory.GetReferencePhysList("FTFP_INCLXX_EMZ");
		// G4VModularPhysicsList* physlist = factory.GetReferencePhysList("FTFP_BERT_HP");
    	physlist->SetVerboseLevel(verbose);
		physlist->RegisterPhysics(new G4HadronPhysicsFTFP_BERT());

    	runManager->SetUserInitialization(physlist);


        // runManager->SetUserInitialization(new MyPhysicsList());
        runManager->SetUserInitialization(new MyAction());
        runManager->Initialize();

// Set visualization

	G4UIExecutive *ui = 0;
	if (argc == 1)
		{
		 ui = new G4UIExecutive(argc, argv);
		}


<<<<<<< HEAD

// Comment out the following lines to disable visualization

=======
>>>>>>> 3ac4172 (Initial fresh commit)
	G4VisManager *visManager = new G4VisExecutive();
	visManager->Initialize();

	// User Interface manager
	G4UImanager *UImanager = G4UImanager::GetUIpointer();
	

	if(ui)
		{
<<<<<<< HEAD
			UImanager->ApplyCommand("/control/execute /home/piyushp/Desktop/PhD_Work/Trento_Project/Project_AntiPulse/vis.mac");
			ui->SessionStart();
			delete ui;  // Clean up UI. If you want to use visuals, comment this out
=======
		UImanager->ApplyCommand("/control/execute vis.mac");
		ui->SessionStart();
		//delete ui;  // Clean up UI. If you want to use visuals, comment this out
>>>>>>> 3ac4172 (Initial fresh commit)
		}
	else
		{
		G4String command ="/control/execute ";
		G4String filename= argv[1];
		UImanager->ApplyCommand(command+filename);
		}
	


	return 0;

}


// new change


// ********To get the visualization and geant4 interface, comment out the part of codes mentioned below********
<<<<<<< HEAD
	// G4VisManager *visManager = new G4VisExecutive();
	// visManager->Initialize();
=======
// G4UIExecutive *ui = 0;
	// if (argc == 1)
	// 	{
	// 	 ui = new G4UIExecutive(argc, argv);
	// 	}


	// G4VisManager *visManager = new G4VisExecutive();
	// visManager->Initialize();

// if(ui)
		// {
		// UImanager->ApplyCommand("/control/execute vis.mac");
		// ui->SessionStart();
		// delete ui;  // Clean up UI. If you want to use visuals, comment this out
		// }
>>>>>>> 3ac4172 (Initial fresh commit)
