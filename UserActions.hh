#ifndef MyAction_HH
#define MyAction_HH

#include "G4VUserActionInitialization.hh"
#include "PrimaryGenerator.hh"

#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "G4UImanager.hh"


class MyAction : public G4VUserActionInitialization
{
public:
	MyAction(const G4String& outputFileName);
	virtual ~MyAction();

//Build() is the virtual function of class G4VUserActionInitialization

	virtual void Build() const; 
	virtual void BuildForMaster() const override;

private:
    G4String fOutputFileName;

};
#endif
