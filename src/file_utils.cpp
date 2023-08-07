#include <file_utils.hpp>

namespace fs = std::filesystem;

namespace {
	using namespace fs_utils;

	inline std::vector<std::string> split(const std::string& s, const std::vector<std::string>& delimiters) {
		size_t pos_start = 0;
		size_t pos_end, pos;
		std::string token;
		std::vector<std::string> res;

		std::string delimiter;
		while (true) {
			pos_end = std::string::npos;

			for (const auto& delim : delimiters) {
				pos = s.find(delim, pos_start);
				if (pos != std::string::npos && (pos_end == std::string::npos || pos < pos_end)) {
					pos_end = pos;
					delimiter = delim;
				}
			}

			if (pos_end == std::string::npos) {
				break;
			}

			token = s.substr(pos_start, pos_end - pos_start);
			pos_start = pos_end + delimiter.length();;
			res.push_back(token);
		}

		res.push_back(s.substr(pos_start));
		return res;
	}

	bool isMatch(const std::string& s, const std::string& p) {
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

	std::string normalize_path(const std::string& path) {
		auto parts = split(path, { "/", "\\" });

		int end = 0;
		for (const auto& part : parts) {
			if (part == "." || part.empty()) {
				continue;
			}

			if (part == "..") {
				end = std::max(0, end - 1);
			}
			else {
				parts[end++] = part;
			}
		}

		if (end == 0) {
			return "";
		}

		fs::path normalized(parts[0]);
		for (int i = 1; i < end; i++) {
			normalized /= parts[i];
		}

		return normalized.string();
	}


	void _findFiles(
		std::vector<std::string>& matches,
		const std::vector<std::string>& parts,
		const fs::path& root_path,
		FindFilesKind flags,
		bool relative,
		int i = 0
	) {
		if (!static_cast<bool>(flags & FindFilesKind::FLTA_FILESFOLDERS)) {
			return;
		}

		const auto last_part = parts.size() - 1;
		bool found = false;
		fs::path dir = root_path;
		std::vector<std::string> next_matches;

		for (; i <= last_part; i++) {
			found = false;
			auto part = parts[i];

			if (
				(i != last_part || (flags & FindFilesKind::FLTA_FOLDERS) == FindFilesKind::FLTA_FOLDERS) &&
				part.find("?") == std::string::npos && part.find("*") == std::string::npos
				) {
				dir = dir / part;
				found = fs::exists(dir);
				if (!found) {
					break;
				}
				continue;
			}

			for (auto const& dir_entry : fs::directory_iterator{ dir }) {
				auto filename = dir_entry.path().filename().string();
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
						matches.push_back(relative ? relpath.string() : filepath.string());
					}

					continue;
				}

				next_matches.clear();
				_findFiles(next_matches, parts, filepath, flags, false, i + 1);

				for (const auto& match : next_matches) {
					filepath = fs::path(match);
					relpath = filepath.lexically_relative(root_path);
					matches.push_back(relative ? relpath.string() : filepath.string());
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
			matches.push_back(relative ? relpath.string() : filepath.string());
		}
	}

	void _findFiles(
		std::vector<std::string>& matches,
		const std::string& path,
		const fs::path& root_path,
		FindFilesKind flags,
		bool relative
	) {
		const auto& parts = split(path, { "/", "\\" });
		_findFiles(matches, parts, root_path, flags, relative);
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

		std::vector<std::string> matches;
		fs::path root_path = fs::absolute(directory);

		while (true) {
			for (const auto& search_path : hints) {
				if (search_path.empty()) {
					continue;
				}

				fs::path spath(search_path);
				if (!filter.empty()) {
					spath = filter / spath;
				}
				spath /= path;

				_findFiles(matches, normalize_path(spath.string()), root_path, FindFilesKind::FLTA_FILESFOLDERS, false);
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
}