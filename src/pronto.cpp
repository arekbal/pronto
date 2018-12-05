// pronto.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "core.hpp"
#include "command_selector.hpp"

struct command_arg_options {
	command_arg_options(int &argc, char **argv) {
		for (int i = 1; i < argc; ++i)
			this->tokens.emplace_back(argv[i]);
	}

	const str& get(const str &opt) const {
    vec<str>::const_iterator itr;
		itr = std::find(this->tokens.begin(), this->tokens.end(), opt);
		if (itr != this->tokens.end() && ++itr != this->tokens.end())
			return *itr;
		
		static const str empty_string("");
    return empty_string;
	}

	bool exists(const str &opt) const {
		return std::find(this->tokens.begin(), this->tokens.end(), opt)
			!= this->tokens.end();
	}

private:
	vec<str> tokens;
};

int main(int argc, char **argv) {

  auto selector = pronto::command_selector(argc, argv);

  return selector.execute();
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
