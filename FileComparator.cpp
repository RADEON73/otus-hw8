#include "FileComparator.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <future>

/// Прозрачный хэшер для std::string
struct TransparentStringHash
{
	using is_transparent = void;

	size_t operator()(const std::string& str) const {
		return std::hash<std::string>{}(str);
	}

	size_t operator()(std::string_view str) const {
		return std::hash<std::string_view>{}(str);
	}

	size_t operator()(const char* str) const {
		return std::hash<std::string_view>{}(str);
	}
};

/// Прозрачный компаратор для std::string
struct TransparentStringEqual
{
	using is_transparent = void;

	bool operator()(const std::string& lhs, const std::string& rhs) const {
		return lhs == rhs;
	}

	bool operator()(const std::string& lhs, std::string_view rhs) const {
		return lhs == rhs;
	}

	bool operator()(const std::string& lhs, const char* rhs) const {
		return lhs == rhs;
	}
};

void FileComparator::compareGroups()
{
	std::vector<std::future<void>> futures;
	for (auto const& [gSize, gList] : files_) {
		if (gList.size() < 2)
			continue;
		futures.push_back(std::async(std::launch::async, [this, gList]() {
			compareGroup(gList);
			}));
	}
	for (auto const& future : futures)
		future.wait();
}

void FileComparator::compareGroup(const std::vector<std::string>& filePaths) {
	if (filePaths.empty())
		return;

	std::vector<FileInfo> files;
	for (const auto& filePath : filePaths) {
		files.emplace_back();
		files.back().fileStream.open(filePath, std::ios::binary);
		if (!files.back().fileStream) {
			std::cerr << "Failed to open file: " << filePath << ". File will be skipped." << std::endl;
			continue;
		}
	}

	/// Функция для чтения и хэширования следующего блока файла
	auto readAndHashNextBlock = [&](FileInfo& fileInfo) {
		std::vector<char> buffer(blockSize_, 0);
		fileInfo.fileStream.read(buffer.data(), blockSize_);
		std::streamsize bytesRead = fileInfo.fileStream.gcount();
		if (bytesRead > 0) {
			if (bytesRead < blockSize_)
				std::fill(buffer.begin() + bytesRead, buffer.end(), 0);
			fileInfo.blockHashes.push_back(hashCalculator_.calculateHash(buffer));
		}
		};

	while (!files.empty()) {
		std::unordered_map<std::string, std::vector<size_t>, TransparentStringHash, TransparentStringEqual> hashToFileIndices;
		for (size_t i = 0; i < files.size(); ++i) {
			if (files[i].currentBlockIndex >= files[i].blockHashes.size())
				readAndHashNextBlock(files[i]);
			if (files[i].currentBlockIndex < files[i].blockHashes.size())
				hashToFileIndices[files[i].blockHashes[files[i].currentBlockIndex]].push_back(i);
		}
		if (hashToFileIndices.empty())
			break;
		std::vector<FileInfo> newFiles;
		for (const auto& [hash, fileIndices] : hashToFileIndices) {
			if (fileIndices.size() > 1) {
				for (size_t i : fileIndices) {
					files[i].currentBlockIndex++;
					newFiles.push_back(std::move(files[i]));
				}
			}
		}
		files = std::move(newFiles);
	}

	// Группируем файлы по их полным последовательностям хэшей
	std::unordered_map<std::string, std::vector<std::string>, TransparentStringHash, TransparentStringEqual> hashToFilePaths;
	for (size_t i = 0; i < files.size(); ++i) {
		std::string hashSequence;
		for (const auto& hash : files[i].blockHashes)
			hashSequence += hash;
		hashToFilePaths[hashSequence].push_back(filePaths[i]);
	}

	// Выводим результаты
	std::scoped_lock<std::mutex> lock(outputMutex_);
	for (const auto& [hashSequence, paths] : hashToFilePaths) {
		for (const auto& path : paths)
			std::cout << path << std::endl;
		std::cout << std::endl; // Разделяем группы пустой строкой
	}
}