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
        code,
        conf
    ));
}

//~ // Обработка справочника
//~ objects::Catalog collectCatalog(pugi::xml_node config) {
    //~ string name     = config.child("id").text().get();
    //~ lstring synonym = xmltools::parseLocalisedString(config.child("synonym"));
    //~ string comment  = config.child("comment").text().get();

    //~ // Реквизиты
    //~ objects::PropertyList properties{};
    //~ for (
        //~ pugi::xml_node property = config.child("properties").child("property");
        //~ property;
        //~ property = property.next_sibling("property")
    //~ )
    //~ {
        //~ string propName                 = property.child("id").text().get();
        //~ lstring propSynonym             = xmltools::parseLocalisedString(property.child("synonym"));
        //~ string comment                  = property.child("comment").text().get();
        //~ shared_ptr<typing::Type> type   = xmltools::parseTypeNode(property.child("type"));

        //~ properties.add(propName, propSynonym, comment, type);
    //~ }
    
    //~ // Табличные части
    //~ objects::TabularsList tabulars{};
    //~ for (
        //~ pugi::xml_node tabularSection = config.child("tabular-sections").child("tabular-section");
        //~ tabularSection;
        //~ tabularSection = tabularSection.next_sibling("tabular-section")
    //~ )
    //~ {
        //~ string tabularName     = tabularSection.child("id").text().get();
        //~ lstring tabularSynonym = xmltools::parseLocalisedString(tabularSection.child("synonym"));
        //~ string tabularComment  = tabularSection.child("comment").text().get();

        //~ // Сбор колонок табличной части
        //~ vector<shared_ptr<objects::TabularColumn>> tabularColumns{};
        //~ for (
            //~ pugi::xml_node tabularColumn = tabularSection.child("columns").child("column");
            //~ tabularColumn;
            //~ tabularColumn = tabularColumn.next_sibling("column")
        //~ )
        //~ {
            //~ string tabularColName = tabularColumn.child("id").text().get();
            //~ lstring tabularColSynonym = xmltools::parseLocalisedString(
                //~ tabularColumn.child("synonym")
            //~ );
            //~ string tabularColComment = tabularColumn.child("comment").text().get();
            //~ shared_ptr<typing::Type> tabularColType = xmltools::parseTypeNode(
                //~ tabularColumn.child("type")
            //~ );

            //~ tabularColumns.push_back(make_shared<objects::TabularColumn>(
                //~ tabularColName,
                //~ tabularColSynonym,
                //~ tabularColComment,
                //~ tabularColType
            //~ ));
        //~ }

        //~ // Добавление табличной части
        //~ tabulars.add(objects::TabularSection{
            //~ tabularName,
            //~ tabularSynonym,
            //~ tabularComment,
            //~ tabularColumns
        //~ });
    //~ }

    //~ return objects::Catalog{name, synonym, comment, properties, tabulars};
//~ }

//~ // Обработка документа
//~ objects::Document collectDocument(pugi::xml_node config) {
    //~ string name     = config.child("id").text().get();
    //~ lstring synonym = xmltools::parseLocalisedString(config.child("synonym"));
    //~ string comment  = config.child("comment").text().get();

    //~ // Реквизиты
    //~ objects::PropertyList properties{};
    //~ for (
        //~ pugi::xml_node property = config.child("properties").child("property");
        //~ property;
        //~ property = property.next_sibling("property")
    //~ )
    //~ {
        //~ string propName                 = property.child("id").text().get();
        //~ lstring propSynonym             = xmltools::parseLocalisedString(property.child("synonym"));
        //~ string comment                  = property.child("comment").text().get();
        //~ shared_ptr<typing::Type> type   = xmltools::parseTypeNode(property.child("type"));

        //~ properties.add(propName, propSynonym, comment, type);
    //~ }
    
    //~ // Табличные части
    //~ objects::TabularsList tabulars{};
    //~ for (
        //~ pugi::xml_node tabularSection = config.child("tabular-sections").child("tabular-section");
        //~ tabularSection;
        //~ tabularSection = tabularSection.next_sibling("tabular-section")
    //~ )
    //~ {
        //~ string tabularName      = tabularSection.child("id").text().get();
        //~ lstring tabularSynonym  = xmltools::parseLocalisedString(tabularSection.child("synonym"));
        //~ string tabularComment   = tabularSection.child("comment").text().get();

        //~ // Сбор колонок табличной части
        //~ vector<shared_ptr<objects::TabularColumn>> tabularColumns{};
        //~ for (
            //~ pugi::xml_node tabularColumn = tabularSection.child("columns").child("column");
            //~ tabularColumn;
            //~ tabularColumn = tabularColumn.next_sibling("column")
        //~ )
        //~ {
            //~ string tabularColName = tabularColumn.child("id").text().get();
            //~ lstring tabularColSynonym = xmltools::parseLocalisedString(
                //~ tabularColumn.child("synonym")
            //~ );
            //~ string tabularColComment = tabularColumn.child("comment").text().get();
            //~ shared_ptr<typing::Type> tabularColType = xmltools::parseTypeNode(
                //~ tabularColumn.child("type")
            //~ );

            //~ tabularColumns.push_back(make_shared<objects::TabularColumn>(
                //~ tabularColName,
                //~ tabularColSynonym,
                //~ tabularColComment,
                //~ tabularColType
            //~ ));
        //~ }

        //~ // Добавление табличной части
        //~ tabulars.add(objects::TabularSection{
            //~ tabularName,
            //~ tabularSynonym,
            //~ tabularComment,
            //~ tabularColumns
        //~ });
    //~ }

    //~ return objects::Document{name, synonym, comment, properties, tabulars};
//~ }

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

    //~ // Парсинг справочников проекта
    //~ vector<objects::Catalog> catalogs;
    //~ try {
        //~ catalogs = collectTypes(
            //~ project.child("catalogs"),
            //~ projectPath,
            //~ "Catalogs",
            //~ "catalog",
            //~ "Не удалось загрузить файл справочника",
            //~ &collectCatalog
        //~ );
    //~ } catch (const runtime_error& e) {
        //~ cerr << e.what() << endl;
        //~ return 1;
    //~ }

    //~ // Парсинг документов проекта
    //~ vector<objects::Document> documents;
    //~ try {
        //~ documents = collectTypes(
            //~ project.child("documents"),
            //~ projectPath,
            //~ "Documents",
            //~ "document",
            //~ "Не удалось загрузить файл документа",
            //~ &collectDocument
        //~ );
    //~ } catch (const runtime_error& e) {
        //~ cerr << e.what() << endl;
        //~ return 1;
    //~ }
    
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