#include "objects.hpp"
#include <iostream>
#include "xmltools.hpp"
#include "ids.hpp"
#include <spdlog/spdlog.h>

namespace objects {

    //==========Узел конфигурации==========//
    ObjectNode::ObjectNode(
        std::string name,
        lstring synonym,
        std::string comment
    )
        : mName{name}
        , mSynonym{synonym}
        , mComment{comment} {}

    pugi::xml_document ObjectNode::createDocument() {
        pugi::xml_document output;
        pugi::xml_node md{output.append_child("MetaDataObject")};
        xmltools::addNamespaces(md);
        return output;
    }

    std::string ObjectNode::getName() {
        return mName;
    }
    //=====================================//

    //==========Реквизит==========//
    Property::Property(
        std::string name,
        lstring synonym,
        std::string comment,
        typing::Type* type)
        : ObjectNode{name, synonym, comment}
        , mType{*type} {}

    pugi::xml_node Property::makeNode(pugi::xml_node md) {
        auto output         = md.append_child("Attribute");
        auto outputProps    = output.append_child("Properties");
        
        output.append_attribute("uuid").set_value(ids::getUUID().c_str());

        xmltools::addNameNode(outputProps, mName);
        xmltools::addLocalisedString(outputProps.append_child("Synonym"), mSynonym);
        xmltools::addCommentNode(outputProps, mComment);

        // Добавление типа
        mType.addTypeNode(outputProps);
        
        return output;
    }
    //============================//

    //==========Колонка табличной части==========//
    TabularColumn::TabularColumn(
        std::string name,
        lstring synonym,
        std::string comment,
        typing::Type* type)
        : ObjectNode{name, synonym, comment}
        , mType{*type} {}

    pugi::xml_node TabularColumn::makeNode(pugi::xml_node md) {
        auto output = md.append_child("TabularColumn");
        output.append_attribute("uuid").set_value(ids::getUUID().c_str());
        return output;
    }
    //===========================================//

    //==========Табличная часть==========//
    TabularSection::TabularSection(
        std::string name,
        lstring synonym,
        std::string comment,
        std::vector<TabularColumn> columns)
        : ObjectNode{name, synonym, comment}
        , mColumns{columns} {}

    pugi::xml_node TabularSection::makeNode(pugi::xml_node md) {
        auto output         = md.append_child("TabularSection");
        output.append_child("InternalInfo");
        auto properties     = output.append_child("Properties");
        auto children       = output.append_child("ChildObjects");
        
        output.append_attribute("uuid").set_value(ids::getUUID().c_str());

        xmltools::addNameNode(properties, mName);
        xmltools::addLocalisedString(properties.append_child("Synonym"), mSynonym);
        xmltools::addCommentNode(properties, mComment);
        
        return output;
    }
    //===================================//

    //==========Список реквизитов==========//
    PropertyList::PropertyList(std::vector<Property> properties)
        : mProperties{properties} {}

    PropertyList::PropertyList()
        : mProperties{std::vector<Property>{}} {}

    void PropertyList::add(Property p) {
        mProperties.push_back(p);
    }

    void PropertyList::addNodesForAll(pugi::xml_node parent) {
        for (auto p : mProperties) {
            p.makeNode(parent);
        }
    }
    //=====================================//

    //==========Список табличных частей==========//
    TabularsList::TabularsList(std::vector<TabularSection> tabulars)
        : mTabulars{tabulars} {}

    TabularsList::TabularsList()
        : mTabulars{std::vector<TabularSection>{}} {}

    void TabularsList::add(TabularSection ts) {
        mTabulars.push_back(ts);
    }

    void TabularsList::addNodesForAll(
        pugi::xml_node parent,
        std::string prefix,
        std::string ownerName
    )
    {
        for (auto ts : mTabulars) {
            auto node = ts.makeNode(parent);

            // Заполнение InternalInfo
            auto internalInfo = node.child("InternalInfo");
            xmltools::addGeneratedType(
                internalInfo,
                prefix + "TabularSection." + ownerName + "." + ts.getName(),
                "TabularSection"
            );
            xmltools::addGeneratedType(
                internalInfo,
                prefix + "TabularSectionRow." + ownerName + "." + ts.getName(),
                "TabularSectionRow"
            );
        }
    }
    //===========================================//

