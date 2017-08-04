void func() {

}
A::A(){
	std::cout << "Constructor of A!";
}

A::~A(){
	std::cout << "Destructor of A!";
}

void A::methodA1() {
	std::cout<<"Method of A!";
}

void A::methodA2() {
	std::cout << "Method of A!";
}


int main(){
	A a;
	int value = globalvariable;
	a.methodA1();
	a.methodA2();
	B::dothis();
	globalFuncB(); 
	return 0;
}