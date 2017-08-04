namespace nameE {
	using namespace nameB;
	typedef Graph<std::string, std::string> graph;
	E::E()
	{
		std::cout << "Constructor of E";
	}
	E::~E()
	{
		std::cout << "Destructor of E";
	}
	E::methodE() {
		std::cout << 20 << " : " << 21;
		std::cout << ex.e1;
		std::cout << ex.e2;
	}

	int main() {
		return 0;
	}
}