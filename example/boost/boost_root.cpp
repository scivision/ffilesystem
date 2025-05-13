#include <boost/filesystem.hpp>

#include <string>
#include <iostream>
#include <cstdlib>

namespace fs = boost::filesystem;

int main()
{

#ifndef _WIN32
std::cerr << "This example is for Windows.\n";
#endif

fs::path p1(R"(C:\a\ball.text)");
fs::path p2(R"(\\?\C:\a\ball.text)");
fs::path p3(R"(\\.\C:\a\ball.text)");

for (const auto& p : {p1, p2, p3}) {
    std::cout << "Path: " << p << "\n";
    std::cout << "  root_name: " << p.root_name() << "\n";
    std::cout << "  root_path: " << p.root_path() << "\n";
    std::cout << "  root_directory: " << p.root_directory() << "\n";
    std::cout << "---------------------" << "\n";
}

return EXIT_SUCCESS;
}
