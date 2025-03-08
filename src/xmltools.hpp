#ifndef XMLTOOLS_H
#define XMLTOOLS_H

// Файл для генерации разного рода идентификаторов
#include <pugixml.hpp>
#include <string>
#include <unordered_map>
#include "typing.hpp"
#include <memory>

namespace xmltools {

    // Добавляет под-узел
    void addSubNode(pugi::xml_node parent, std::string name, std::string value);
    
    // Добавляет <Name> в узел XML
    void addNameNode(pugi::xml_node parent, std::string value);

    // Добавляет <Synonym> в узел XML
    // node - узел в который добавлять данные
    // synonym - узел XML из настроек SUPER
    void addSynonymNode(pugi::xml_node parent, pugi::xml_node synonym);
    
    // Добавляет <Comment> в узел XML
    void addCommentNode(pugi::xml_node parent, std::string value);

    // Добавляет в узел версий запись об объекте
    // parent - родитель
    // objectName - имя объекта
    // objectPrefix - префикс объекта (Catalog, Enum, Language...)
    pugi::xml_node addConfigVersion(
        pugi::xml_node parent,
        std::string name);

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

    // Добавляет узел GeneratedType для родителя "InternalInfo"
    void addGeneratedType(
        pugi::xml_node node,
        std::string name,
        std::string category
    );

    // Парсит тип из узла SUPER
    typing::Type* parseTypeNode(pugi::xml_node node);

    // Добавляет узел <Type>
    // parent - узел в который нужно добавить <Type>
    // type - указатель на тип
    void addTypeNode(pugi::xml_node parent, std::shared_ptr<typing::Type> type);
}

#endif