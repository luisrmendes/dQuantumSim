#include "consoleUtils.hpp"
#include <iostream>

using namespace std;

string printBold(string content)
{
	string result;
	result += "\033[1m";
	result += content;
	result += "\033[0m";

	return result;
}

string printYellowBold(string content)
{
	string result;
	result += "\033[01;33m";
	result += content;
	result += "\033[0m";

	return result;
}

string printRedBold(string content)
{
	string result;
	result += "\033[01;31m";
	result += content;
	result += "\033[0m";

	return result;
}

string printGreenBold(string content)
{
	string result;
	result += "\033[01;32m";
	result += content;
	result += "\033[0m";

	return result;
}

void clear() { system("clear"); }