/**
 * @file ArgumentParser.h
 * @brief Заголовочный файл для класса ArgumentParser.
 *
 * Класс ArgumentParser предназначен для обработки аргументов командной строки и их валидации.
 */
#pragma once
#include "HashCalculator.h"
#include <memory>
#include <string>
#include <vector>

 /**
  * @class ArgumentParser
  * @brief Класс для парсинга аргументов командной строки.
  */
class ArgumentParser
{
public:
	/**
	 * @struct ParserData
	 * @brief Структура для хранения данных, полученных из аргументов командной строки.
	 */
	struct ParserData
	{
		std::vector<std::string> directories; ///< Список директорий для сканирования.
		std::vector<std::string> excludeDirectories; ///< Список директорий для исключения.
		std::vector<std::string> masks; ///< Маски файлов для фильтрации.
		size_t level; ///< Глубина сканирования.
		size_t minFileSize; ///< Минимальный размер файла для обработки.
		std::unique_ptr<IHashAlgorithm> hashAlgorithm; ///< Алгоритм хэширования.
		size_t blockSize{ 1024 }; ///< Размер блока для чтения файлов.
	};

	/**
	 * @enum PARSE_RES_CODE
	 * @brief Коды результата парсинга аргументов.
	 */
	enum class PARSE_RES_CODE
	{
		OK = 0, ///< Успешный парсинг.
		NO_ARGUMENTS, ///< Отсутствуют аргументы.
		PARSE_ERROR, ///< Ошибка парсинга.
		NO_DIRECTORIES, ///< Не указаны директории.
		INVALID_HASH_ALGORITHM ///< Неверный алгоритм хэширования.
	};

	/**
	 * @brief Конструктор класса ArgumentParser.
	 * @param argc Количество аргументов командной строки.
	 * @param argv Массив аргументов командной строки.
	 */
	ArgumentParser(int argc, char* argv[]) : argc(argc), argv(argv) {};

	/**
	 * @brief Деструктор класса ArgumentParser.
	 */
	~ArgumentParser() = default;

	/**
	 * @brief Метод для парсинга аргументов командной строки.
	 * @return Код результата парсинга.
	 */
	PARSE_RES_CODE parse();

	/**
	 * @brief Метод для получения данных, полученных из аргументов.
	 * @return Константная ссылка на структуру ParserData.
	 */
	const ParserData& data() const { return data_; }

private:
	int argc; ///< Количество аргументов командной строки.
	char** argv; ///< Массив аргументов командной строки.

	ParserData data_; ///< Данные, полученные из аргументов.
};