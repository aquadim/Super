#ifndef XMLTOOLS_H
#define XMLTOOLS_H

// Файл для генерации разного рода идентификаторов
#include <pugixml.hpp>
#include <string>
#include <unordered_map>
#include "typing.hpp"
#include <memory>

using namespace std;

namespace xmltools {

    // Добавляет под-узел
    void addSubNode(pugi::xml_node parent, string name, string value);
    
    // Добавляет <Name> в узел XML
    void addNameNode(pugi::xml_node parent, string value);

    // Добавляет <Synonym> в узел XML
    // node - узел в который добавлять данные
    // synonym - узел XML из настроек SUPER
    void addSynonymNode(pugi::xml_node parent, pugi::xml_node synonym);
    
    // Добавляет <Comment> в узел XML
    void addCommentNode(pugi::xml_node parent, string value);

    // Добавляет в узел версий запись об объекте
    pugi::xml_node addConfigVersion(
        pugi::xml_node parent,
        string name,
        string version
    );

    // Добавляет в узел детских объектов описание объекта
    // childrenNode - узел <ChildObjects>
    // objectName - имя объекта
    // objectType - тип объекта (Catalog, Enum, Language...)
    void addChildObject(
        pugi::xml_node childrenNode,
        string objectName,
        string objectType);

    // Добавляет пространства имёт в объект
    void addNamespaces(pugi::xml_node node);

    // Парсит <localised-string> из проекта super в map
    unordered_map<string, string> parseLocalisedString(pugi::xml_node node);

    // Добавляет в файл выгрузки объекта локализованную строку
    void addLocalisedString(
        pugi::xml_node node,
        unordered_map<string, string> langMap
    );

    // Добавляет узел GeneratedType для родителя "InternalInfo"
    void addGeneratedType(
        pugi::xml_node node,
        string name,
        string category
    );

    // Парсит тип из узла SUPER
    shared_ptr<typing::Type> parseTypeNode(pugi::xml_node node);
}

#endif