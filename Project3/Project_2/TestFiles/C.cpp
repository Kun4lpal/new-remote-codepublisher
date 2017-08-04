std::vector<int> C::getVector1()
{
    return testVec;
}

std::vector<int> C::getVector2() {
	return androidVec;
}

int main(){
	C c;
	bool glambda = [](int a, int b) { return a < b; };
	std::vector<int> testv1 = c.getVector1(); 
	return 0;
}