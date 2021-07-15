#include "aux.h"
#include "Qubit.h"

using namespace std;

int main(int argc, char* argv[])
{
		cout << "Hello simulator!" << endl;

		Qubit q1(1);

		cout << q1.getId() << endl;

		return 0;
}