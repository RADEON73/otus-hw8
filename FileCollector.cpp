#include "FileCollector.h"
#include <iostream>
#include <future>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <mutex>
#include <string>
#include <vector>
#include "ArgumentParser.h"

std::string to_lower(const std::string& str) {
	std::string result = str;
	std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
		return std::tolower(c);
		});
	return result;
}

FileCollector::FileCollector(const ArgumentParser::ParserData& data) {
	FilePaths allPaths;
	std::vector<std::future<void>> futures;
	for (const auto& path : data.directories) {
		futures.push_back(std::async(std::launch::async, [this, path, &data, &allPaths]() {
			try {
				collectPaths(path, 0, data.level, allPaths);
			}
			catch (const std::exception& e) {
				std::scoped_lock<std::mutex> lock(cout_mutex);
				std::cerr << "Error: Failed to process directory " << path << ": " << e.what() << ". Skipping this directory." << std::endl;
			}
			}));
	}
	for (auto const& future : futures)
		future.wait();
	for (auto const& path : data.excludeDirectories)
		allPaths.erase(path);
	std::vector<std::future<void>> fileFutures;
	for (std::filesystem::path const& dirPath : allPaths) {
		fileFutures.push_back(std::async(std::launch::async, [this, dirPath, &data]() {
			try {
				processDirectory(dirPath, data);
			}
			catch (const std::exception& e) {
				std::scoped_lock<std::mutex> lock(cout_mutex);
				std::cerr << "Error: Failed to process directory " << dirPath << ": " << e.what() << ". Skipping this directory." << std::endl;
			}
			}));
	}
	for (auto const& future : fileFutures)
		future.wait();
}

void FileCollector::collectPaths(const fs::path& root, size_t depth, size_t maxDepth, FilePaths& paths) {
	if (depth > maxDepth)
		return;
	if (!fs::exists(root)) {
		std::scoped_lock<std::mutex> lock(cout_mutex);
		std::cout << "Error: Directory does not exist: " << root << ". Skipping this directory." << std::endl;
		return;
	}
	if (!fs::is_directory(root)) {
		std::scoped_lock<std::mutex> lock(cout_mutex);
		std::cout << "Error: Path is not a directory: " << root << ". Skipping this path." << std::endl;
		return;
	}
	try {
		paths.insert(root.string());
		for (const auto& entry : fs::directory_iterator(root)) {
			if (fs::is_directory(entry)) {
				collectPaths(entry.path(), depth + 1, maxDepth, paths);
			}
		}
	}
	catch (const fs::filesystem_error& e) {
		std::scoped_lock<std::mutex> lock(cout_mutex);
		std::cerr << "Error: Unable to access directory " << root << ": " << e.what() << ". Skipping this directory." << std::endl;
	}
}

void FileCollector::processDirectory(const fs::path& dirPath, const ArgumentParser::ParserData& data) {
	if (fs::exists(dirPath) && fs::is_directory(dirPath)) {
		for (const auto& entry : fs::directory_iterator(dirPath)) {
			if (fs::is_regular_file(entry)) {
				uintmax_t fileSize = fs::file_size(entry);
				auto const& entryPath = entry.path();
				std::string filePath = entryPath.string();
				std::string fileExtension = to_lower(entryPath.extension().string());

				if (!data.masks.empty()) {
					for (auto& mask : data.masks) {
						if (to_lower(mask) != to_lower(fileExtension))
							continue;
						if (fileSize < data.minFileSize)
							continue;
						std::scoped_lock<std::mutex> lock(filesMutex_);
						fileGroups_[fileSize].emplace_back(filePath);
					}
				}
				else {
					if (fileSize < data.minFileSize)
						continue;
					std::scoped_lock<std::mutex> lock(filesMutex_);
					fileGroups_[fileSize].emplace_back(filePath);
				}
			}
		}
	}
}