/**
 * @file FileComparator.h
 * @brief Заголовочный файл для класса FileComparator.
 *
 * Класс FileComparator предназначен для сравнения файлов по их содержимому.
 */
#pragma once
#include "HashCalculator.h"
#include "FileCollector.h"
#include <mutex>
#include <string>
#include <vector>
#include "ArgumentParser.h"
#include <fstream>

 /// Структура для хранения информации о файлах
struct FileInfo
{
	std::ifstream fileStream;
	std::vector<std::string> blockHashes;
	size_t currentBlockIndex = 0;
	bool isUnique = false;

	/// Конструктор по умолчанию
	FileInfo() = default;

	/// Деструктор по умолчанию
	~FileInfo() = default;

	/// Move-конструктор (noexcept)
	FileInfo(FileInfo&& other) noexcept
		: fileStream(std::move(other.fileStream)),
		blockHashes(std::move(other.blockHashes)),
		currentBlockIndex(other.currentBlockIndex),
		isUnique(other.isUnique)
	{
		/// Обнуляем перемещенные данные
		other.currentBlockIndex = 0;
		other.isUnique = false;
	}

	/// Move-оператор присваивания (noexcept)
	FileInfo& operator=(FileInfo&& other) noexcept
	{
		if (this != &other) {
			fileStream = std::move(other.fileStream);
			blockHashes = std::move(other.blockHashes);
			currentBlockIndex = other.currentBlockIndex;
			isUnique = other.isUnique;

			/// Обнуляем перемещенные данные
			other.currentBlockIndex = 0;
			other.isUnique = false;
		}
		return *this;
	}

	/// Удаляем копирующий конструктор и оператор присваивания
	FileInfo(const FileInfo&) = delete;
	FileInfo& operator=(const FileInfo&) = delete;
};

/**
 * @class FileComparator
 * @brief Класс для сравнения файлов по их содержимому.
 */
class FileComparator
{
public:
	/**
	 * @brief Конструктор класса FileComparator.
	 * @param files Группы файлов для сравнения.
	 * @param data Данные, полученные из аргументов командной строки.
	 */
	FileComparator(FileGroups& files, const ArgumentParser::ParserData& data)
		: files_(files), hashCalculator_(data.hashAlgorithm.get(), data.blockSize), blockSize_(data.blockSize)
	{
	}

	/**
	 * @brief Метод для сравнения групп файлов.
	 */
	void compareGroups();

private:
	/**
	 * @brief Метод для сравнения группы файлов.
	 * @param filePaths Список путей к файлам для сравнения.
	 */
	void compareGroup(const std::vector<std::string>& filePaths);

	FileGroups& files_; ///< Группы файлов.
	HashCalculator hashCalculator_; ///< Калькулятор хэшей.
	size_t blockSize_; ///< Размер блока для чтения файлов.
	std::mutex outputMutex_; ///< Мьютекс для синхронизации вывода.
	std::mutex cout_mutex;  ///< Мьютекс для синхронизации вывода в консоль.
};