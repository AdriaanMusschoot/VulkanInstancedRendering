#include "App.h"
#include <memory>

int main() 
{
	std::unique_ptr appUPtr{ std::make_unique<ave::App>("GP2 Assignment", 800, 600) };

	appUPtr->Run();

	return 0;
}