#include "App.h"
#include <memory>

int main() 
{
	std::unique_ptr appUPtr{ std::make_unique<ave::App>("GP2 Assignment", 690, 420, true) };

	appUPtr->Run();

	return 0;
}