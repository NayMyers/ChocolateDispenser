//Ask about typecasting 
//Add in the enter pin method
#include <vector>
#include <iostream>

using namespace std;

class StateContext;

enum state { Out_Of_Chocolate, No_Credit, Has_Credit, Dispenses_Chocolate, Maintinence_Mode };
/////////////  Generic ??????????????????????????????????????????????????????
class State //This is the super class State. This stores a pointer to the current context and a has a public mehtod that takes a StateContext and changes
{ //it to be the current state. all of the different StateContexts are stored on the heap, by having the CurrentContext pointer pointing to whatever state the FSM is in 
protected: //it allows the program to be able to execute the code in that context when needed
	StateContext* CurrentContext; //This is the current contextpointer it points to the code that represents the current state of the FSM ????????????????????????????????????????
public:
	State(StateContext* Context) { CurrentContext = Context; }
	virtual ~State(void) {}
};

class StateContext //This is the generic state context class. 
{
protected:
	State* CurrentState = nullptr; //Each StateContext has a pointer of type state called CurrentState to be able to keep track of the current state.
	int stateIndex = 0;
	vector<State*> availableStates; //This vector stores the pointers to all of the available states so that the state can be changed. 
public:
	virtual ~StateContext(void);
	virtual void setState(state newState);
	virtual int getStateIndex(void); //finds out the current state 
};
StateContext::~StateContext(void)
{
	for (int count = 0; count < int(this->availableStates.size()); count++) delete this->availableStates[count]; // code that deletes all pointers to all of the prevoiusly saved states 
}
void StateContext::setState(state newState)
{
	this->CurrentState = availableStates[newState];
	this->stateIndex = newState;
}
int StateContext::getStateIndex(void)
{
	return this->stateIndex;
}
class Transition //This class stores all of the possible transitions. This is so that if something happens in a state which it doesn't have a bespoke response for it will print the generic response Error!
	//////////////////////// Generic ^
{
public:
	virtual bool insertMoney(int) { cout << "Error!" << endl; return false; } //The methods here are virtual. This allows for rutime polymorphism whereby the compiler can decide at runtime whether to call the 
	virtual bool makeSelection(int) { cout << "Error!" << endl; return false; }//base class version or the overidden version declared in the class which inherits from transition.
	virtual bool moneyRejected(void) { cout << "Error!" << endl; return false; }
	virtual bool addChocolate(int) { cout << "Error!" << endl; return false; }
	virtual bool dispense(void) { cout << "Error!" << endl; return false; }
	virtual bool enterPin(int) { cout << "Error!" << endl; return false; }
	virtual bool exit(void) { cout << "Error!" << endl; return false; }
};
////
class ChocoState : public State, public Transition //The choco state class inherits from State and Transition. This means it is a state and also has all of the transition methods.
{
public:
	ChocoState(StateContext* Context) : State(Context) {}
};
//Classes for each pontential state 
class OutOfChocolate : public ChocoState //Each state inherits from the superclass ChocoState. this means that each state then can overwrite any of the virtual mehtods it inherits this is polymorphism
{ //so that it can create a bespoke response for each action it needs to deal with. but still have a generic response if an action occurs which isn't handled by the FSM eg. entering the pin when the FSM 
public: //is in has credit state. 
	OutOfChocolate(StateContext* Context) :ChocoState(Context) {}
	bool enterPin(int pin); //Each possible transition from a state is a method. 
	bool moneyRejected(void);
};
class NoCredit : public ChocoState
{
public:
	NoCredit(StateContext* Context) :ChocoState(Context) {}
	bool insertMoney(int credit);
	bool enterPin(int pin);
};
class HasCredit : public ChocoState
{
public:
	HasCredit(StateContext* Context) : ChocoState(Context) {}
	bool insertMoney(int credit);
	bool makeSelection(int option);
	bool moneyRejected(void);
};
class DispensesChocolate : public ChocoState
{
public:
	DispensesChocolate(StateContext* Context) :ChocoState(Context) {}
	bool dispense(void);
};
class MaintenanceMode : public ChocoState
{
public:
	MaintenanceMode(StateContext* Context) : ChocoState(Context) {}
	bool addChocolate(int number);
	bool exit(void);
};
/////////
class Chocolate_Dispenser : public StateContext, public Transition
{
	friend class OutOfChocolate;
	friend class NoCredit;
	friend class HasCredit;
	friend class DispensesChocolate;
	friend class MaintenanceMode;
private:
	int inventory = 0; // number of chocolate
	int credit = 0; // a measure of the number of bars that can be mpurchased and not money 
	int pin = 54321;
public:
	Chocolate_Dispenser(void);
	bool insertMoney(int credit);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addChocolate(int number);
	bool dispense(void);
	bool enterPin(int pin);
	bool exit(void);

};
Chocolate_Dispenser::Chocolate_Dispenser(void) //This is the constructor for the Chocolate_Dispenser class. This runs when a chocolate dispenser object is created.
{ //This constructor pushes all of the pointers to the possible states into the availableStates vector and then sets the current state to be out of chocolate as this is the base state.
	this->availableStates.push_back(new OutOfChocolate(this));
	this->availableStates.push_back(new NoCredit(this));
	this->availableStates.push_back(new HasCredit(this));
	this->availableStates.push_back(new DispensesChocolate(this));
	this->availableStates.push_back(new MaintenanceMode(this));

	this->setState(Out_Of_Chocolate);
}
bool Chocolate_Dispenser::insertMoney(int credit)
{
	return ((ChocoState*)CurrentState)->insertMoney(credit);
}
bool Chocolate_Dispenser::makeSelection(int option)
{
	return ((ChocoState*)CurrentState)->makeSelection(option);
}
bool Chocolate_Dispenser::moneyRejected(void)
{
	return ((ChocoState*)CurrentState)->moneyRejected();
}
bool Chocolate_Dispenser::addChocolate(int number)
{
	return ((ChocoState*)CurrentState)->addChocolate(number);
}
bool Chocolate_Dispenser::dispense(void)
{
	return ((ChocoState*)CurrentState)->dispense();
}
bool Chocolate_Dispenser::enterPin(int pin)
{
	return ((ChocoState*)CurrentState)->enterPin(pin);
}
bool Chocolate_Dispenser::exit(void)
{
	return ((ChocoState*)CurrentState)->exit();
}

