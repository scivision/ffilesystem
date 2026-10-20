#include "ffilesystem.h"

#include <boost/ut.hpp>

int main() {
using namespace boost::ut;

"root"_test = [] {

expect(fs_root("") == "");
expect(fs_root("a/b") == "");
expect(fs_root("./a/b") == "");
expect(fs_root("../a/b") == "");

if (fs_is_windows()) {
expect(fs_root("c:") == "c:");
expect(fs_root("c:/a/b") == "c:/");
expect(fs_root("/etc") == "/");
expect(fs_root("\\etc") == "\\");
expect(fs_root("c:\\") == "c:\\");
expect(fs_root("c:/") == "c:/");
expect(fs_root("\\") == "\\");
}
else {
expect(fs_root("/a/b") == "/");
expect(fs_root("c:/etc") == "");
}

};

"root_name"_test = [] {
expect(fs_root_name("") == "");
expect(fs_root_name("a/b") == "");
expect(fs_root_name("./a/b") == "");
expect(fs_root_name("../a/b") == "");
expect(!fs_has_root_name(""));

if (fs_is_windows()) {
expect(fs_root_name("c:/a/b") == "c:");
expect(fs_has_root_name("c:/a/b"));
expect(fs_root_name("/etc") == "");
expect(fs_root_name(R"(C:\)") == "C:");

const std::string drive_prefixed = "C:/must-not-be-read";
const std::string_view truncated_drive(drive_prefixed.data(), 1);
expect((truncated_drive == "C") >> fatal);
expect(fs_root_name(truncated_drive) == "")
  << "fs_root_name() read past string_view length for a non-null-terminated path";
}
else {
expect(fs_root_name("/a/b") == "");
expect(fs_root_name("c:/etc") == "");
}

};


"root_directory"_test = [] {
expect(fs_root_directory("") == "");
expect(!fs_has_root_directory(""));

expect(fs_root_directory("a/b") == "");
expect(fs_root_directory("./a/b") == "");
expect(fs_root_directory("../a/b") == "");

if (fs_is_windows()) {
expect(fs_root_directory("c:") == "");

expect(fs_root_directory("c:/a/b") == "/");
expect(fs_has_root_directory("c:/a/b"));

expect(fs_root_directory("/etc") == "/");
expect(fs_has_root_directory("/etc"));

expect(fs_root_directory("\\etc") == "\\");
expect(fs_root_directory("c:\\") == "\\");
expect(fs_root_directory("c:/") == "/");
expect(fs_root_directory("\\") == "\\");
}
else {
expect(fs_root_directory("/a/b") == "/");
expect(fs_has_root_directory("/a/b"));

expect(fs_root_directory("c:/etc") == "");
expect(!fs_has_root_directory("c:/etc"));
}
};
}
