// GE_Project.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <gep/gepmodule.h>
#include <gep/globalmanager.h>
#include "gep/weakptr.h"

int main()
{
    std::cout << "Hello World!\n"; 

	g_globalManager.initialize();


	gep::WeakRefIndex data	{ 1000, 1000 };
	std::cout << "data size: " << sizeof(data) << "\n";
	std::cout << "hash: " << data.hash << "\n";
	std::cout << "index: " << data.index << "\n";

	//g_logMessage("trying this shit agen");

	gep::WeakRefIndex data2{ 0, 50 };
	std::cout << "hashcmp: " << data.Compare(data) << "\n";

	
	g_globalManager.destroy();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
