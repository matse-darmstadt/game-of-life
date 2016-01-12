#include <iostream>
#include <functional>

using namespace std;

void myFunction(int i, int j ) {
	cout << "Hallo Welt" << i << j <<endl;
}

int main(int argc, char* argv[]) {
	auto f = [](int i, int j) {
		cout << "bla" << i << j << endl;
	};

	auto f2 = std::bind(myFunction, std::placeholders::_2, std::placeholders::_1);

	f2(4, 5);
	f2(5, 7);

}