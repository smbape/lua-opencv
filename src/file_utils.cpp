#include <file_utils.hpp>

#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef STRICT
#define STRICT
#endif

#include <windows.h>
#endif

namespace fs = std::filesystem;

#ifdef _MSC_VER
#define DOT L"."
#define DOTDOT L".."
#define QUESTION_MARK L"?"
#define STAR L"*"
#else
#define DOT "."
#define DOTDOT ".."
#define QUESTION_MARK "?"
#define STAR "*"
#endif

namespace {
	using namespace fs_utils;

	template <typename string_type>
	bool null_or_empty(const string_type* s) {
		return s == nullptr || *s == 0;
	}

	template <typename string_type>
	void split(const string_type& s, const std::vector<string_type>& delimiters, std::vector<string_type>& res) {
		res.clear();

		size_t pos_start = 0;
		size_t pos_end, pos;
		string_type token;

		string_type delimiter;
		while (true) {
			pos_end = string_type::npos;

			for (const auto& delim : delimiters) {
				pos = s.find(delim, pos_start);
				if (pos != string_type::npos && (pos_end == string_type::npos || pos < pos_end)) {
					pos_end = pos;
					delimiter = delim;
				}
			}

			if (pos_end == string_type::npos) {
				break;
			}

			token = s.substr(pos_start, pos_end - pos_start);
			pos_start = pos_end + delimiter.length();;
			res.push_back(token);
		}

		res.push_back(s.substr(pos_start));
	}

	template <typename string_type>
	bool isMatch(const string_type& s, const string_type& p) {
		const auto slen = s.length();
		const auto plen = p.length();
		int scur = 0;
		int pcur = 0;
		int lastindex = -1;
		int lastmatch = -1;

		while (scur < slen) {
			if (pcur < plen && p[pcur] == '*') {
				while (pcur < plen && p[pcur] == '*') {
					pcur++;
				}
				lastmatch = scur;
				lastindex = pcur;
			}
			else if (p[pcur] == '?' || s[scur] == p[pcur]) {
				scur++;
				pcur++;
			}
			else if (lastindex != -1) {
				scur = ++lastmatch;
				pcur = lastindex;
			}
			else {
				return false;
			}
		}

		while (pcur < plen && p[pcur] == '*') {
			pcur++;
		}

		return pcur == plen;
	}

	void normalize_path(const fs::path& path, std::vector<fs::path::string_type>& normalized) {
		split(fs::path(path).make_preferred().native(), { fs::path::string_type(&fs::path::preferred_separator, 1) }, normalized);

		int end = 0;
		for (const auto& part : normalized) {
			if (part == DOT || end != 0 && part.empty()) {
				continue;
			}

			if (part == DOTDOT) {
				end = std::max(0, end - 1);
			}
#ifdef _MSC_VER
			// https://en.cppreference.com/w/cpp/filesystem/path/append
			// path("C:") / "foo";         // the result is "C:foo"     (appends, without separator)
			else if (end == 0 && part.ends_with(':')) {
				normalized[end++] = part + fs::path::preferred_separator;
			}
#endif
			else {
				normalized[end++] = part;
			}
		}

		normalized.resize(end);
	}

#ifdef _MSC_VER
	/**
	 * Maps a UTF-16 (wide character) string to a new character string. The new character string is not necessarily from a multibyte character set.
	 *
	 * @param  codePage Code page to use in performing the conversion.
	 * @param  c_wstr   Pointer to the Unicode string to convert.
	 * @param  length   Size, in characters, of the string indicated by c_wstr parameter.
	 * @param  str      Pointer to a buffer that receives the converted string.
	 * @return          The number of bytes written to the buffer pointed to by c_str.
	 * @see             https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
	 */
	inline int wcs_to_mbs(UINT codePage, const fs::path::value_type* c_wstr, size_t length, std::string& str) {
		if (null_or_empty(c_wstr)) {
			str.clear();
			return 0;
		}

		int size = WideCharToMultiByte(codePage, 0, c_wstr, length, nullptr, 0, nullptr, nullptr);
		str.assign(size, 0);
		return WideCharToMultiByte(codePage, 0, c_wstr, length, &str[0], size + 1, nullptr, nullptr);
	}

	inline int wcs_to_utf8(const fs::path::value_type* c_wstr, size_t length, std::string& str) {
		return wcs_to_mbs(CP_UTF8, c_wstr, length, str);
	}
#endif

	inline std::string _string_type_to_string(const fs::path::string_type& match) {
#ifdef _MSC_VER
		std::string str; wcs_to_utf8(match.c_str(), match.length(), str);
		return str;
#else
		return match;
#endif
	}

	inline void _addMatch(std::vector<std::string>& matches, const fs::path::string_type& match) {
		matches.push_back(_string_type_to_string(match));
	}

