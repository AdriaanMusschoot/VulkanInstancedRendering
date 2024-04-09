#include "App.h"
#include <memory>

int main() 
{
	std::unique_ptr appUPtr{ std::make_unique<ave::App>("GP2 Assignemnt", 690, 480, true) };

	appUPtr->Run();

	return 0;
}