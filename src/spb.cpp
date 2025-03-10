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
using namespace std;

// Обработка языка
void collectLanguage(
    pugi::xml_node config,
    shared_ptr<objects::Configuration> conf
) {
    string name     = config.child("id").text().get();
    lstring synonym = xmltools::parseLocalisedString(config.child("synonym"));
    string comment  = config.child("comment").text().get();
    string code     = config.child("code").text().get();
    string version  = config.child("version").text().get();
    
    conf->addLanguage(make_shared<objects::Language>(
        name,
        synonym,
        comment,
        version,
        conf,
        code
    ));
}

// Обработка справочника
void collectCatalog(
    pugi::xml_node config,
    shared_ptr<objects::Configuration> conf
) {
    string name     = config.child("id").text().get();
    lstring synonym = xmltools::parseLocalisedString(config.child("synonym"));
    string comment  = config.child("comment").text().get();
    string version  = config.child("version").text().get();

    auto catalog = make_shared<objects::Catalog>(
        name,
        synonym,
        comment,
        version,
        conf
    );

    // Список реквизитов
    auto propertyList = make_shared<objects::PropertyList>(catalog);
    for (
        pugi::xml_node property = config.child("properties").child("property");
        property;
        property = property.next_sibling("property")
    ) {
        auto p = make_shared<objects::Property>(
            property.child("id").text().get(),
            xmltools::parseLocalisedString(property.child("synonym")),
            property.child("comment").text().get(),
            property.child("version").text().get(),
            xmltools::parseTypeNode(property.child("type")),
            catalog
        );
        propertyList->add(p);
    }

    // Список табличных частей
    auto tabularsList = make_shared<objects::TabularsList>(catalog);
    for (
        pugi::xml_node ts = config.child("tabular-sections").child("tabular-section");
        ts;
        ts = ts.next_sibling("tabular-section")
    ) {
        auto tabularSection = make_shared<objects::TabularSection>(
            ts.child("id").text().get(),
            xmltools::parseLocalisedString(ts.child("synonym")),
            ts.child("comment").text().get(),
            ts.child("version").text().get(),
            catalog,
            "Catalog"
        );

        // Сбор колонок табличной части
        for (
            pugi::xml_node tabularColumn = ts.child("columns").child("column");
            tabularColumn;
            tabularColumn = tabularColumn.next_sibling("column")
        ) {
            auto tabularColType = xmltools::parseTypeNode(tabularColumn.child("type"));

            tabularSection->addColumn(make_shared<objects::TabularColumn>(
                tabularColumn.child("id").text().get(),
                xmltools::parseLocalisedString(tabularColumn.child("synonym")),
                tabularColumn.child("comment").text().get(),
                tabularColumn.child("version").text().get(),
                tabularSection,
                tabularColType
            ));
        }

        // Добавление табличной части
        tabularsList->add(tabularSection);
    }

    catalog->setPropertyList(propertyList);
    catalog->setTabularsList(tabularsList);
    conf->addCatalog(catalog);
}

// Обработка справочника
void collectDocument(
    pugi::xml_node config,
    shared_ptr<objects::Configuration> conf
) {
    string name     = config.child("id").text().get();
    lstring synonym = xmltools::parseLocalisedString(config.child("synonym"));
    string comment  = config.child("comment").text().get();
    string version  = config.child("version").text().get();

    auto document = make_shared<objects::Document>(
        name,
        synonym,
        comment,
        version,
        conf
    );

    // Список реквизитов
    auto propertyList = make_shared<objects::PropertyList>(document);
    for (
        pugi::xml_node property = config.child("properties").child("property");
        property;
        property = property.next_sibling("property")
    ) {
        auto p = make_shared<objects::Property>(
            property.child("id").text().get(),
            xmltools::parseLocalisedString(property.child("synonym")),
            property.child("comment").text().get(),
            property.child("version").text().get(),
            xmltools::parseTypeNode(property.child("type")),
            document
        );
        propertyList->add(p);
    }

    // Список табличных частей
    auto tabularsList = make_shared<objects::TabularsList>(document);
    for (
        pugi::xml_node ts = config.child("tabular-sections").child("tabular-section");
        ts;
        ts = ts.next_sibling("tabular-section")
    ) {
        auto tabularSection = make_shared<objects::TabularSection>(
            ts.child("id").text().get(),
            xmltools::parseLocalisedString(ts.child("synonym")),
            ts.child("comment").text().get(),
            ts.child("version").text().get(),
            document,
            "Document"
        );

        // Сбор колонок табличной части
        for (
            pugi::xml_node tabularColumn = ts.child("columns").child("column");
            tabularColumn;
            tabularColumn = tabularColumn.next_sibling("column")
        ) {
            auto tabularColType = xmltools::parseTypeNode(tabularColumn.child("type"));

            tabularSection->addColumn(make_shared<objects::TabularColumn>(
                tabularColumn.child("id").text().get(),
                xmltools::parseLocalisedString(tabularColumn.child("synonym")),
                tabularColumn.child("comment").text().get(),
                tabularColumn.child("version").text().get(),
                tabularSection,
                tabularColType
            ));
        }

        // Добавление табличной части
        tabularsList->add(tabularSection);
    }

    document->setPropertyList(propertyList);
    document->setTabularsList(tabularsList);
    conf->addDocument(document);
}

