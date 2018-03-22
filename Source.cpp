//Ask about typecasting 
//Add in the enter pin method
#include <vector>
#include <iostream>

using namespace std;

class StateContext;

enum state { Out_Of_Chocolate, No_Credit, Has_Credit, Dispenses_Chocolate, Maintinence_Mode };
/////////////  Generic 
class State //This is the super class State. It is generic and contains what is common to all states. 
{
protected:
	StateContext* CurrentContext; //every state contains a pointer to the current context. 
public:
	State(StateContext* Context) { CurrentContext = Context; } //This is the constructor. The state that is passed in when the object is created is the context that the FSM will start in.
	virtual ~State(void) {}
};

class StateContext //This is the generic state context class. This class contains the generic information that is common to all StateContext classes 
{ //In this example Chocolate_Dispenser is the state context class. 
protected:
	State* CurrentState = nullptr; //This pointer points to the current state 
	int stateIndex = 0;
	vector<State*> availableStates; //This vector stores the pointers to all of the available states so that the state can be changed. 
public:
	virtual ~StateContext(void);
	virtual void setState(state newState);
	virtual int getStateIndex(void); //finds out the current state 
};
StateContext::~StateContext(void)
{
	for (int i = 0; i < int(this->availableStates.size()); i++) delete this->availableStates[i]; // code that deletes all pointers to all of the prevoiusly saved states 
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
class Transition //This class stores all of the possible transitions. This is so that if something happens in a state which it doesn't have a bespoke response for it will print the generic response
	//////////////////////// Generic ^
{
public:
	virtual bool insertMoney(int) { cout << "Error!" << endl; return false; } //The methods here are virtual. This allows for rutime polymorphism whereby the compiler can decide at runtime whether to call the 
	virtual bool makeSelection(int) { cout << "Error!" << endl; return false; }//base class version or the overidden version declared in the class which inherits from transition. The class in this case being
	virtual bool moneyRejected(void) { cout << "Error!" << endl; return false; } //Chocolate_Dispenser
	virtual bool addChocolate(int) { cout << "Error!" << endl; return false; }
	virtual bool dispense(void) { cout << "Error!" << endl; return false; }
	virtual bool enterPin(int) { cout << "Error!" << endl; return false; }
	virtual bool exit(void) { cout << "Error!" << endl; return false; }
};
////
class ChocoState : public State, public Transition //The choco state class inherits from State and Transition. This means it is a state and also has all of the transition methods. 
{ //All other states will also inherit from this. //This is because this class is the non generic state class.
public:
	ChocoState(StateContext* Context) : State(Context) {}
};
//Classes for each pontential state 
class OutOfChocolate : public ChocoState //Each state inherits from the superclass ChocoState. this means that each state then can overwrite any of the virtual mehtods it inherits this is polymorphism
{ //so that it can create a bespoke response for each action it needs to deal with. but still have a generic response if an action occurs which isn't handled by the FSM eg. entering the pin when the FSM 
public: //is in has credit state. 
	OutOfChocolate(StateContext* Context) :ChocoState(Context) {}
	bool enterPin(int pin); //Each possible transition from a state is a method. This is an example of polymorphism
	bool moneyRejected(void);
};
class NoCredit : public ChocoState
{
public:
	NoCredit(StateContext* Context) :ChocoState(Context) {}
	bool insertMoney(int credit); //Each method in each of the state classes is a possible transition. These methods
	bool enterPin(int pin); //overwrite the methods inside transition
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
class Chocolate_Dispenser : public StateContext, public Transition //This class inherits from State Context. The purpose of this class is to store information about the FSM.
{ //Context in this case is information such as the inventory, credit and pin. The context is information about the FSM that will change how each of the states will react to inputs. 
	friend class OutOfChocolate; //Each of the state classes are friends of chocolate dispenser this means that any friend can access anything private or less inside Chocolate_Dispenser
	friend class NoCredit;  //This means that all of the state classes can access any information about the StateContext. This is neccecarry because how the state will react to inputs will depend on the context.
	friend class HasCredit;
	friend class DispensesChocolate;
	friend class MaintenanceMode;
private:
	int inventory = 0; // number of chocolate
	int credit = 0; // a measure of the number of bars that can be mpurchased and not money 
	int pin = 54321;
public:
	Chocolate_Dispenser(void);
	bool insertMoney(int credit); //These are the possible transitions. 
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
//These methods ------------------  are used to pass information up to the higher classes 
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
//----------------
bool NoCredit::insertMoney(int credit) //Each of these methods is a possible interaction that can occour with the FSM these overwrite the the virtual functions declared in transition. 
{ //The insertMoney method bespoke for NoCredit takes in an integer which will be the number of bars the person can buy
	((Chocolate_Dispenser*)CurrentContext)->credit += credit; // The amount of credit is then increased by the amount passed into the method.
	cout << "Adding credit ... Credit = " << ((Chocolate_Dispenser*)CurrentContext)->credit << endl; 
	CurrentContext->setState(Has_Credit);
	return true;
}
bool HasCredit::insertMoney(int credit) //This is the bespoke method for inserting money when the FSM is in the HasCredit state. It takes in an integer called credit so it can 
{ //update the credit variable and therefore update the overall context of the FSM
	((Chocolate_Dispenser*)CurrentContext)->credit += credit;
	cout << "Adding credit ... Credit = " << ((Chocolate_Dispenser*)CurrentContext)->credit << endl;
	return true;
}
bool HasCredit::makeSelection(int option) //This method handles the logic to decide if the amount of chocolate the user has selected is possible for them to dispense based on 
{//the contextual information inventory and credit. 
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
bool HasCredit::moneyRejected(void) //This method sets the context to No_Credit 
{
	cout << "Rejecting money...." << endl;
	((Chocolate_Dispenser*)CurrentContext)->credit = 0;
	CurrentContext->setState(No_Credit); //
	return true;
}
bool MaintenanceMode::addChocolate(int number)
{
	((Chocolate_Dispenser*)CurrentContext)->inventory += number;
	cout << "Adding chocolate... Inventory = " << ((Chocolate_Dispenser*)CurrentContext)->inventory << endl;
	return true;
}
bool MaintenanceMode::exit() //This method is used to exit maintinence mode. It handles the logic of which state to move to based on the contextual information inventory and credit 
{ 
	if (((Chocolate_Dispenser*)CurrentContext)->inventory > 0 && ((Chocolate_Dispenser*)CurrentContext)->credit == 0)
	{
		CurrentContext->setState(No_Credit);
		return true;
	}
	else if (((Chocolate_Dispenser*)CurrentContext)->inventory > 0 && ((Chocolate_Dispenser*)CurrentContext)->credit > 0)
	{
		CurrentContext->setState(Has_Credit);
		return true;
	}
	else
	{
		CurrentContext->setState(Out_Of_Chocolate);
		return true;
	}
}
bool DispensesChocolate::dispense(void) //This method displayes the current inventory and credit it then deducts 
{
	cout << "Dispensing..." << endl;
	cout << "Inventory = " << ((Chocolate_Dispenser*)CurrentContext)->inventory << endl;
	cout << "Credit = " << ((Chocolate_Dispenser*)CurrentContext)->credit << endl;

	if (((Chocolate_Dispenser*)CurrentContext)->inventory == 0) CurrentContext->setState(Out_Of_Chocolate);
	else if (((Chocolate_Dispenser*)CurrentContext)->credit == 0) CurrentContext->setState(No_Credit);
	else CurrentContext->setState(Has_Credit);
	return true;
}
bool OutOfChocolate::enterPin(int pin) //This method takes the pin as an argument of type integer and then if the pin matches the pin stored in the context it will change the state to MaintinenceMode 
{
	if (pin == ((Chocolate_Dispenser*)CurrentContext)->pin)
	{
		((Chocolate_Dispenser*)CurrentContext)->setState(Maintinence_Mode);
	}
	else
	{
		cout << "Incorrect pin " << endl;
	}
	return true;
}
bool OutOfChocolate::moneyRejected(void) //This method simply remains at the same state 
{
	cout << "Rejecting money...." << endl;
	((Chocolate_Dispenser*)CurrentContext)->credit = 0;
	CurrentContext->setState(Out_Of_Chocolate);
	return true;
}
bool NoCredit::enterPin(int pin) //This method takes the pin as an argument of type integer and then if the pin matches the pin stored in the context it will change the state to MaintinenceMode 
{ 
	if (pin == ((Chocolate_Dispenser*)CurrentContext)->pin)
	{
		((Chocolate_Dispenser*)CurrentContext)->setState(Maintinence_Mode);
	}
	else
	{
		cout << "Incorrect pin " << endl;
	}
	return true;
}

int main(void)
{
	Chocolate_Dispenser MyDispenser;

	MyDispenser.addChocolate(10); //Should produce error
	MyDispenser.makeSelection(10); //Should produce error
	MyDispenser.enterPin(54321); //Enters the maintinence mode 
	MyDispenser.insertMoney(10); //Should produce error	
	MyDispenser.addChocolate(50); //Should add 50 chocolate bars
	MyDispenser.exit();//Should exit to no credit	
	MyDispenser.enterPin(33);//Should say wrong pin
	MyDispenser.insertMoney(60);
	MyDispenser.makeSelection(60);//Should say there isn't enough chocolate
	MyDispenser.dispense(); //Should produce an error
	MyDispenser.makeSelection(30);
	MyDispenser.dispense();//should dispense leaving 20 chocolate bars and 30 credit
	MyDispenser.moneyRejected();// should leave credit at 0
	MyDispenser.makeSelection(10); //Should produce error
	MyDispenser.dispense(); //should produce error
	MyDispenser.insertMoney(10);
	MyDispenser.makeSelection(10);
	MyDispenser.dispense();


	return 0;
}