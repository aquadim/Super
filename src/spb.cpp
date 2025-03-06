#include <iostream>
#include <filesystem>
#include <argparse/argparse.hpp>
#include <pugixml.hpp>
#include "ids.hpp"
#include "xmltools.hpp"

namespace fs = std::filesystem;

// Добавляет ContainedObject в <InternalInfo> конфигурации
void addContainedObject(pugi::xml_node node, std::string uuid) {
    pugi::xml_node containedObject = node.append_child("xr:ContainedObject");
    containedObject.append_child("xr:ClassId").text().set(uuid);
    containedObject.append_child("xr:ObjectId").text().set(ids::getUUID());
}

// Обработка языка
void handleLanguage(fs::path dumpRootPath, pugi::xml_node config, std::string objectId) {
    pugi::xml_document language;
    pugi::xml_node metadataNode = language.append_child("MetaDataObject");
    xmltools::addNamespaces(metadataNode);

    pugi::xml_node languageNode = metadataNode.append_child("Language");
    languageNode.append_attribute("uuid").set_value(ids::getUUID());

    pugi::xml_node propsNode = languageNode.append_child("Properties");

    // -- Properties --
    // Name
    xmltools::addNameNode(propsNode, objectId);
    // Synonym
    xmltools::addLocalisedString(
        propsNode.append_child("Synonym"),
        xmltools::parseLocalisedString(config.child("synonym").child("localised-string"))
    );
    // Comment
    xmltools::addCommentNode(propsNode, config.child("comment").text().get());
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

    // -- InternalInfo --
    pugi::xml_node internalInfoNode = catalogNode.append_child("InternalInfo");
    xmltools::addGeneratedType(internalInfoNode,"CatalogObject."+objectId,"Object");
    xmltools::addGeneratedType(internalInfoNode,"CatalogRef."+objectId,"Ref");
    xmltools::addGeneratedType(internalInfoNode,"CatalogSelection."+objectId,"Selection");
    xmltools::addGeneratedType(internalInfoNode,"CatalogList."+objectId,"List");
    xmltools::addGeneratedType(internalInfoNode,"CatalogManager."+objectId,"Manager");
    
    // -- Properties --
    pugi::xml_node propsNode = catalogNode.append_child("Properties");
    // Name
    xmltools::addNameNode(propsNode, objectId);
    // Synonym
    xmltools::addLocalisedString(
        propsNode.append_child("Synonym"),
        xmltools::parseLocalisedString(config.child("synonym").child("localised-string"))
    );
    // Comment
    xmltools::addCommentNode(propsNode, config.child("comment").text().get());

    // <ChildObjects>
    pugi::xml_node childObjectsNode = catalogNode.append_child("ChildObjects");
    // Свойства
    for (
        pugi::xml_node propertyNode = config.child("properties").child("property");
        propertyNode;
        propertyNode = propertyNode.next_sibling("property")
    )
    {
        // <Attribute>
        pugi::xml_node attributeNode = childObjectsNode.append_child("Attribute");
        attributeNode.append_attribute("uuid").set_value(ids::getUUID());

        // <Properties>
        pugi::xml_node attributePropsNode = attributeNode.append_child("Properties");
        // Название свойства
        xmltools::addNameNode(
            attributePropsNode,
            propertyNode.attribute("id").as_string()
        );
        // Синоним
        xmltools::addLocalisedString(
            attributePropsNode.append_child("Synonym"),
            xmltools::parseLocalisedString(propertyNode.child("synonym").child("localised-string"))
        );
        // Комментарий
        xmltools::addCommentNode(
            attributePropsNode,
            propertyNode.child("comment").text().get()
        );
        // Тип
        typing::Type* type = xmltools::parseTypeNode(propertyNode.child("type"));
        type->addTypeNode(attributePropsNode);
        delete type;
    }
    // Табличные части
    for (
        pugi::xml_node tabularSection = config.child("tabular-sections").child("tabular-section");
        tabularSection;
        tabularSection = tabularSection.next_sibling("tabular-section")
    )
    {
        // Имя табличной части
        std::string tabularId = tabularSection.attribute("id").as_string();
        
        // <TabularSection>
        pugi::xml_node tabularSectionNode = childObjectsNode.append_child("TabularSection");
        tabularSectionNode.append_attribute("uuid").set_value(ids::getUUID());

        // <InternalInfo>
        pugi::xml_node tabularInternalInfoNode = tabularSectionNode.append_child("InternalInfo");
        xmltools::addGeneratedType(
            tabularInternalInfoNode,
            "CatalogTabularSection."+objectId+"."+tabularId,
            "TabularSection"
        );
        xmltools::addGeneratedType(
            tabularInternalInfoNode,
            "CatalogTabularSectionRow."+objectId+"."+tabularId,
            "TabularSectionRow"
        );

        // <Properties>
        pugi::xml_node tabularPropsNode = tabularSectionNode.append_child("Properties");
        xmltools::addNameNode(tabularPropsNode, tabularId);
        xmltools::addLocalisedString(
            tabularPropsNode.append_child("Synonym"),
            xmltools::parseLocalisedString(tabularSection.child("synonym").child("localised-string"))
        );
        xmltools::addCommentNode(
            tabularPropsNode,
            tabularSection.child("comment").text().get()
        );
    }

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
    configDumpInfo.append_attribute("xmlns").set_value("http://v8.1c.ru/8.3/xcf/dumpinfo");
    configDumpInfo.append_attribute("xmlns:xen").set_value("http://v8.1c.ru/8.3/xcf/enums");
    configDumpInfo.append_attribute("xmlns:xs").set_value("http://www.w3.org/2001/XMLSchema");
    configDumpInfo.append_attribute("xmlns:xsi").set_value("http://www.w3.org/2001/XMLSchema-instance");
    configDumpInfo.append_attribute("format").set_value("Hierarchical");
    configDumpInfo.append_attribute("version").set_value("2.18");
    pugi::xml_node configVersionsNode   = configDumpInfo.append_child("ConfigVersions");

    // Configuration.xml
    pugi::xml_document configurationXML;
    pugi::xml_node configurationMetaNode    = configurationXML.append_child("MetaDataObject");
    pugi::xml_node configurationNode        = configurationMetaNode.append_child("Configuration");
    
    // Configuration -- InternalInfo
    pugi::xml_node configurationInternalInfoNode = configurationNode.append_child("InternalInfo");
    addContainedObject(configurationInternalInfoNode, "9cd510cd-abfc-11d4-9434-004095e12fc7");
    addContainedObject(configurationInternalInfoNode, "9fcd25a0-4822-11d4-9414-008048da11f9");
    addContainedObject(configurationInternalInfoNode, "e3687481-0a87-462c-a166-9f34594f9bba");
    addContainedObject(configurationInternalInfoNode, "9de14907-ec23-4a07-96f0-85521cb6b53b");
    addContainedObject(configurationInternalInfoNode, "51f2d5d8-ea4d-4064-8892-82951750031e");
    addContainedObject(configurationInternalInfoNode, "e68182ea-4237-4383-967f-90c1e3370bc7");
    addContainedObject(configurationInternalInfoNode, "fb282519-d103-4dd3-bc12-cb271d631dfc");

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