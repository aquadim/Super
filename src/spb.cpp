#include <iostream>
#include <filesystem>
#include <argparse/argparse.hpp>
#include <pugixml.hpp>
#include "ids.hpp"
#include "xmltools.hpp"
#include <vector>
#include <stdexcept>
#include "objects.hpp"
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

// Обработка языка
objects::Language collectLanguage(pugi::xml_node config) {
    std::string name    = config.child("id").text().get();
    lstring synonym     = xmltools::parseLocalisedString(
        config.child("synonym")
    );
    std::string comment = config.child("comment").text().get();
    std::string code    = config.child("code").text().get();
    
    return objects::Language{name, synonym, comment, code};
}

// Обработка справочника
objects::Catalog collectCatalog(pugi::xml_node config) {
    std::string name    = config.child("id").text().get();
    lstring synonym     = xmltools::parseLocalisedString(config.child("synonym"));
    std::string comment = config.child("comment").text().get();

    // Реквизиты
    objects::PropertyList properties{};
    for (
        pugi::xml_node property = config.child("properties").child("property");
        property;
        property = property.next_sibling("property")
    )
    {
        std::string propName    = property.child("id").text().get();
        lstring propSynonym     = xmltools::parseLocalisedString(property.child("synonym"));
        std::string comment     = property.child("comment").text().get();
        typing::Type* type      = xmltools::parseTypeNode(property.child("type"));

        properties.add(propName, propSynonym, comment, type);
    }
    
    // Табличные части
    objects::TabularsList tabulars{};
    for (
        pugi::xml_node tabularSection = config.child("tabular-sections").child("tabular-section");
        tabularSection;
        tabularSection = tabularSection.next_sibling("tabular-section")
    )
    {
        std::string tabularName     = tabularSection.child("id").text().get();
        lstring tabularSynonym      = xmltools::parseLocalisedString(tabularSection.child("synonym"));
        std::string tabularComment  = tabularSection.child("comment").text().get();

        // Сбор колонок табличной части
        std::vector<std::shared_ptr<objects::TabularColumn>> tabularColumns{};
        for (
            pugi::xml_node tabularColumn = tabularSection.child("columns").child("column");
            tabularColumn;
            tabularColumn = tabularColumn.next_sibling("column")
        )
        {
            std::string tabularColName      = tabularColumn.child("id").text().get();
            lstring tabularColSynonym       = xmltools::parseLocalisedString(
                tabularColumn.child("synonym")
            );
            std::string tabularColComment   = tabularColumn.child("comment").text().get();
            typing::Type* tabularColType    = xmltools::parseTypeNode(
                tabularColumn.child("type")
            );

            tabularColumns.push_back(std::make_shared<objects::TabularColumn>(
                tabularColName,
                tabularColSynonym,
                tabularColComment,
                tabularColType
            ));
        }

        // Добавление табличной части
        tabulars.add(objects::TabularSection{
            tabularName,
            tabularSynonym,
            tabularComment,
            tabularColumns
        });
    }

    return objects::Catalog{name, synonym, comment, properties, tabulars};
}

// Обработка документа
objects::Document collectDocument(pugi::xml_node config) {
    std::string name    = config.child("id").text().get();
    lstring synonym     = xmltools::parseLocalisedString(config.child("synonym"));
    std::string comment = config.child("comment").text().get();

    // Реквизиты
    objects::PropertyList properties{};
    for (
        pugi::xml_node property = config.child("properties").child("property");
        property;
        property = property.next_sibling("property")
    )
    {
        std::string propName    = property.child("id").text().get();
        lstring propSynonym     = xmltools::parseLocalisedString(property.child("synonym"));
        std::string comment     = property.child("comment").text().get();
        typing::Type* type      = xmltools::parseTypeNode(property.child("type"));

        properties.add(propName, propSynonym, comment, type);
    }
    
    // Табличные части
    objects::TabularsList tabulars{};
    for (
        pugi::xml_node tabularSection = config.child("tabular-sections").child("tabular-section");
        tabularSection;
        tabularSection = tabularSection.next_sibling("tabular-section")
    )
    {
        std::string tabularName     = tabularSection.child("id").text().get();
        lstring tabularSynonym      = xmltools::parseLocalisedString(tabularSection.child("synonym"));
        std::string tabularComment  = tabularSection.child("comment").text().get();

        // Сбор колонок табличной части
        std::vector<std::shared_ptr<objects::TabularColumn>> tabularColumns{};
        for (
            pugi::xml_node tabularColumn = tabularSection.child("columns").child("column");
            tabularColumn;
            tabularColumn = tabularColumn.next_sibling("column")
        )
        {
            std::string tabularColName      = tabularColumn.child("id").text().get();
            lstring tabularColSynonym       = xmltools::parseLocalisedString(
                tabularColumn.child("synonym")
            );
            std::string tabularColComment   = tabularColumn.child("comment").text().get();
            typing::Type* tabularColType    = xmltools::parseTypeNode(
                tabularColumn.child("type")
            );

            tabularColumns.push_back(std::make_shared<objects::TabularColumn>(
                tabularColName,
                tabularColSynonym,
                tabularColComment,
                tabularColType
            ));
        }

        // Добавление табличной части
        tabulars.add(objects::TabularSection{
            tabularName,
            tabularSynonym,
            tabularComment,
            tabularColumns
        });
    }

    return objects::Document{name, synonym, comment, properties, tabulars};
}

