#include "ffilesystem.h"

#include <string_view>
#include <utility>
#include <vector>

#include <boost/ut.hpp>

int main() {
	using namespace boost::ut;

	"drop_slash"_test = [](auto const& test_case) {
		expect(eq(fs_drop_slash(test_case.first), test_case.second));
	} | std::vector<std::pair<std::string_view, std::string_view>>{
			{"", ""},
			{"a", "a"},
			{"a/", "a"},
			{"a/b", "a/b"},
			{"a/b/", "a/b"},
			{"////", "/"},
			{"a////b", "a/b"},
			{"a//b//", "a/b"},
			{"/", "/"},
		};

	if (ffilesystem::is_windows) {
		"drop_slash_windows"_test = [](auto const& test_case) {
			expect(eq(fs_drop_slash(test_case.first), test_case.second));
		} | std::vector<std::pair<std::string_view, std::string_view>>{
				{"c:/", "c:/"},
				{"c:///", "c:/"},
				{"c:/a/b//", "c:/a/b"},
				{R"(\\server\share\a\\b\\)", R"(\\server\share\a\\b\\)"},
			};

		if (fs_win32_long_paths_enabled()) {
			"drop_slash_windows_long_paths"_test = [] {
				expect(eq(fs_drop_slash(R"(\\?\C:/)"), std::string_view{R"(\\?\C:/)"}));
			};
		}
	}
}
