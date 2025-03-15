/**
 * @file IHashAlgorithm.h
 * @brief Заголовочный файл для интерфейса IHashAlgorithm.
 *
 * Интерфейс IHashAlgorithm определяет методы для расчета хэша данных.
 */
#pragma once
#include <vector>
#include <string>
#include <memory>

 /**
  * @class IHashAlgorithm
  * @brief Интерфейс для расчета хэша данных.
  */
class IHashAlgorithm
{
public:

	/**
	 * @brief Деструктор класса IHashAlgorithm.
	 */
	virtual ~IHashAlgorithm() = default;

	/**
	 * @brief Метод для расчета хэша блока данных.
	 * @param block Блок данных.
	 * @return Хэш блока данных.
	 */
	virtual std::string calculateHash(const std::vector<char>& block) const = 0;
};

/**
 * @class HashCalculator
 * @brief Класс для расчета хэша данных с использованием заданного алгоритма.
 */
class HashCalculator
{
public:
	/**
	 * @brief Конструктор класса HashCalculator.
	 * @param algorithm Указатель на алгоритм хэширования.
	 * @param blockSize Размер блока данных.
	 */
	HashCalculator(IHashAlgorithm* algorithm, size_t blockSize);

	/**
	 * @brief Метод для расчета хэша блока данных.
	 * @param block Блок данных.
	 * @return Хэш блока данных.
	 */
	std::string calculateHash(const std::vector<char>& block) const;

private:
	IHashAlgorithm* algorithm_; ///< Указатель на алгоритм хэширования.
	size_t blockSize_; ///< Размер блока данных.
};

/**
 * @class HashAlgorithmFactory
 * @brief Класс для создания объектов алгоритмов хэширования.
 */
class HashAlgorithmFactory
{
public:
	/**
	 * @brief Метод для создания объекта алгоритма хэширования.
	 * @param algorithm Название алгоритма.
	 * @return Указатель на объект алгоритма хэширования.
	 */
	static std::unique_ptr<IHashAlgorithm> create(const std::string_view& algorithm);
};

/**
 * @class CRC32Hash
 * @brief Класс для расчета хэша CRC32.
 */
class CRC32Hash : public IHashAlgorithm
{
public:
	std::string calculateHash(const std::vector<char>& block) const override;
};

/**
 * @class MD5Hash
 * @brief Класс для расчета хэша MD5.
 */
class MD5Hash : public IHashAlgorithm
{
public:
	std::string calculateHash(const std::vector<char>& block) const override;
};