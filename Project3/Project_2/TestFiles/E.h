namespace nameE {
	int globalvariable = 10;
	typedef struct Estruct {
		int e1;
		int e2;
	};

	class E {
	public:
		E();
		~E();
		void methodE();
	private:
		struct Estruct ex;
	};
}