    //==========Язык==========//
    Language::Language(
        std::string name,
        lstring synonym,
        std::string comment,
        std::string code
    )
        : ObjectNode{name, synonym, comment}
        , mCode{code} {}

    void Language::exportToFiles(fs::path exportRoot) {
        auto doc = ObjectNode::createDocument();
        auto obj = this->makeNode(doc.child("MetaDataObject"));

        xmltools::addNameNode(obj, mName);
        xmltools::addLocalisedString(obj.append_child("Synonym"), mSynonym);
        xmltools::addCommentNode(obj, mComment);
        xmltools::addSubNode(obj, "LanguageCode", mCode);

        doc.save_file((exportRoot / "Languages" / (mName + ".xml")).c_str());
        spdlog::info("Выгружено: язык: {}", mName);
    }

    pugi::xml_node Language::makeNode(pugi::xml_node md) {
        auto output = md.append_child("Language");
        output.append_attribute("uuid").set_value(ids::getUUID().c_str());
        return output;
    }
    //========================//

    //==========Справочник==========//
    Catalog::Catalog(
        std::string name,
        lstring synonym,
        std::string comment,
        PropertyList properties,
        TabularsList tabulars
    )
        : ObjectNode{name, synonym, comment}
        , mProperties{properties}
        , mTabulars{tabulars} {}

    void Catalog::exportToFiles(fs::path exportRoot) {
        auto doc = ObjectNode::createDocument();
        auto obj = this->makeNode(doc.child("MetaDataObject"));

        auto internalInfo   = obj.append_child("InternalInfo");
        auto properties     = obj.append_child("Properties");
        auto children       = obj.append_child("ChildObjects");

        // Внутренняя информация
        xmltools::addGeneratedType(internalInfo, "CatalogObject."+mName, "Object");
        xmltools::addGeneratedType(internalInfo, "CatalogRef."+mName, "Ref");
        xmltools::addGeneratedType(internalInfo, "CatalogSelection."+mName, "Selection");
        xmltools::addGeneratedType(internalInfo, "CatalogList."+mName, "List");
        xmltools::addGeneratedType(internalInfo, "CatalogManager."+mName, "Manager");

        // Свойства справочника
        xmltools::addNameNode(properties, mName);
        xmltools::addLocalisedString(properties.append_child("Synonym"), mSynonym);
        xmltools::addCommentNode(properties, mComment);

        // Реквизиты
        mProperties.addNodesForAll(children);

        // Табличные части
        mTabulars.addNodesForAll(
            children,
            "Catalog",
            mName
        );

        doc.save_file((exportRoot / "Catalogs" / (mName + ".xml")).c_str());
        spdlog::info("Выгружено: справочник: {}", mName);
    }

    pugi::xml_node Catalog::makeNode(pugi::xml_node md) {
        auto output = md.append_child("Catalog");
        output.append_attribute("uuid").set_value(ids::getUUID().c_str());
        return output;
    }
    //==============================//

    //==========Конфигурация==========//
    Configuration::Configuration(
        std::string name,
        lstring synonym,
        std::string comment,
        std::vector<Language> languages,
        std::vector<Catalog> catalogs
    )
        : ObjectNode{name, synonym, comment}
        , mLanguages{languages}
        , mCatalogs{catalogs} {}

    void Configuration::exportToFiles(fs::path exportRoot) {
        // Документ версий объектов
        pugi::xml_document versionsDoc;

        // Документ конфигурации
        auto doc = ObjectNode::createDocument();
        auto obj = this->makeNode(doc.child("MetaDataObject"));

        xmltools::addNameNode(obj, mName);

        // Языки
        fs::create_directory(exportRoot / "Languages");
        for (auto lang : mLanguages) {
            lang.exportToFiles(exportRoot);
        }
        // Справочники
        fs::create_directory(exportRoot / "Catalogs");
        for (auto catalog : mCatalogs) {
            catalog.exportToFiles(exportRoot);
        }

        doc.save_file((exportRoot / "Configuration.xml").c_str());
        spdlog::info("Выгружено: конфигурация: {}", mName);
    }

    pugi::xml_node Configuration::makeNode(pugi::xml_node md) {
        auto output = md.append_child("Configuration");
        output.append_attribute("uuid").set_value(ids::getUUID().c_str());
        return output;
    }
    //================================//
}