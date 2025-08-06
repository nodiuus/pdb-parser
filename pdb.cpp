#include "pdb.hpp"

pdb_parser::pdb_parser(std::string path) : pdb_path(path) {}

void pdb_parser::parse() {
	// additional scope is added so that COM is uninitialized properly
	{ 
		if (FAILED(CoInitialize(NULL))) {
			std::cerr << "Failed to initialize COM library." << std::endl;
			return;
		}

		CComPtr<IDiaDataSource> source;
		if (FAILED(source.CoCreateInstance(CLSID_DiaSource))) {
			std::cout << source.CoCreateInstance(CLSID_DiaSource) << std::endl;
			std::cerr << "Failed to create IDiaDataSource instance." << std::endl;
			return;
		}

		std::wstring pdb_wpath = std::filesystem::path(pdb_path).wstring();
		if (FAILED(source->loadDataFromPdb(pdb_wpath.c_str()))) {
			std::cerr << "Failed to load PDB file: " << pdb_path << std::endl;
			return;
		}

		CComPtr<IDiaSession> session;
		if (FAILED(source->openSession(&session))) {
			std::cerr << "Failed to open PDB session." << std::endl;
			return;
		}

		CComPtr<IDiaSymbol> global;
		session->get_globalScope(&global);

		CComPtr<IDiaEnumSymbols> symbols;
		if (FAILED(global->findChildren(SymTagFunction, nullptr, nsNone, &symbols))) {
			std::cout << "No functions found in PDB." << std::endl;
			return;
		}

		CComPtr<IDiaSymbol> function;
		ULONG fetched = 0;
		while (SUCCEEDED(symbols->Next(1, &function, &fetched)) && fetched == 1) {
			BSTR decorated = nullptr;
			BSTR undecorated = nullptr;
			DWORD rva = 0;
			std::uintptr_t length = 0;

			function->get_name(&decorated);
			function->get_undecoratedName(&undecorated);
			function->get_relativeVirtualAddress(&rva);

			symbol_info function_info;

			if (decorated) {
				function_info.decorated_name = std::wstring(decorated);
			}
			if (undecorated) {
				function_info.undecorated_name = std::wstring(undecorated);
			}
			if (SUCCEEDED(function->get_length(&length))) {
				function_info.start_rva = rva;
				function_info.end_rva = rva + static_cast<DWORD>(length);
			} else {
				function_info.end_rva = 0;
			}

			info.push_back(function_info);

			if (decorated) SysFreeString(decorated);
			if (undecorated) SysFreeString(undecorated);

			function.Release();
		}
	}
	CoUninitialize();
}

void pdb_parser::print_info() {
	for (int i = 0; i < info.size(); i++) {
		std::wcout << L"Function:    " << info[i].decorated_name << L"\n";
		std::wcout << L"Undecorated: " << info[i].undecorated_name << L"\n";
		std::wcout << L"Start RVA: 0x" << std::hex << info[i].start_rva << std::dec << L"\n\n";
		std::wcout << L"Start RVA: 0x" << std::hex << info[i].end_rva << std::dec << L"\n\n";
	}
}