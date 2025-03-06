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

    class Catalog;
    class Language;

    // Узел конфигурации. Может хранить имя, синоним, комментарий
    class ObjectNode {
        public:
        ObjectNode(
            std::string name,
            lstring synonym,
            std::string comment
        );
        virtual ~ObjectNode();
        std::string getName();
        // Экспортирует объект в файл
        // Некоторые объекты (реквизиты, табличные части, ...) могут не
        // реализовывать этот метод
        // exportRoot - путь к каталогу выгрузки
        //~ virtual void exportToFiles(fs::path exportRoot);
        
        protected:
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
            const typing::Type& type
        );
        virtual ~Property();

        protected:
        // Тип свойства
        const typing::Type& mType;
    };

    // Список реквизитов
    class PropertyList {
        public:
        PropertyList(std::vector<Property> properties);
        PropertyList();
        void add(Property& p);

        private:
        // Вектор реквизитов
        std::vector<Property> mProperties;
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
        virtual ~Configuration();
        //~ void exportToFiles(fs::path exportRoot) override;

        protected:
        // Список языков
        std::vector<Language> mLanguages;
        // Список справочников
        std::vector<Catalog> mCatalogs;
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
        virtual ~Language();
        //~ void exportToFiles(fs::path exportRoot) override;

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
            PropertyList properties
        );
        virtual ~Catalog();
        //~ void exportToFiles(fs::path exportRoot) override;

        protected:
        // Список реквизитов
        PropertyList mProperties;
    };
    
}

#endif