
using namespace nameB;
B::printB() {
	std::cout << "\nPrint B implementation!\n";
}
int main(){
	Bstruct bs;
	A a;
	a.methodA1();
	a.methodA2();
	bs.methodB1();
	return 0;	
}