#include "UserActions.hh"

MyAction::MyAction()
{}

MyAction::~MyAction()
{}

void MyAction::Build() const
{
	// Add Primary Generator Action
	MyPrimaryParticles *generatePrimaries = new MyPrimaryParticles();
	SetUserAction(generatePrimaries);

	// Add Run Action
	MyRunAction *runAction = new MyRunAction();
	SetUserAction(runAction);
	
	//Add Event Action
	MyEventAction *eventAction = new MyEventAction(runAction);
	SetUserAction(eventAction);

	// Check if the file name is set via macro (or default to base name)
    G4String outputFileName = G4UImanager::GetUIpointer()->GetCurrentValues("/setOutputFileName");
    if (!outputFileName.empty()) {
        runAction->SetOutputFileName(outputFileName);
    }
	
	//Add Stepping Action
	MySteppingAction *steppingAction = new MySteppingAction(eventAction);
	SetUserAction(steppingAction);
}

