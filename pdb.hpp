#pragma once

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <atlbase.h>
#include <dia2.h>

struct symbol_info {
	std::wstring decorated_name;
	std::wstring undecorated_name;
	std::uintptr_t start_rva;
	std::uintptr_t end_rva;
};

class pdb_parser {
public:
	pdb_parser(std::string path);
	void parse();
	void print_info();

private:
	std::string pdb_path;
	std::vector<symbol_info> info;
};