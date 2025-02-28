#include <iostream>
#include <filesystem>
#include <argparse/argparse.hpp>
#include <pugixml.hpp>
#include "ids.hpp"
#include "xmltools.hpp"

namespace fs = std::filesystem;

// Обработка языка
void handleLanguage(fs::path dumpRootPath, pugi::xml_node config, std::string objectId) {
    pugi::xml_document language;
    pugi::xml_node metadataNode = language.append_child("MetaDataObject");
    xmltools::addNamespaces(metadataNode);

    pugi::xml_node languageNode = metadataNode.append_child("Language");
    languageNode.append_attribute("uuid").set_value(ids::getUUID());

    pugi::xml_node propsNode = languageNode.append_child("Properties");

    // Name
    propsNode.append_child("Name").text().set(objectId);

    // Synonym
    xmltools::addLocalisedString(
        propsNode.append_child("Synonym"),
        xmltools::parseLocalisedString(config.child("synonym").child("localised-string"))
    );
    
    // Comment
    xmltools::addLocalisedString(
        propsNode.append_child("Comment"),
        xmltools::parseLocalisedString(config.child("comment").child("localised-string"))
    );

    // LanguageCode
    propsNode.append_child("LanguageCode").text().set(
        config.child("code").text().get()
    );

    language.save_file((dumpRootPath / "Languages" / (objectId + ".xml")).c_str());
}

// Обработка справочника
void handleCatalog(fs::path dumpRootPath, pugi::xml_node config, std::string objectId) {
    pugi::xml_document catalog;
    pugi::xml_node metadataNode = catalog.append_child("MetaDataObject");
    xmltools::addNamespaces(metadataNode);

    pugi::xml_node catalogNode = metadataNode.append_child("Catalog");
    catalogNode.append_attribute("uuid").set_value(ids::getUUID());

    pugi::xml_node propsNode = catalogNode.append_child("Properties");

    // Name
    propsNode.append_child("Name").text().set(objectId);

    // Synonym
    xmltools::addLocalisedString(
        propsNode.append_child("Synonym"),
        xmltools::parseLocalisedString(config.child("synonym").child("localised-string"))
    );
    
    // Comment
    xmltools::addLocalisedString(
        propsNode.append_child("Comment"),
        xmltools::parseLocalisedString(config.child("comment").child("localised-string"))
    );

    catalog.save_file((dumpRootPath / "Catalogs" / (objectId + ".xml")).c_str());
}

// Обрабатывает все объекты какого либо типа
// includes - xml узел с <include>
// projectPath - путь к проекту super
// dumpRootPath - путь к выгрузке
// typeDirectory - папка в которую поместить информацию
// rootTagName - имя xml узла из файла настроек
// errorMessage - сообщение об ошибке чтения файла настроек
// configVersions - xml узел версий объектов
// configChildren - xml узел объектов конфигурации
// objectType - тип объекта (для создания xml узлов) (Catalog, Document, Language ...)
// handler - функция обработки определённого типа объектов
bool handleType(
    pugi::xml_node includes,
    fs::path projectPath,
    fs::path dumpRootPath,
    std::string typeDirectory,
    std::string rootTagName,
    std::string errorMessage,
    pugi::xml_node configVersions,
    pugi::xml_node configChildren,
    std::string objectType,
    void(*handler)(fs::path, pugi::xml_node, std::string))
{
    for (pugi::xml_node include = includes.child("include"); include; include = include.next_sibling("include")) {
        // Путь к настройкам объекта
        fs::path objectConfigPath = projectPath / typeDirectory / include.text().get();

        // Получить и прочитать настройки объекта
        pugi::xml_document objectConfig;
        if (!objectConfig.load_file(objectConfigPath.c_str())) {
            std::cerr << errorMessage << objectConfigPath << std::endl;
            return false;
        }
        pugi::xml_node object = objectConfig.child(rootTagName);

        std::string objectId = object.child("id").text().get();
        handler(dumpRootPath, object, objectId);

        xmltools::addConfigVersion(configVersions, objectId, objectType);
        xmltools::addChildObject(configChildren, objectId, objectType);
    }
    return true;
}

int main(int argc, char* argv[]) {
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
    std::string outputPathStr = outputPath.string();

    // Загрузка настроек XML проекта
    pugi::xml_document projectInfo;
    if (!projectInfo.load_file(
        (projectPath / "project.xml").c_str()
    )) {
        std::cerr << "Не удалось прочитать файл проекта" << std::endl;
        return 1;
    }
    pugi::xml_node projectNode = projectInfo.child("project");

    // ConfigDumpInfo.xml
    pugi::xml_document configDumpInfoXML;
    pugi::xml_node configDumpInfo       = configDumpInfoXML.append_child("ConfigDumpInfo");
    pugi::xml_node configVersionsNode   = configDumpInfo.append_child("ConfigVersions");

    // Configuration.xml
    pugi::xml_document configurationXML;
    pugi::xml_node configurationMetaNode        = configurationXML.append_child("MetaDataObject");
    pugi::xml_node configurationNode            = configurationMetaNode.append_child("Configuration");
    pugi::xml_node configurationPropertiesNode  = configurationNode.append_child("Properties");
    pugi::xml_node configurationChildrenNode    = configurationNode.append_child("ChildObjects");

    xmltools::addNamespaces(configurationMetaNode);

    configurationNode.append_attribute("uuid").set_value(ids::getUUID().c_str());

    bool success;
    
    // Обработка языков
    fs::create_directory(outputPath / "Languages");
    success = handleType(
        projectNode.child("languages"),
        projectPath,
        outputPath,
        "Languages",
        "language-definition",
        "Не удалось прочитать файл языка",
        configVersionsNode,
        configurationChildrenNode,
        "Language",
        &handleLanguage);
    if (!success) {
        return 1;
    }
    
    // Обработка справочников
    fs::create_directory(outputPath / "Catalogs");
    success = handleType(
        projectNode.child("catalogs"),
        projectPath,
        outputPath,
        "Catalogs",
        "catalog",
        "Не удалось прочитать файл справочника",
        configVersionsNode,
        configurationChildrenNode,
        "Catalog",
        &handleCatalog);
    if (!success) {
        return 1;
    }
    
    // Название конфигурации
    std::string projectName = projectNode.child("name").text().get();
    xmltools::addConfigVersion(configVersionsNode, projectName, "Configuration");
    xmltools::addNameNode(configurationPropertiesNode, projectName);

    // Запись ConfigDumpInfo, Configuration
    configDumpInfoXML.save_file((outputPath / "ConfigDumpInfo.xml").c_str());
    configurationXML.save_file((outputPath / "Configuration.xml").c_str());
}