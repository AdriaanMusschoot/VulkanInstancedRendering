#include "App.h"
#include <memory>

int main() 
{
	std::unique_ptr appUPtr{ std::make_unique<ave::App>("GP2 Assignment", 1920, 1080) };

	appUPtr->Run();

	return 0;
}