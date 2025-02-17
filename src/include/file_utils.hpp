#pragma once
#include <filesystem>
#include <opencv2/core.hpp>

namespace fs_utils {
	enum class FindFilesKind : std::uint8_t
	{
		FLTA_FILES = 1 << 0,
		FLTA_FOLDERS = 1 << 1,
		FLTA_FILESFOLDERS = FLTA_FILES | FLTA_FOLDERS,
	};

	FindFilesKind operator| (FindFilesKind lhs, FindFilesKind rhs);
	FindFilesKind operator& (FindFilesKind lhs, FindFilesKind rhs);

	CV_EXPORTS_W void findFiles(
		CV_OUT std::vector<std::string>& matches,
		const std::string& path,
		const std::string& directory,
		FindFilesKind flags = FindFilesKind::FLTA_FILESFOLDERS,
		bool relative = true
	);

	CV_EXPORTS_W std::string findFile(
		const std::string& path,
		const std::string& directory = std::filesystem::current_path().string(),
		const std::string& filter = "",
		const std::vector<std::string>& hints = std::vector<std::string>(1, ".")
	);

	CV_EXPORTS_W std::string absolute(const std::string& path);
	CV_EXPORTS_W std::string current_path();
	CV_EXPORTS_W bool exists(const std::string& path);
	CV_EXPORTS_W bool equivalent(const std::string& p1, const std::string& p2);
	CV_EXPORTS_W std::string temp_directory_path();
}