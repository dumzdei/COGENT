#include "gui/CogentWorker.h"
#include <QDir>
#include <QFileInfo>
#include <QThread>
#include <chrono>
#include <thread>

// ⬇️ РАСКОММЕНТИРУЙТЕ, когда будете готовы подключить core
// #include "Parser.h"
// #include "Exporter.h"

void CogentWorker::startGeneration(const QString& inputDir, const QString& outputDir, const QString& format)
{
    //m_cancelRequested.store(0); // Сброс флага отмены
    emit logMessage("⚙️ Инициализация генерации...");
    emit logMessage(QString("📂 Входная папка: %1").arg(inputDir));
    emit logMessage(QString("📤 Выходная папка: %1").arg(outputDir));
    emit logMessage(QString("📝 Формат: %1").arg(format));

    QDir dir(inputDir);
    if (!dir.exists()) {
        emit logMessage("❌ Входная папка не найдена!");
        emit finished(false, "Входная директория не существует.");
        return;
    }

    // Пример рекурсивного поиска файлов
    QFileInfoList files = dir.entryInfoList({ "*.sv", "*.v", "*.vhd", "*.vhdl" }, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
    int totalFiles = files.size();
    emit progressChanged(0, totalFiles);

    if (totalFiles == 0) {
        emit logMessage("⚠️ Файлы не найдены. Проверьте фильтры или путь.");
        emit finished(false, "HDL файлы не найдены.");
        return;
    }

    emit logMessage(QString("🔍 Найдено файлов: %1").arg(totalFiles));

    // =========================================================
    // 🛠 ЗДЕСЬ ВСТАВЛЯЕТЕ ЛОГИКУ ИЗ cogent_core
    // Пример псевдокода:
    // Parser parser;
    // Exporter exporter(format.toStdString());
    // exporter.setOutputDir(outputDir.toStdString());
    // =========================================================

    for (int i = 0; i < totalFiles; ++i) {
        /*if (m_cancelRequested.load()) {
            emit logMessage("🛑 Генерация отменена пользователем.");
            emit finished(false, "Отменено.");
            return;
        }*/

        const auto& file = files[i];
        emit logMessage(QString("📄 Обработка: %1").arg(file.fileName()));

        // 🔍 Вставьте сюда вызов парсера и экспортера для одного файла
        // parser.parse(file.absoluteFilePath().toStdString());
        // exporter.export(...);

        // Эмуляция работы (удалите при интеграции core)
        QThread::msleep(50);

        emit progressChanged(i + 1, totalFiles);
    }

    emit logMessage("✅ Генерация завершена успешно!");
    emit finished(true, "Документация готова.");
}

void CogentWorker::cancel()
{
    //m_cancelRequested.store(1);
    emit logMessage("⏳ Запрос на отмену...");
}