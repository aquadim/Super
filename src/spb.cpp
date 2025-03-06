#include <iostream>
#include <filesystem>
#include <argparse/argparse.hpp>
#include <pugixml.hpp>
#include "ids.hpp"
#include "xmltools.hpp"
#include <vector>
#include <stdexcept>
#include "objects.hpp"

namespace fs = std::filesystem;

// Добавляет ContainedObject в <InternalInfo> конфигурации
//~ void addContainedObject(pugi::xml_node config) {
    //~ pugi::xml_node containedObject = config.append_child("xr:ContainedObject");
    //~ containedObject.append_child("xr:ClassId").text().set(uuid);
    //~ containedObject.append_child("xr:ObjectId").text().set(ids::getUUID());
//~ }

// Обработка языка
objects::Language collectLanguage(pugi::xml_node config) {
    std::string name    = config.child("id").text().get();
    lstring synonym     = xmltools::parseLocalisedString(config.child("synonym"));
    std::string comment = config.child("comment").text().get();
    std::string code    = config.child("code").text().get();

    objects::Language output(name, synonym, comment, code);
    return output;
}

// Обработка справочника
objects::Catalog collectCatalog(pugi::xml_node config) {
    std::string name    = config.child("id").text().get();
    lstring synonym     = xmltools::parseLocalisedString(config.child("synonym").child("localised-string"));
    std::string comment = config.child("comment").text().get();

    // Реквизиты
    objects::PropertyList properties;
    for (
        pugi::xml_node property = config.child("properties").child("property");
        property;
        property = property.next_sibling("property")
    )
    {
        std::string propName    = property.child("id").text().get();
        lstring propSynonym     = xmltools::parseLocalisedString(property.child("synonym").child("localised-string"));
        std::string comment     = property.child("comment").text().get();
        typing::Type* type      = xmltools::parseTypeNode(property.child("type"));

        auto p = objects::Property(propName, propSynonym, comment, *type);
        properties.add(p);
        delete type;
    }
    //~ // Табличные части
    //~ for (
        //~ pugi::xml_node tabularSection = config.child("tabular-sections").child("tabular-section");
        //~ tabularSection;
        //~ tabularSection = tabularSection.next_sibling("tabular-section")
    //~ )
    //~ {
        //~ // Имя табличной части
        //~ std::string tabularId = tabularSection.attribute("id").as_string();

        //~ // Колонки табличной части
        //~ pugi::xml_node tabularColumns = tabularSection.child("columns");
        
        //~ // Узел табличной части
        //~ pugi::xml_node tabularSectionNode = childObjectsNode.append_child("TabularSection");
        //~ tabularSectionNode.append_attribute("uuid").set_value(ids::getUUID());

        //~ // InternalInfo
        //~ pugi::xml_node tabularInternalInfoNode = tabularSectionNode.append_child("InternalInfo");
        //~ xmltools::addGeneratedType(
            //~ tabularInternalInfoNode,
            //~ "CatalogTabularSection."+objectId+"."+tabularId,
            //~ "TabularSection"
        //~ );
        //~ xmltools::addGeneratedType(
            //~ tabularInternalInfoNode,
            //~ "CatalogTabularSectionRow."+objectId+"."+tabularId,
            //~ "TabularSectionRow"
        //~ );

        //~ // Свойства табличной части
        //~ pugi::xml_node tabularPropsNode = tabularSectionNode.append_child("Properties");
        //~ // Имя
        //~ xmltools::addNameNode(tabularPropsNode, tabularId);
        //~ // Синоним
        //~ xmltools::addLocalisedString(
            //~ tabularPropsNode.append_child("Synonym"),
            //~ xmltools::parseLocalisedString(tabularSection.child("synonym").child("localised-string"))
        //~ );
        //~ // Комментарий
        //~ xmltools::addCommentNode(
            //~ tabularPropsNode,
            //~ tabularSection.child("comment").text().get()
        //~ );

        //~ // Колонки табличной части
        //~ pugi::xml_node tabularColsNode = tabularSectionNode.append_child("ChildObjects");
        //~ for (
            //~ pugi::xml_node tabularColumn = tabularColumns.child("column");
            //~ tabularColumn;
            //~ tabularColumn = tabularColumn.next_sibling("column")
        //~ )
        //~ {
            //~ // Название колонки
            //~ std::string tabularColumnId = tabularColumn.attribute("id").as_string();
            //~ // Синоним колонки
            //~ pugi::xml_node tabularColumnSynonym = tabularColumn.child("synonym");
            //~ // Комментарий колонки
            //~ pugi::xml_node tabularColumnComment = tabularColumn.child("comment");
            //~ // Тип колонки
            //~ typing::Type* tabularColumnType = xmltools::parseTypeNode(tabularColumn.child("type"));

            //~ // Создание XML узла колонки
            //~ pugi::xml_node tabularColNode       = tabularColsNode.append_child("Attribute");
            //~ pugi::xml_node tabularColPropsNode  = tabularColNode.append_child("Properties");
            //~ tabularColNode.append_attribute("uuid").set_value(ids::getUUID());
            //~ xmltools::addNameNode(tabularColPropsNode, tabularColumnId);
            //~ xmltools::addSynonymNode(tabularColPropsNode, tabularColumnSynonym);
            //~ xmltools::addCommentNode(tabularColPropsNode, tabularColumnComment.text().get());
            //~ tabularColumnType->addTypeNode(tabularColPropsNode);

            //~ delete tabularColumnType;
        //~ }
    //~ }

    //~ catalogRootNode.save_file((dumpRootPath / "Catalogs" / (objectId + ".xml")).c_str());
    objects::Catalog output(name, synonym, comment, properties);
    return output;
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
        std::cout << "Обработка: " << objectConfigPath << std::endl;
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




    

    //~ // ConfigDumpInfo.xml
    //~ pugi::xml_document configDumpInfoXML;
    //~ pugi::xml_node configDumpInfo       = configDumpInfoXML.append_child("ConfigDumpInfo");
    //~ configDumpInfo.append_attribute("xmlns").set_value("http://v8.1c.ru/8.3/xcf/dumpinfo");
    //~ configDumpInfo.append_attribute("xmlns:xen").set_value("http://v8.1c.ru/8.3/xcf/enums");
    //~ configDumpInfo.append_attribute("xmlns:xs").set_value("http://www.w3.org/2001/XMLSchema");
    //~ configDumpInfo.append_attribute("xmlns:xsi").set_value("http://www.w3.org/2001/XMLSchema-instance");
    //~ configDumpInfo.append_attribute("format").set_value("Hierarchical");
    //~ configDumpInfo.append_attribute("version").set_value("2.18");
    //~ pugi::xml_node configVersionsNode   = configDumpInfo.append_child("ConfigVersions");

    //~ // Configuration.xml
    //~ pugi::xml_document configurationXML;
    //~ pugi::xml_node configurationMetaNode    = configurationXML.append_child("MetaDataObject");
    //~ pugi::xml_node configurationNode        = configurationMetaNode.append_child("Configuration");
    
    //~ // Configuration -- InternalInfo
    //~ pugi::xml_node configurationInternalInfoNode = configurationNode.append_child("InternalInfo");
    //~ addContainedObject(configurationInternalInfoNode, "9cd510cd-abfc-11d4-9434-004095e12fc7");
    //~ addContainedObject(configurationInternalInfoNode, "9fcd25a0-4822-11d4-9414-008048da11f9");
    //~ addContainedObject(configurationInternalInfoNode, "e3687481-0a87-462c-a166-9f34594f9bba");
    //~ addContainedObject(configurationInternalInfoNode, "9de14907-ec23-4a07-96f0-85521cb6b53b");
    //~ addContainedObject(configurationInternalInfoNode, "51f2d5d8-ea4d-4064-8892-82951750031e");
    //~ addContainedObject(configurationInternalInfoNode, "e68182ea-4237-4383-967f-90c1e3370bc7");
    //~ addContainedObject(configurationInternalInfoNode, "fb282519-d103-4dd3-bc12-cb271d631dfc");

    //~ pugi::xml_node configurationPropertiesNode  = configurationNode.append_child("Properties");
    //~ pugi::xml_node configurationChildrenNode    = configurationNode.append_child("ChildObjects");

    //~ xmltools::addNamespaces(configurationMetaNode);

    //~ configurationNode.append_attribute("uuid").set_value(ids::getUUID().c_str());

    //~ bool success;
    
    //~ // Обработка языков
    //~ fs::create_directory(outputPath / "Languages");
    //~ success = handleType(
        //~ projectNode.child("languages"),
        //~ projectPath,
        //~ outputPath,
        //~ "Languages",
        //~ "language-definition",
        //~ "Не удалось прочитать файл языка",
        //~ configVersionsNode,
        //~ configurationChildrenNode,
        //~ "Language",
        //~ &handleLanguage);
    //~ if (!success) {
        //~ return 1;
    //~ }
    
    //~ // Обработка справочников
    //~ fs::create_directory(outputPath / "Catalogs");
    //~ success = handleType(
        //~ projectNode.child("catalogs"),
        //~ projectPath,
        //~ outputPath,
        //~ "Catalogs",
        //~ "catalog",
        //~ "Не удалось прочитать файл справочника",
        //~ configVersionsNode,
        //~ configurationChildrenNode,
        //~ "Catalog",
        //~ &handleCatalog);
    //~ if (!success) {
        //~ return 1;
    //~ }
    
    //~ // Название конфигурации
    //~ std::string projectName = projectNode.child("name").text().get();
    //~ xmltools::addConfigVersion(configVersionsNode, projectName, "Configuration");
    //~ xmltools::addNameNode(configurationPropertiesNode, projectName);

    //~ // Запись ConfigDumpInfo, Configuration
    //~ configDumpInfoXML.save_file((outputPath / "ConfigDumpInfo.xml").c_str());
    //~ configurationXML.save_file((outputPath / "Configuration.xml").c_str());
}