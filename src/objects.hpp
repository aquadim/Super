#pragma once

// Классы бизнес-объектов конфигурации
#include <unordered_map>
#include <vector>
#include "typing.hpp"
#include <pugixml.hpp>
#include <filesystem>
#include <memory>

// Словарь, где ключ - код языка, а значение - строка на этом языке
using lstring = unordered_map<string, string>;
using namespace std;

namespace fs = std::filesystem;

namespace objects {

    class Property;
    class TabularSection;
    class ObjectNode;
    class Configuration;
    class Catalog;

    // Список реквизитов
    class PropertyList {
        public:
        PropertyList(shared_ptr<ObjectNode> parent);
        // Добавляет реквизит
        void add(shared_ptr<Property> p);
        // Добавляет реквизиты в узел parent
        void addNodesForAll(pugi::xml_node parent);
        void addConfigVersionForAll(pugi::xml_node parent);
        
        private:
        // Список реквизитов
        vector<shared_ptr<Property>> mProperties;
        // Владелец реквизитов
        shared_ptr<ObjectNode> mParent;
    };

    // Список табличных частей
    class TabularsList {
        public:
        TabularsList(shared_ptr<ObjectNode> parent);
        // Добавляет табличную часть
        void add(shared_ptr<TabularSection> ts);
        // Добавляет реквизиты в узел parent
        void addNodesForAll(pugi::xml_node parent);
        void addConfigVersionForAll(pugi::xml_node parent);
        
        private:
        // Список табличных частей
        vector<shared_ptr<TabularSection>> mTabulars;
        // Владелец табличных частей
        shared_ptr<ObjectNode> mParent;
    };

    // Узел конфигурации. Может хранить имя, синоним, комментарий
    class ObjectNode {
        public:
        ObjectNode(
            string name,
            lstring synonym,
            string comment,
            string version,
            shared_ptr<ObjectNode> parent
        );
        virtual ~ObjectNode() {};
        // Экспортирует объект в файл
        // Некоторые объекты (реквизиты, табличные части, ...) могут не
        // реализовывать этот метод
        // exportRoot - путь к каталогу выгрузки
        virtual void exportToFiles(fs::path exportRoot) = 0;
        // Создаёт запись в файле ConfigDumpInfo
        virtual void generateConfigVersions(pugi::xml_node parent) = 0;
        // Добавляет и возвращает узел к MetaDataObject
        virtual pugi::xml_node makeNode(pugi::xml_node md) = 0;
        // Возвращает полный путь объекта
        virtual string getQualifiedName() = 0;
        // Возвращает имя объекта
        string getName();
        // Возвращает синоним объекта
        lstring getSynonym();
        // Возвращает комментарий объекта
        string getComment();
        
        protected:
        // Создаёт файл объекта в выгрузке
        pugi::xml_document createDocument();
        // Имя объекта
        string mName;
        // Синоним
        lstring mSynonym;
        // Комментарий
        string mComment;
        // Родитель объекта
        shared_ptr<ObjectNode> mParent;
        // Версия объекта
        string mVersion;
    };

    // Реквизит узла конфигурации
    class Property : public ObjectNode {
        public:
        Property(
            string name,
            lstring synonym,
            string comment,
            string version,
            shared_ptr<typing::Type> type,
            shared_ptr<ObjectNode> parent
        );
        void exportToFiles(fs::path exportRoot) override;
        pugi::xml_node makeNode(pugi::xml_node md) override;
        string getQualifiedName() override;
        void generateConfigVersions(pugi::xml_node parent) override;

        protected:
        shared_ptr<typing::Type> mType;
    };

    // Элемент перечисления
    //~ class EnumElement : public ObjectNode {
        //~ public:
        //~ EnumElement(
            //~ string name,
            //~ lstring synonym,
            //~ string comment,
            //~ shared_ptr<ObjectNode> parent
        //~ );
        //~ void exportToFiles(fs::path exportRoot) override { (void)exportRoot; }
        //~ pugi::xml_node makeNode(pugi::xml_node md) override;
        //~ void generateConfigVersions(pugi::xml_node parent, string prefix) override;
    //~ };
    
    // Колонка табличной части
    class TabularColumn : public ObjectNode {
        public:
        TabularColumn(
            string name,
            lstring synonym,
            string comment,
            string version,
            shared_ptr<TabularSection> parent,
            shared_ptr<typing::Type> type
        );
        void exportToFiles(fs::path exportRoot) override;
        pugi::xml_node makeNode(pugi::xml_node md) override;
        string getQualifiedName() override;
        void generateConfigVersions(pugi::xml_node parent) override;

