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
        PropertyList(std::vector<std::shared_ptr<Property>> properties);
        PropertyList();

        // Добавляет реквизит
        void add(
            const std::string& name,
            lstring synonym,
            const std::string& comment,
            typing::Type* type);

        // Добавляет реквизиты в узел parent
        void addNodesForAll(pugi::xml_node parent);

        void addConfigVersionForAll(
            pugi::xml_node parent,
            std::string prefix
        );

        // Список реквизитов
        std::vector<std::shared_ptr<Property>> mProperties;
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

        void addConfigVersionForAll(
            pugi::xml_node parent,
            std::string prefix
        );
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

        // Создаёт запись в файле ConfigDumpInfo
        virtual void generateConfigVersions(pugi::xml_node parent, std::string prefix) = 0;

        // Добавляет и возвращает узел к MetaDataObject
        virtual pugi::xml_node makeNode(pugi::xml_node md) = 0;

        // Возвращает имя объекта
        std::string getName();

        // Возвращает синоним объекта
        lstring getSynonym();

        // Возвращает комментарий объекта
        std::string getComment();
        
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
        void exportToFiles(fs::path exportRoot) override { (void)exportRoot; }
        pugi::xml_node makeNode(pugi::xml_node md) override;
        void generateConfigVersions(pugi::xml_node parent, std::string prefix) override;

        protected:
        // Тип свойства
        std::shared_ptr<typing::Type> mType;
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
        void generateConfigVersions(pugi::xml_node parent, std::string prefix) override;

        protected:
        // Тип свойства
        std::shared_ptr<typing::Type> mType;
    };

    // Табличная часть
    class TabularSection : public ObjectNode {
        public:
        TabularSection(
            std::string name,
            lstring synonym,
            std::string comment,
            std::vector<std::shared_ptr<TabularColumn>> columns
        );
        void exportToFiles(fs::path exportRoot) override { (void)exportRoot; }
        pugi::xml_node makeNode(pugi::xml_node md) override;
        void generateConfigVersions(pugi::xml_node parent, std::string prefix) override;

        protected:
        std::vector<std::shared_ptr<TabularColumn>> mColumns;
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
        void generateConfigVersions(pugi::xml_node parent, std::string prefix) override;

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
        void generateConfigVersions(pugi::xml_node parent, std::string prefix) override;

        protected:
        // Список реквизитов
        PropertyList mProperties;
        // Список табличных частей
        TabularsList mTabulars;
    };
    
    // Документ
    class Document : public ObjectNode {
        public:
        Document(
            std::string name,
            lstring synonym,
            std::string comment,
            PropertyList properties,
            TabularsList tabulars
        );
        void exportToFiles(fs::path exportRoot) override;
        pugi::xml_node makeNode(pugi::xml_node md) override;
        void generateConfigVersions(pugi::xml_node parent, std::string prefix) override;

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
            std::string vendor,
            std::string version,
            std::string updatesAddress,
            std::string defaultLanguageName,
            std::vector<Language> languages,
            std::vector<Catalog> catalogs,
            std::vector<Document> documents
        );
        void exportToFiles(fs::path exportRoot) override;
        pugi::xml_node makeNode(pugi::xml_node md) override;
        void addContainedObject(pugi::xml_node parent, std::string uuid);
        void generateConfigVersions(pugi::xml_node parent, std::string prefix) override;

        protected:
        // Список языков
        std::vector<Language> mLanguages;
        // Список справочников
        std::vector<Catalog> mCatalogs;
        // Список документов
        std::vector<Document> mDocuments;
        // Поставщик
        std::string mVendor;
        // Версия
        std::string mVersion;
        // Адрес обновлений
        std::string mUpdatesAddress;
        // Индекс основного языка конфигурации в mLanguages
        int mDefaultLanguageIndex;
    };
}

#endif