#include "FileDeleter.h"

void FileDeleter::deleteDuplicates(const std::vector<std::string>& fileGroup)
{
    if (fileGroup.size() <= 1) {
        return; // Нечего удалять, если только один файл в группе
    }

    std::scoped_lock<std::mutex> lock(outputMutex_);

    std::cout << "Found duplicate group (" << fileGroup.size() << " files):" << std::endl;
    for (const auto& file : fileGroup) {
        std::cout << "  " << file << std::endl;
    }

    // Сохраняем первый файл, остальные удаляем
    const std::string& fileToKeep = fileGroup[0];

    std::cout << "Keeping file: " << fileToKeep << std::endl;

    for (size_t i = 1; i < fileGroup.size(); ++i) {
        const std::string& fileToDelete = fileGroup[i];

        try {
            if (fs::remove(fileToDelete)) {
                std::cout << "Deleted: " << fileToDelete << std::endl;
            }
            else {
                std::cerr << "Error: Failed to delete file: " << fileToDelete << std::endl;
            }
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "Error deleting file " << fileToDelete << ": " << e.what() << std::endl;
        }
    }

    std::cout << std::endl;
}