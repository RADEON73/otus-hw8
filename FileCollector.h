/**
 * @file FileCollector.h
 * @brief Заголовочный файл для класса FileCollector.
 *
 * Класс FileCollector предназначен для сбора файлов из указанных директорий.
 */
#pragma once
#include "ArgumentParser.h"
#include <vector>
#include <filesystem>
#include <unordered_set>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <mutex>

namespace fs = std::filesystem;

/// Вектор групп путей к файлам сгруппированных по размерам файлов
using FileGroups = std::unordered_map<uintmax_t, std::vector<std::string>>;

/// Прозрачный хэшер для std::string_view
struct string_view_hash
{
	using is_transparent = void; //Включаем поддержку прозрачного хэширования

	std::size_t operator()(std::string_view sv) const {
		return std::hash<std::string_view>{}(sv);
	}
};

/// Прозрачный компаратор для std::string_view
struct string_view_equal
{
	using is_transparent = void; //Включаем поддержку прозрачного сравнения

	bool operator()(std::string_view lhs, std::string_view rhs) const {
		return lhs == rhs;
	}
};

using FilePaths = std::unordered_set<std::string, string_view_hash, string_view_equal>;

/**
 * @class FileCollector
 * @brief Класс для сбора файлов из указанных директорий.
 */
class FileCollector
{
public:
	/**
	 * @brief Конструктор класса FileCollector.
	 * @param data Данные, полученные из аргументов командной строки.
	 */
	explicit FileCollector(const ArgumentParser::ParserData& data);

	/**
	 * @brief Метод для получения групп файлов.
	 * @return Ссылка на группы файлов.
	 */
	FileGroups& fileGroups() { return fileGroups_; }

private:
	/**
	 * @brief Метод для сбора путей к файлам.
	 * @param root Корневая директория.
	 * @param depth Текущая глубина сканирования.
	 * @param maxDepth Максимальная глубина сканирования.
	 * @param paths Набор путей к файлам.
	 */
	void collectPaths(const fs::path& root, size_t depth, size_t maxDepth, FilePaths& paths);

	/**
	 * @brief Метод для обработки директории.
	 * @param dirPath Путь к директории.
	 * @param data Данные, полученные из аргументов командной строки.
	 */
	void processDirectory(const fs::path& dirPath, const ArgumentParser::ParserData& data);

	FileGroups fileGroups_; ///< Группы файлов.
	std::mutex filesMutex_; ///< Мьютекс для синхронизации доступа к files_.
	std::mutex cout_mutex;  ///< Мьютекс для синхронизации вывода в консоль.
};