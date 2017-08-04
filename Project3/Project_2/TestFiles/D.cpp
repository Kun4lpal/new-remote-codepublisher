D2::D2()
{
	std::cout << "Constructor of DC";
}
D2::~D2()
{
	std::cout << "Destructor of DC";
}

D2::methodD2()
{
	std::cout << structD.val1 << std::endl;
	std::cout << structD.val2 << std::endl;
}

int main(){
	Bstruct b;
	std::string s = b.bstr1;
	return 0;
}