//~ // Обработка перечисления
//~ objects::Enum collectEnum(pugi::xml_node config) {
    //~ string name     = config.child("id").text().get();
    //~ lstring synonym = xmltools::parseLocalisedString(config.child("synonym"));
    //~ string comment  = config.child("comment").text().get();

    //~ // Элементы
    //~ vector<shared_ptr<objects::EnumElement>> elements{};
    //~ for (
        //~ pugi::xml_node element = config.child("elements").child("element");
        //~ element;
        //~ element = element.next_sibling("element")
    //~ )
    //~ {
        //~ string propName     = element.child("id").text().get();
        //~ lstring propSynonym = xmltools::parseLocalisedString(element.child("synonym"));
        //~ string propComment  = element.child("comment").text().get();

        //~ elements.push_back(
            //~ make_shared<objects::EnumElement>(propName, propSynonym, propComment)
        //~ );
    //~ }

    //~ return objects::Enum{name, synonym, comment, elements};
//~ }

void collectTypes(
    pugi::xml_node includes,
    fs::path projectPath,
    fs::path typeDirectory,
    string rootTagName,
    string errorMessage,
    shared_ptr<objects::Configuration> conf,
    void(*collector)(pugi::xml_node config, shared_ptr<objects::Configuration> conf))
{
    for (pugi::xml_node include = includes.child("include"); include; include = include.next_sibling("include")) {
        // Путь к настройкам объекта
        fs::path objectConfigPath = projectPath / typeDirectory / include.text().get();

        // Получить и прочитать настройки объекта
        spdlog::info("Сбор информации: {}", objectConfigPath.string());
        pugi::xml_document objectConfig;
        if (!objectConfig.load_file(objectConfigPath.c_str())) {
            // Не удалось открыть файл настроек этого объекта
            throw runtime_error(errorMessage + " : " + objectConfigPath.string());
        }
        pugi::xml_node objectInfo = objectConfig.child(rootTagName);

        // Обработать объект, добавить в конфигурацию
        collector(objectInfo, conf);
    }
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
    catch (const exception& err) {
        spdlog::error(err.what());
        cerr << program;
        return 1;
    }

    srand(time(NULL));

    // Путь к корневому каталогу проекта, объект
    fs::path projectPath = fs::path(program.get<string>("project"));

    // Путь к каталогу выгрузки, объект
    fs::path outputPath = fs::path(program.get<string>("output"));

    // Загрузка настроек XML проекта
    pugi::xml_document projectDoc;
    if (!projectDoc.load_file((projectPath / "project.xml").c_str())) {
        cerr << "Не удалось прочитать файл проекта" << endl;
        return 1;
    }
    pugi::xml_node project = projectDoc.child("project");

    // Конфигурация
    // Создание объекта конфигурации
    shared_ptr<objects::Configuration> conf = make_shared<objects::Configuration>(
        project.child("id").text().get(),
        xmltools::parseLocalisedString(project.child("synonym")),
        project.child("comment").text().get(),
        project.child("version").text().get(),
        project.child("vendor").text().get(),
        project.child("dev-version").text().get(),
        project.child("update-address").text().get(),
        project.child("default-language").text().get()
    );

    // Парсинг языков проекта
    try {
        collectTypes(
            project.child("languages"),
            projectPath,
            "Languages",
            "language-definition",
            "Не удалось загрузить файл языка",
            conf,
            &collectLanguage
        );
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }

    // Парсинг справочников проекта
    try {
        collectTypes(
            project.child("catalogs"),
            projectPath,
            "Catalogs",
            "catalog",
            "Не удалось загрузить файл справочника",
            conf,
            &collectCatalog
        );
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }

    // Парсинг документов проекта
    try {
        collectTypes(
            project.child("documents"),
            projectPath,
            "Documents",
            "document",
            "Не удалось загрузить файл документа",
            conf,
            &collectDocument
        );
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }
    
    //~ // Парсинг перечислений проекта
    //~ vector<objects::Enum> enums;
    //~ try {
        //~ enums = collectTypes(
            //~ project.child("enums"),
            //~ projectPath,
            //~ "Enums",
            //~ "enum",
            //~ "Не удалось загрузить файл перечисления",
            //~ &collectEnum
        //~ );
    //~ } catch (const runtime_error& e) {
        //~ cerr << e.what() << endl;
        //~ return 1;
    //~ }

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

    conf->exportToFiles(outputPath);
    conf->generateConfigVersions(configVersions);
    versionsDoc.save_file((outputPath / "ConfigDumpInfo.xml").c_str());
}