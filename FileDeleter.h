/**
 * @file FileDeleter.h
 * @brief ������������ ���� ��� ������ FileDeleter.
 *
 * ����� FileDeleter ������������ ��� �������� ���������� ������.
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
 * @brief ����� ��� �������� ���������� ������.
 */
class FileDeleter
{
public:
    /**
     * @brief ����������� ������ FileDeleter.
     */
    FileDeleter() = default;

    /**
     * @brief ����� ��� �������� ���������� � ������ ������.
     * @param fileGroup ������ ������-����������.
     */
    void deleteDuplicates(const std::vector<std::string>& fileGroup);

private:
    std::mutex outputMutex_; ///< ������� ��� ������������� ������.
};