	void _findFiles(
		std::vector<std::string>& matches,
		const std::vector<fs::path::string_type>& parts,
		const fs::path& root_path,
		FindFilesKind flags,
		bool relative,
		int i = 0
	) {
		if (!static_cast<bool>(flags & FindFilesKind::FLTA_FILESFOLDERS)) {
			return;
		}

		using string_type = fs::path::string_type;

		const auto last_part = parts.size() - 1;
		bool found = false;
		fs::path dir = root_path;
		std::vector<std::string> next_matches;

		for (; i <= last_part; i++) {
			found = false;
			auto part = parts[i];

			if (
				(i != last_part || (flags & FindFilesKind::FLTA_FOLDERS) == FindFilesKind::FLTA_FOLDERS) &&
				part.find(QUESTION_MARK) == string_type::npos && part.find(STAR) == string_type::npos
				) {
				dir /= part;
				found = fs::exists(dir);
				if (!found) {
					break;
				}
				continue;
			}

			for (auto const& dir_entry : fs::directory_iterator{ dir }) {
				const auto& filename = dir_entry.path().filename().native();
				if (!isMatch(filename, part)) {
					continue;
				}

				auto filepath = dir / filename;
				auto relpath = filepath.lexically_relative(root_path);

				if (i == last_part) {
					bool is_valid = (flags & FindFilesKind::FLTA_FILESFOLDERS) == FindFilesKind::FLTA_FILESFOLDERS;
					if (!is_valid) {
						auto status = dir_entry.symlink_status();
						is_valid = (flags & FindFilesKind::FLTA_FILES) == FindFilesKind::FLTA_FILES && !fs::is_directory(status) ||
							(flags & FindFilesKind::FLTA_FILES) == FindFilesKind::FLTA_FOLDERS && fs::is_directory(status);
					}

					if (is_valid) {
						_addMatch(matches, relative ? relpath.native() : filepath.native());
					}

					continue;
				}

				next_matches.clear();
				_findFiles(next_matches, parts, filepath, flags, false, i + 1);

				for (const auto& match : next_matches) {
					filepath = fs::path(match);
					relpath = filepath.lexically_relative(root_path);
					_addMatch(matches, relative ? relpath.native() : filepath.native());
				}
			}

			break;
		}

		if (!found) {
			return;
		}

		const auto& filepath = dir;
		auto relpath = filepath.lexically_relative(root_path);

		bool is_valid = (flags & FindFilesKind::FLTA_FILESFOLDERS) == FindFilesKind::FLTA_FILESFOLDERS;
		if (!is_valid) {
			auto status = fs::symlink_status(filepath);
			is_valid = (flags & FindFilesKind::FLTA_FILES) == FindFilesKind::FLTA_FILES && !fs::is_directory(status) ||
				(flags & FindFilesKind::FLTA_FILES) == FindFilesKind::FLTA_FOLDERS && fs::is_directory(status);
		}

		if (is_valid) {
			_addMatch(matches, relative ? relpath.native() : filepath.native());
		}
	}

	void _findFiles(
		std::vector<std::string>& matches,
		const fs::path& path,
		const fs::path& root_path,
		FindFilesKind flags,
		bool relative
	) {
		std::vector<fs::path::string_type> parts; normalize_path(path, parts);
		_findFiles(matches, parts, fs::path(root_path).make_preferred(), flags, relative);
	}
}


namespace fs_utils {
	FindFilesKind operator| (FindFilesKind lhs, FindFilesKind rhs) {
		using FindFilesKindType = std::underlying_type<FindFilesKind>::type;
		return FindFilesKind(static_cast<FindFilesKindType>(lhs) | static_cast<FindFilesKindType>(rhs));
	}

	FindFilesKind operator& (FindFilesKind lhs, FindFilesKind rhs) {
		using FindFilesKindType = std::underlying_type<FindFilesKind>::type;
		return FindFilesKind(static_cast<FindFilesKindType>(lhs) & static_cast<FindFilesKindType>(rhs));
	}

	void findFiles(
		std::vector<std::string>& matches,
		const std::string& path,
		const std::string& directory,
		FindFilesKind flags,
		bool relative
	) {
		_findFiles(matches, path, fs::absolute(directory), flags, relative);
	}

	std::string findFile(
		const std::string& path,
		const std::string& directory,
		const std::string& filter,
		const std::vector<std::string>& hints
	) {
		std::string found;

		if (path.empty()) {
			return found;
		}

		fs::path root_path = fs::absolute(directory);

		if (path == ".") {
			return _string_type_to_string(root_path.native());
		}

		std::vector<std::string> matches;
		bool top_search_absolute = true;

		while (true) {
			bool search_absolute = top_search_absolute;
			for (const auto& search_path : hints) {
				if (search_path.empty()) {
					continue;
				}

				fs::path spath(search_path);

				if (spath.is_absolute()) {
					if (!search_absolute) {
						continue;
					}
					top_search_absolute = false;
				}

				if (!filter.empty()) {
					spath = filter / spath;
				}

				spath /= path;

				_findFiles(matches, spath, root_path, FindFilesKind::FLTA_FILESFOLDERS, false);
				if (!matches.empty()) {
					return matches[0];
				}
			}

			auto parent_path = root_path.parent_path();
			if (parent_path == root_path) {
				break;
			}
			root_path = parent_path;
		}

		return found;
	}

	std::string absolute(const std::string& path) {
		return _string_type_to_string(fs::absolute(path).native());
	}

	std::string current_path() {
		return _string_type_to_string(fs::current_path().native());
	}

	bool exists(const std::string& path) {
		return fs::exists(path);
	}

	bool equivalent(const std::string& p1, const std::string& p2) {
		return fs::equivalent(p1, p2);
	}

	std::string temp_directory_path() {
		return _string_type_to_string(fs::temp_directory_path().native());
	}
}