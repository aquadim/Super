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
    class Configuration;

    // Список реквизитов
    //~ class PropertyList {
        //~ public:
        //~ PropertyList(vector<shared_ptr<Property>> properties);
        //~ PropertyList();

        //~ // Добавляет реквизит
        //~ void add(
            //~ string name,
            //~ lstring synonym,
            //~ string comment,
            //~ shared_ptr<typing::Type> type);

        //~ // Добавляет реквизиты в узел parent
        //~ void addNodesForAll(pugi::xml_node parent);

        //~ void addConfigVersionForAll(
            //~ pugi::xml_node parent,
            //~ string prefix
        //~ );

        //~ // Список реквизитов
        //~ vector<shared_ptr<Property>> mProperties;
    //~ };
    
    // Список табличных частей
    //~ class TabularsList {
        //~ public:
        //~ TabularsList(vector<TabularSection> sections);
        //~ TabularsList();

        //~ // Добавляет табличную часть
        //~ void add(TabularSection ts);

        //~ // Добавляет табличные части в узел parent
        //~ // prefix - Catalog/Document -- префикс для узлов в InternalInfo
        //~ // ownerName - имя узла-владельца
        //~ void addNodesForAll(
            //~ pugi::xml_node parent,
            //~ string prefix,
            //~ string ownerName
        //~ );

        //~ // Список табличных частей
        //~ vector<TabularSection> mTabulars;

        //~ void addConfigVersionForAll(
            //~ pugi::xml_node parent,
            //~ string prefix
        //~ );
    //~ };

    // Узел конфигурации. Может хранить имя, синоним, комментарий
    class ObjectNode {
        public:
        ObjectNode(
            string name,
            lstring synonym,
            string comment,
            shared_ptr<ObjectNode> parent,
            string version
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
    //~ class Property : public ObjectNode {
        //~ public:
        //~ Property(
            //~ string name,
            //~ lstring synonym,
            //~ string comment,
            //~ shared_ptr<typing::Type> type,
            //~ shared_ptr<ObjectNode> parent
        //~ );
        //~ void exportToFiles(fs::path exportRoot) override { (void)exportRoot; }
        //~ pugi::xml_node makeNode(pugi::xml_node md) override;

        //~ protected:
        //~ // Тип свойства
        //~ shared_ptr<typing::Type> mType;
    //~ };

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
    //~ class TabularColumn : public ObjectNode {
        //~ public:
        //~ TabularColumn(
            //~ string name,
            //~ lstring synonym,
            //~ string comment,
            //~ shared_ptr<typing::Type> type,
            //~ shared_ptr<ObjectNode> parent
        //~ );
        //~ void exportToFiles(fs::path exportRoot) override { (void)exportRoot; }

        //~ protected:
        //~ // Тип свойства
        //~ shared_ptr<typing::Type> mType;
    //~ };

    // Табличная часть
    //~ class TabularSection : public ObjectNode {
        //~ public:
        //~ TabularSection(
            //~ string name,
            //~ lstring synonym,
            //~ string comment,
            //~ vector<shared_ptr<TabularColumn>> columns,
            //~ shared_ptr<ObjectNode> parent
        //~ );
        //~ void exportToFiles(fs::path exportRoot) override { (void)exportRoot; }
        //~ pugi::xml_node makeNode(pugi::xml_node md) override;

        //~ protected:
        //~ vector<shared_ptr<TabularColumn>> mColumns;
    //~ };

    // Язык
    class Language : public ObjectNode {
        public:
        Language(
            string name,
            lstring synonym,
            string comment,
            string version,
            string code,
            shared_ptr<Configuration> parent
        );
        void exportToFiles(fs::path exportRoot) override;
        pugi::xml_node makeNode(pugi::xml_node md) override;
        string getQualifiedName() override;
        void generateConfigVersions(pugi::xml_node parent) override;

        protected:
        string mCode;
    };
    
    // Справочник
    //~ class Catalog : public ObjectNode {
        //~ public:
        //~ Catalog(
            //~ string name,
            //~ lstring synonym,
            //~ string comment,
            //~ PropertyList properties,
            //~ TabularsList tabulars,
            //~ shared_ptr<ObjectNode> parent
        //~ );
        //~ void exportToFiles(fs::path exportRoot) override;
        //~ pugi::xml_node makeNode(pugi::xml_node md) override;

        //~ protected:
        //~ // Список реквизитов
        //~ PropertyList mProperties;
        //~ // Список табличных частей
        //~ TabularsList mTabulars;
    //~ };
    
    // Документ
    //~ class Document : public ObjectNode {
        //~ public:
        //~ Document(
            //~ string name,
            //~ lstring synonym,
            //~ string comment,
            //~ PropertyList properties,
            //~ TabularsList tabulars,
            //~ shared_ptr<ObjectNode> parent
        //~ );
        //~ void exportToFiles(fs::path exportRoot) override;
        //~ pugi::xml_node makeNode(pugi::xml_node md) override;

        //~ protected:
        //~ // Список реквизитов
        //~ PropertyList mProperties;
        //~ // Список табличных частей
        //~ TabularsList mTabulars;
    //~ };

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
        void addContainedObject(pugi::xml_node parent, string uuid);
        string getQualifiedName() override;
        void generateConfigVersions(pugi::xml_node parent) override;

        void addLanguage(shared_ptr<Language> l);

        protected:
        // Список языков
        vector<shared_ptr<Language>> mLanguages;
        // Список справочников
        //~ vector<Catalog> mCatalogs;
        // Список документов
        //~ vector<Document> mDocuments;
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