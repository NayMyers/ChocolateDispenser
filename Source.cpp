//Ask about _(int) 
//Ask about typecasting 
#include <vector>
#include <iostream>

using namespace std;

class StateContext;

enum state {Out_Of_Chocolate, No_Credit, Has_Credit, Dispenses_Chocolate};
///////////// 
class State
{
protected:
	StateContext* CurrentContext;
public:
	State(StateContext* Context) { CurrentContext = Context; }
	virtual ~State(void) {}
};

class StateContext
{
protected:
	State* CurrentState = nullptr;
	int stateIndex = 0;
	vector<State*> availableStates;
public:
	virtual ~StateContext(void);
	virtual void setState(state newState);
	virtual int getStateIndex(void);
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
////////////
class Transition
{
public:
	virtual bool insertMoney(int) { cout << "Error!" << endl; return false; }
	virtual bool makeSelection(int) { cout << "Error!" << endl; return false; }
	virtual bool moneyRejected(void) { cout << "Error!" << endl; return false; }
	virtual bool addChocolate(int) { cout << "Error!" << endl; return false; }
	virtual bool dispense(void) { cout << "Error!" << endl; return false; }
};
class ChocoState : public State, public Transition
{
public:
	ChocoState(StateContext* Context) : State(Context) {}
};
class OutOfChocolate : public ChocoState
{
public:
	OutOfChocolate(StateContext* Context) : ChocoState(Context) {}
	bool addChocolate(int number);
};
class NoCredit : public ChocoState
{
public:
	NoCredit(StateContext* Context) :ChocoState(Context) {}
	bool insertMoney(int credit);
};
class HasCredit : public ChocoState
{
	HasCredit(StateContext* Context):ChocoState(Context){}
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
class Chocolate_Dispenser : public StateContext, public Transition
{
	friend class OutOfChocolate;
	friend class NoCredit;
	friend class HasCredit;
	friend class DispensesChocolate;
private:
	int inventory = 0; // number of chocolate
	int credit = 0; // a measure of the number of bars that can be mpurchased and not money 
public:
	Chocolate_Dispenser(void);
	bool insertMoney(int credit);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addChocolate(int number);
	bool dispense(void);
};
Chocolate_Dispenser::Chocolate_Dispenser(void)
{
	this->availableStates.push_back(new OutOfChocolate(this));
	this->availableStates.push_back(new NoCredit(this));
	this->availableStates.push_back(new HasCredit(this));
	this->availableStates.push_back(new DispensesChocolate(this));

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

bool OutOfChocolate::addChocolate(int number)
{
	((Chocolate_Dispenser*)CurrentContext)->inventory += number;
	cout << "Adding chocolate... Inventory = " << ((Chocolate_Dispenser*)CurrentContext)->inventory << endl;
	CurrentContext->setState(No_Credit);
	return true;
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
int main(void)
{
	
	return 0;
}