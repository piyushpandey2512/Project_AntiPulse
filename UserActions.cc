#include "UserActions.hh"

MyAction::MyAction(const G4String& outputFileName)
 : fOutputFileName(outputFileName) {}

MyAction::~MyAction()
{}

void MyAction::BuildForMaster() const
{
    // Master thread only needs a RunAction
    SetUserAction(new MyRunAction());
}

void MyAction::Build() const
{
	// Add Primary Generator Action
	MyPrimaryParticles *generatePrimaries = new MyPrimaryParticles();
	SetUserAction(generatePrimaries);

	// Add Run Action
	MyRunAction *runAction = new MyRunAction();
    runAction->SetOutputFileName(fOutputFileName);  // << Set it directly
    SetUserAction(runAction);
	
	//Add Event Action
	MyEventAction *eventAction = new MyEventAction(runAction);
	SetUserAction(eventAction);

	   
	
	//Add Stepping Action
	MySteppingAction *steppingAction = new MySteppingAction(eventAction);
	SetUserAction(steppingAction);
}

