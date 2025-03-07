#ifndef OBJECTS_H
#define OBJECTS_H

// Классы бизнес-объектов конфигурации
#include <unordered_map>
#include <vector>
#include "typing.hpp"
#include <pugixml.hpp>
#include <filesystem>

// Словарь, где ключ - код языка, а значение - строка на этом языке
using lstring = std::unordered_map<std::string, std::string>;

namespace fs = std::filesystem;

namespace objects {

    class Property;
    class TabularSection;

    // Список реквизитов
    class PropertyList {
        public:
        PropertyList(std::vector<Property> properties);
        PropertyList();
        // Добавляет реквизит
        void add(Property p);
        // Добавляет реквизиты в узел parent
        void addNodesForAll(pugi::xml_node parent);
        // Список реквизитов
        std::vector<Property> mProperties;
    };
    
    // Список табличных частей
    class TabularsList {
        public:
        TabularsList(std::vector<TabularSection> sections);
        TabularsList();
        // Добавляет табличную часть
        void add(TabularSection ts);
        // Добавляет табличные части в узел parent
        // prefix - Catalog/Document -- префикс для узлов в InternalInfo
        // ownerName - имя узла-владельца
        void addNodesForAll(
            pugi::xml_node parent,
            std::string prefix,
            std::string ownerName
        );
        // Список табличных частей
        std::vector<TabularSection> mTabulars;
    };

    // Узел конфигурации. Может хранить имя, синоним, комментарий
    class ObjectNode {
        public:
        ObjectNode(
            std::string name,
            lstring synonym,
            std::string comment
        );
        virtual ~ObjectNode() {};

        // Экспортирует объект в файл
        // Некоторые объекты (реквизиты, табличные части, ...) могут не
        // реализовывать этот метод
        // exportRoot - путь к каталогу выгрузки
        virtual void exportToFiles(fs::path exportRoot) = 0;

        // Добавляет и возвращает узел к MetaDataObject
        virtual pugi::xml_node makeNode(pugi::xml_node md) = 0;

        std::string getName();
        
        protected:

        // Создаёт файл объекта в выгрузке
        pugi::xml_document createDocument();
        
        // Имя объекта
        std::string mName;
        // Синоним
        lstring mSynonym;
        // Комментарий
        std::string mComment;
    };

    // Реквизит узла конфигурации
    class Property : public ObjectNode {
        public:
        Property(
            std::string name,
            lstring synonym,
            std::string comment,
            typing::Type* type
        );
        virtual ~Property() {};
        void exportToFiles(fs::path exportRoot) override { (void)exportRoot; }
        pugi::xml_node makeNode(pugi::xml_node md) override;

        protected:
        // Тип свойства
        typing::Type mType;
    };
    
    // Колонка табличной части
    class TabularColumn : public ObjectNode {
        public:
        TabularColumn(
            std::string name,
            lstring synonym,
            std::string comment,
            typing::Type* type
        );
        void exportToFiles(fs::path exportRoot) override { (void)exportRoot; }
        pugi::xml_node makeNode(pugi::xml_node md) override;

        protected:
        // Тип свойства
        typing::Type mType;
    };

    // Табличная часть
    class TabularSection : public ObjectNode {
        public:
        TabularSection(
            std::string name,
            lstring synonym,
            std::string comment,
            std::vector<TabularColumn> columns
        );
        void exportToFiles(fs::path exportRoot) override { (void)exportRoot; }
        pugi::xml_node makeNode(pugi::xml_node md) override;

        protected:
        std::vector<TabularColumn> mColumns;
    };

    // Язык
    class Language : public ObjectNode {
        public:
        Language(
            std::string name,
            lstring synonym,
            std::string comment,
            std::string code
        );
        void exportToFiles(fs::path exportRoot) override;
        pugi::xml_node makeNode(pugi::xml_node md) override;

        protected:
        std::string mCode;
    };
    
    // Справочник
    class Catalog : public ObjectNode {
        public:
        Catalog(
            std::string name,
            lstring synonym,
            std::string comment,
            PropertyList properties,
            TabularsList tabulars
        );
        void exportToFiles(fs::path exportRoot) override;
        pugi::xml_node makeNode(pugi::xml_node md) override;

        protected:
        // Список реквизитов
        PropertyList mProperties;
        // Список табличных частей
        TabularsList mTabulars;
    };

    // Конфигурация -- корневой узел
    class Configuration : public ObjectNode {
        public:
        Configuration(
            std::string name,
            lstring synonym,
            std::string comment,
            std::vector<Language> languages,
            std::vector<Catalog> catalogs
        );
        void exportToFiles(fs::path exportRoot) override;
        pugi::xml_node makeNode(pugi::xml_node md) override;

        protected:
        // Список языков
        std::vector<Language> mLanguages;
        // Список справочников
        std::vector<Catalog> mCatalogs;
    };
    
}

#endif