template <typename T>
std::vector<T> collectTypes(
    pugi::xml_node includes,
    fs::path projectPath,
    fs::path typeDirectory,
    std::string rootTagName,
    std::string errorMessage,
    T(*collector)(pugi::xml_node config))
{
    std::vector<T> output;
    
    for (pugi::xml_node include = includes.child("include"); include; include = include.next_sibling("include")) {
        // Путь к настройкам объекта
        fs::path objectConfigPath = projectPath / typeDirectory / include.text().get();

        // Получить и прочитать настройки объекта
        spdlog::info("Сбор информации: {}", objectConfigPath.string());
        pugi::xml_document objectConfig;
        if (!objectConfig.load_file(objectConfigPath.c_str())) {
            // Не удалось открыть файл настроек этого объекта
            throw std::runtime_error(errorMessage + " : " + objectConfigPath.string());
        }
        pugi::xml_node objectInfo = objectConfig.child(rootTagName);

        // Обработать объект, добавить в вывод
        T object = collector(objectInfo);
        output.push_back(object);
    }
    
    return output;
}

int main(int argc, char* argv[]) {
    spdlog::set_level(spdlog::level::debug);
    
    // Парсинг аргументов
    argparse::ArgumentParser program("superbuild", "0.0.1");

    // Путь к корневому каталогу проекта
    program.add_argument("-p", "--project");

    // Путь к выходному каталогу выгрузки
    program.add_argument("-o", "--output");

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    srand(time(NULL));

    // Путь к корневому каталогу проекта, объект
    fs::path projectPath = fs::path(program.get<std::string>("project"));

    // Путь к каталогу выгрузки, объект
    fs::path outputPath = fs::path(program.get<std::string>("output"));

    // Загрузка настроек XML проекта
    pugi::xml_document projectDoc;
    if (!projectDoc.load_file((projectPath / "project.xml").c_str())) {
        std::cerr << "Не удалось прочитать файл проекта" << std::endl;
        return 1;
    }
    pugi::xml_node project = projectDoc.child("project");

    // Парсинг языков проекта
    std::vector<objects::Language> languages;
    try {
        languages = collectTypes(
            project.child("languages"),
            projectPath,
            "Languages",
            "language-definition",
            "Не удалось загрузить файл языка",
            &collectLanguage
        );
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // Парсинг справочников проекта
    std::vector<objects::Catalog> catalogs;
    try {
        catalogs = collectTypes(
            project.child("catalogs"),
            projectPath,
            "Catalogs",
            "catalog",
            "Не удалось загрузить файл справочника",
            &collectCatalog
        );
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // Парсинг документов проекта
    std::vector<objects::Document> documents;
    try {
        documents = collectTypes(
            project.child("documents"),
            projectPath,
            "Documents",
            "document",
            "Не удалось загрузить файл документа",
            &collectDocument
        );
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // Файл версий
    pugi::xml_document versionsDoc;
    auto configDumpInfo = versionsDoc.append_child("ConfigDumpInfo");
    configDumpInfo.append_attribute("xmlns").set_value("http://v8.1c.ru/8.3/xcf/dumpinfo");
    configDumpInfo.append_attribute("xmlns:xen").set_value("http://v8.1c.ru/8.3/xcf/enums");
    configDumpInfo.append_attribute("xmlns:xs").set_value("http://www.w3.org/2001/XMLSchema");
    configDumpInfo.append_attribute("xmlns:xsi").set_value("http://www.w3.org/2001/XMLSchema-instance");
    configDumpInfo.append_attribute("format").set_value("Hierarchical");
    configDumpInfo.append_attribute("version").set_value("2.18");
    auto configVersions = configDumpInfo.append_child("ConfigVersions");

    // Конфигурация
    auto confName = project.child("id").text().get();
    lstring confSynonym = xmltools::parseLocalisedString(
        project.child("synonym")
    );
    auto confComment = project.child("comment").text().get();
    auto defaultLanguageName = project.child("default-language").text().get();
    
    objects::Configuration conf{
        confName,
        confSynonym,
        confComment,
        project.child("vendor").text().get(),
        project.child("version").text().get(),
        project.child("update-address").text().get(),
        defaultLanguageName,
        languages,
        catalogs,
        documents
    };
    conf.exportToFiles(outputPath);
    conf.generateConfigVersions(configVersions, "Configuration.");
    versionsDoc.save_file((outputPath / "ConfigDumpInfo.xml").c_str());
}