bool NoCredit::insertMoney(int credit)
{
	((Chocolate_Dispenser*)CurrentContext)->credit += credit;
	cout << "Adding credit ... Credit = " << ((Chocolate_Dispenser*)CurrentContext)->credit << endl;
	CurrentContext->setState(Has_Credit);
	return true;
}
bool HasCredit::insertMoney(int credit)
{
	((Chocolate_Dispenser*)CurrentContext)->credit += credit;
	cout << "Adding credit ... Credit = " << ((Chocolate_Dispenser*)CurrentContext)->credit << endl;
	return true;
}
bool HasCredit::makeSelection(int option)
{
	//in this example option = number of bars but could be used to represent a menu choice 
	if (((Chocolate_Dispenser*)CurrentContext)->inventory < option)
	{
		cout << "Error: you have selected more chocolate than the machine contains" << endl;
		return false;
	}
	if (((Chocolate_Dispenser*)CurrentContext)->credit < option)
	{
		cout << "Error: you do not have enough money for that selection" << endl;
		return false;
	}

	cout << "Credit and inventory is sufficient for your selection" << endl;

	((Chocolate_Dispenser*)CurrentContext)->inventory -= option; //deduct inventory 
	((Chocolate_Dispenser*)CurrentContext)->credit -= option; //deduct credit

	CurrentContext->setState(Dispenses_Chocolate);

	return true;

}
bool HasCredit::moneyRejected(void)
{
	cout << "Rejecting money...." << endl;
	((Chocolate_Dispenser*)CurrentContext)->credit = 0;
	CurrentContext->setState(No_Credit);
	return true;
}
bool MaintenanceMode::addChocolate(int number)
{
	((Chocolate_Dispenser*)CurrentContext)->inventory += number;
	cout << "Adding chocolate... Inventory = " << ((Chocolate_Dispenser*)CurrentContext)->inventory << endl;
	return true;
}
bool MaintenanceMode::exit()
{

	if (((Chocolate_Dispenser*)CurrentContext)->inventory > 0 && ((Chocolate_Dispenser*)CurrentContext)->credit == 0)
	{
		CurrentContext->setState(Has_Credit);
		return true;
	}
	else if (((Chocolate_Dispenser*)CurrentContext)->inventory > 0 && ((Chocolate_Dispenser*)CurrentContext)->credit == 0)
	{
		CurrentContext->setState(No_Credit);
		return true;
	}
	else
	{
		CurrentContext->setState(Out_Of_Chocolate);
		return true;
	}
}
bool DispensesChocolate::dispense(void)
{
	cout << "Dispensing..." << endl;
	cout << "Inventory = " << ((Chocolate_Dispenser*)CurrentContext)->inventory << endl;
	cout << "Credit = " << ((Chocolate_Dispenser*)CurrentContext)->credit << endl;

	if (((Chocolate_Dispenser*)CurrentContext)->inventory == 0) CurrentContext->setState(Out_Of_Chocolate);
	else if (((Chocolate_Dispenser*)CurrentContext)->credit == 0) CurrentContext->setState(No_Credit);
	else CurrentContext->setState(Has_Credit);
	return true;
}
bool OutOfChocolate::enterPin(int pin)
{
	if (pin == ((Chocolate_Dispenser*)CurrentContext)->pin)
	{
		((Chocolate_Dispenser*)CurrentContext)->setState(Maintinence_Mode);
	}
	else
	{
		cout << "Incorrect pin: " << endl;
	}
	return true;
}
bool OutOfChocolate::moneyRejected(void)
{
	cout << "Rejecting money...." << endl;
	((Chocolate_Dispenser*)CurrentContext)->credit = 0;
	CurrentContext->setState(Out_Of_Chocolate);
	return true;
}
bool NoCredit::enterPin(int pin)
{
	if (pin == ((Chocolate_Dispenser*)CurrentContext)->pin)
	{
		((Chocolate_Dispenser*)CurrentContext)->setState(Maintinence_Mode);
	}
	else
	{
		cout << "Incorrect pin: " << endl;
	}
	return true;
}

int main(void)
{
	Chocolate_Dispenser MyDispenser;

	MyDispenser.enterPin(54321);
	MyDispenser.addChocolate(50);
	MyDispenser.enterPin(54322);
	MyDispenser.addChocolate(10);
	MyDispenser.insertMoney(20);
	MyDispenser.exit();
	MyDispenser.insertMoney(20);
	MyDispenser.makeSelection(10);
	MyDispenser.enterPin(54321);
	MyDispenser.dispense();

	return 0;
}