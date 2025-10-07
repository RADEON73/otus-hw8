/**
 * @file FileDeleter.h
 * @brief Заголовочный файл для класса FileDeleter.
 *
 * Класс FileDeleter предназначен для удаления дубликатов файлов.
 */
#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <mutex>

namespace fs = std::filesystem;

/**
 * @class FileDeleter
 * @brief Класс для удаления дубликатов файлов.
 */
class FileDeleter
{
public:
    /**
     * @brief Конструктор класса FileDeleter.
     */
    FileDeleter() = default;

    /**
     * @brief Метод для удаления дубликатов в группе файлов.
     * @param fileGroup Группа файлов-дубликатов.
     */
    void deleteDuplicates(const std::vector<std::string>& fileGroup);

private:
    std::mutex outputMutex_; ///< Мьютекс для синхронизации вывода.
};
