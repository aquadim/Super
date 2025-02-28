#ifndef XMLTOOLS_H
#define XMLTOOLS_H

// Файл для генерации разного рода идентификаторов
#include <pugixml.hpp>
#include <string>
#include <unordered_map>

namespace xmltools {    
    // Добавляет <Name> в узел XML
    void addNameNode(pugi::xml_node node, std::string value);

    // Добавляет в узел версий запись об объекте
    // configVersions - узел версий
    // objectName - имя объекта
    // objectPrefix - префикс объекта (Catalog, Enum, Language...)
    void addConfigVersion(
        pugi::xml_node configVersions,
        std::string objectName,
        std::string objectPrefix);

    // Добавляет в узел детских объектов описание объекта
    // childrenNode - узел <ChildObjects>
    // objectName - имя объекта
    // objectType - тип объекта (Catalog, Enum, Language...)
    void addChildObject(
        pugi::xml_node childrenNode,
        std::string objectName,
        std::string objectType);

    // Добавляет пространства имёт в объект
    void addNamespaces(pugi::xml_node node);

    // Парсит <localised-string> из проекта super в map
    std::unordered_map<std::string, std::string> parseLocalisedString(pugi::xml_node node);

    // Добавляет в файл выгрузки объекта локализованную строку
    void addLocalisedString(
        pugi::xml_node node,
        std::unordered_map<std::string, std::string> langMap
    );
}

#endif