        protected:
        shared_ptr<typing::Type> mType;
    };

    // Табличная часть
    class TabularSection : public ObjectNode {
        public:
        TabularSection(
            string name,
            lstring synonym,
            string comment,
            string version,
            shared_ptr<ObjectNode> parent,
            string generatedTypePrefix
        );
        void exportToFiles(fs::path exportRoot) override;
        pugi::xml_node makeNode(pugi::xml_node md) override;
        string getQualifiedName() override;
        void generateConfigVersions(pugi::xml_node parent) override;

        void addColumn(shared_ptr<TabularColumn> column);

        protected:
        // Колонки ТЧ
        vector<shared_ptr<TabularColumn>> mColumns;
        // префикс для <GeneratedType>
        string mGeneratedTypePrefix;
    };

    // Язык
    class Language : public ObjectNode {
        public:
        Language(
            string name,
            lstring synonym,
            string comment,
            string version,
            shared_ptr<Configuration> parent,
            string code
        );
        void exportToFiles(fs::path exportRoot) override;
        pugi::xml_node makeNode(pugi::xml_node md) override;
        string getQualifiedName() override;
        void generateConfigVersions(pugi::xml_node parent) override;

        protected:
        string mCode;
    };
    
    // Перечисление
    //~ class Enum : public ObjectNode {
        //~ public:
        //~ Enum(
            //~ string name,
            //~ lstring synonym,
            //~ string comment,
            //~ vector<shared_ptr<EnumElement>> elements,
            //~ shared_ptr<ObjectNode> parent
        //~ );
        //~ void exportToFiles(fs::path exportRoot) override;
        //~ pugi::xml_node makeNode(pugi::xml_node md) override;

        //~ protected:
        //~ // Список реквизитов
        //~ vector<shared_ptr<EnumElement>> mElements;
    //~ };

    // Документ
    class Document : public ObjectNode {
        public:
        Document(
            string name,
            lstring synonym,
            string comment,
            string version,
            shared_ptr<Configuration> parent
        );
        void exportToFiles(fs::path exportRoot) override;
        pugi::xml_node makeNode(pugi::xml_node md) override;
        string getQualifiedName() override;
        void generateConfigVersions(pugi::xml_node parent) override;

        void setPropertyList(shared_ptr<PropertyList> properties);
        void setTabularsList(shared_ptr<TabularsList> tabulars);

        protected:
        // Список реквизитов
        shared_ptr<PropertyList> mProperties;
        // Список ТЧ
        shared_ptr<TabularsList> mTabulars;
    };
    
    // Справочник
    class Catalog : public ObjectNode {
        public:
        Catalog(
            string name,
            lstring synonym,
            string comment,
            string version,
            shared_ptr<Configuration> parent
        );
        void exportToFiles(fs::path exportRoot) override;
        pugi::xml_node makeNode(pugi::xml_node md) override;
        string getQualifiedName() override;
        void generateConfigVersions(pugi::xml_node parent) override;

        void setPropertyList(shared_ptr<PropertyList> properties);
        void setTabularsList(shared_ptr<TabularsList> tabulars);

        protected:
        // Список реквизитов
        shared_ptr<PropertyList> mProperties;
        // Список ТЧ
        shared_ptr<TabularsList> mTabulars;
    };

    // Конфигурация -- корневой узел
    class Configuration : public ObjectNode {
        public:
        Configuration(
            string name,
            lstring synonym,
            string comment,
            string version,
            string vendor,
            string devVersion,
            string updatesAddress,
            string defaultLanguageName
        );
        void exportToFiles(fs::path exportRoot) override;
        pugi::xml_node makeNode(pugi::xml_node md) override;
        string getQualifiedName() override;
        void generateConfigVersions(pugi::xml_node parent) override;

        void addLanguage(shared_ptr<Language> l);
        void addCatalog(shared_ptr<Catalog> c);
        void addDocument(shared_ptr<Document> d);
        void addContainedObject(pugi::xml_node parent, string uuid);

        protected:
        // Список языков
        vector<shared_ptr<Language>> mLanguages;
        // Список справочников
        vector<shared_ptr<Catalog>> mCatalogs;
        // Список документов
        vector<shared_ptr<Document>> mDocuments;
        // Список перечисления
        //~ vector<Enum> mEnums;
        // Поставщик
        string mVendor;
        // Версия
        string mDevVersion;
        // Адрес обновлений
        string mUpdatesAddress;
        // Имя основного языка конфигурации
        string mDefaultLanguageName;
        // Индекс основного языка конфигурации в mLanguages
        int mDefaultLanguageIndex;
    };
}