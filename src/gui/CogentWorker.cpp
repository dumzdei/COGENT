#include "gui/CogentWorker.h"

CogentWorker::CogentWorker(QObject* parent) : QObject(parent) {
}

CogentWorker::~CogentWorker() {
}

void CogentWorker::process(const QString& pathStr, int formatInt) {
    std::filesystem::path path(pathStr.toStdString());
    emit logMessage("=== Начало обработки ===");

    if (!std::filesystem::exists(path)) {
        emit logMessage(QString("ОШИБКА: Путь \"%1\" не существует.").arg(pathStr));
        emit finished(false, "Путь не существует");
        return;
    }
    if (!std::filesystem::is_directory(path)) {
        emit logMessage(QString("ОШИБКА: \"%1\" не является директорией.").arg(pathStr));
        emit finished(false, "Путь не является директорией");
        return;
    }

    // --- ЧТЕНИЕ И ПАРСИНГ ---
    Parser* parser = nullptr;
    std::vector<Module> modules;

    emit logMessage("Чтение входных файлов...");

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (!entry.is_regular_file()) continue;

        emit logMessage(QString("Обработка файла: '%1'").arg(QString::fromStdString(entry.path().string())));

        parser = GetParser(entry.path().string());
        if (!parser) {
            emit logMessage("  [ПРЕДУПРЕЖДЕНИЕ] Неподдерживаемый формат, пропущен.");
            continue;
        }

        auto parsed_modules = parser->Parse(entry.path().string());
        if (parsed_modules.empty()) {
            emit logMessage("  [ПРЕДУПРЕЖДЕНИЕ] Модули не найдены в файле.");
        }
        else {
            emit logMessage(QString("  [УСПЕХ] Найдено модулей: %1").arg(parsed_modules.size()));
            modules.insert(modules.end(), parsed_modules.begin(), parsed_modules.end());
        }
        FreeParser(&parser);
    }

    emit logMessage(QString("\nВсего найдено модулей: %1").arg(modules.size()));
    emit logMessage("Запись результата...");

    // --- ЭКСПОРТ ---
    OutputFormat outFormat = static_cast<OutputFormat>(formatInt);
    Exporter* exporter = GetExporter(outFormat);
    if (!exporter) {
        emit logMessage("ОШИБКА: Неподдерживаемый формат вывода.");
        emit finished(false, "Неподдерживаемый формат вывода");
        return;
    }

    std::string theme_name = "dark";
    int export_result = exporter->Export(modules, theme_name);
    if (export_result != 0) {
        emit logMessage(QString("ОШИБКА: Экспорт завершился с кодом %1").arg(export_result));
        emit finished(false, "Ошибка экспорта");
        return;
    }

    if (outFormat == OutputFormat::asciidoc) {
        auto* adoc_exp = dynamic_cast<Exporter_ADOC*>(exporter);
        if (adoc_exp) {
            int pl_result = adoc_exp->ExportPortList(modules);
            if (pl_result != 0) {
                emit logMessage(QString("ОШИБКА: Экспорт списка портов завершился с кодом %1").arg(pl_result));
            }
            else {
                emit logMessage("УСПЕХ: Список портов сохранен в 'portlist.adoc'");
            }
        }
    }

    emit logMessage("\n=== ВСЕ ГОТОВО! ===");
    emit finished(true, "Документация успешно сгенерирована!");
}