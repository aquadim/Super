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
        std::string comment,
        string version,
        shared_ptr<ObjectNode> parent
    )
        : mName{name}
        , mSynonym{synonym}
        , mComment{comment}
        , mParent{parent}
        , mVersion{version} {}

    pugi::xml_document ObjectNode::createDocument() {
        pugi::xml_document output;
        pugi::xml_node md{output.append_child("MetaDataObject")};
        xmltools::addNamespaces(md);
        return output;
    }

    std::string ObjectNode::getName() {
        return mName;
    }

    lstring ObjectNode::getSynonym() {
        return mSynonym;
    }

    std::string ObjectNode::getComment() {
        return mComment;
    }
    //=====================================//

    //==========Реквизит==========//
    Property::Property(
        string name,
        lstring synonym,
        string comment,
        string version,
        shared_ptr<typing::Type> type,
        shared_ptr<ObjectNode> parent)
        : ObjectNode{name, synonym, comment, version, parent}
        , mType{type} {}

    void Property::exportToFiles(fs::path exportRoot) { (void)exportRoot; }

    pugi::xml_node Property::makeNode(pugi::xml_node md) {
        auto output         = md.append_child("Attribute");
        auto outputProps    = output.append_child("Properties");
        
        output.append_attribute("uuid").set_value(ids::getUUIDFor(getQualifiedName()));

        xmltools::addNameNode(outputProps, mName);
        xmltools::addLocalisedString(outputProps.append_child("Synonym"), mSynonym);
        xmltools::addCommentNode(outputProps, mComment);
        mType->addTypeNode(outputProps);
        
        return output;
    }

    string Property::getQualifiedName() {
        return mParent->getQualifiedName() + ".Attribute." + mName;
    }

    void Property::generateConfigVersions(pugi::xml_node parent) {
        xmltools::addConfigVersion(parent, getQualifiedName(), mVersion);
    }
    //============================//
    
    //==========Элемент перечисления==========//
    //~ EnumElement::EnumElement(
        //~ std::string name,
        //~ lstring synonym,
        //~ std::string comment)
        //~ : ObjectNode{name, synonym, comment} {}

    //~ pugi::xml_node EnumElement::makeNode(pugi::xml_node md) {
        //~ auto output = md.append_child("EnumValue");
        //~ output.append_attribute("uuid").set_value(ids::getUUID().c_str());

        //~ auto outputProps = output.append_child("Properties");
        //~ xmltools::addNameNode(outputProps, mName);
        //~ xmltools::addLocalisedString(outputProps.append_child("Synonym"), mSynonym);
        //~ xmltools::addCommentNode(outputProps, mComment);
        
        //~ return output;
    //~ }

    //~ void EnumElement::generateConfigVersions(pugi::xml_node parent, std::string prefix) {
        //~ // prefix = Enum.Пол.
        //~ xmltools::addConfigVersion(parent, prefix + "EnumValue." + mName);
    //~ }
    //========================================//

    //==========Колонка табличной части==========//
    TabularColumn::TabularColumn(
        string name,
        lstring synonym,
        string comment,
        string version,
        shared_ptr<TabularSection> parent,
        shared_ptr<typing::Type> type)
        : ObjectNode{name, synonym, comment, version, parent}
        , mType{type} {}

    void TabularColumn::exportToFiles(fs::path exportRoot) {
        (void)exportRoot;
    }

    pugi::xml_node TabularColumn::makeNode(pugi::xml_node md) {
        auto output   = md.append_child("Attribute");
        auto colProps = output.append_child("Properties");

        output.append_attribute("uuid").set_value(ids::getUUIDFor(getQualifiedName()));

        xmltools::addNameNode(colProps, mName);
        xmltools::addLocalisedString(colProps.append_child("Synonym"), mSynonym);
        xmltools::addCommentNode(colProps, mComment);
        mType->addTypeNode(colProps);

        return output;
    }
    
    string TabularColumn::getQualifiedName() {
        return mParent->getQualifiedName() + ".Attribute." + mName;
    }

    void TabularColumn::generateConfigVersions(pugi::xml_node parent) {
        xmltools::addConfigVersion(
            parent,
            getQualifiedName(),
            mVersion
        );
    }
    //===========================================//

    //==========Табличная часть==========//
    TabularSection::TabularSection(
        string name,
        lstring synonym,
        string comment,
        string version,
        shared_ptr<ObjectNode> parent,
        string generatedTypePrefix)
        : ObjectNode{name, synonym, comment, version, parent}
        , mColumns{}
        , mGeneratedTypePrefix{generatedTypePrefix} {}
    
    void TabularSection::exportToFiles(fs::path exportRoot) {
        (void)exportRoot;
    }

    pugi::xml_node TabularSection::makeNode(pugi::xml_node md) {
        auto output         = md.append_child("TabularSection");
        output.append_attribute("uuid").set_value(ids::getUUIDFor(getQualifiedName()));

        auto internalInfo   = output.append_child("InternalInfo");
        auto properties     = output.append_child("Properties");
        auto children       = output.append_child("ChildObjects");

        // InternalInfo
        xmltools::addGeneratedType(
            internalInfo,
            mGeneratedTypePrefix + "TabularSection." + mParent->getName() + "." + mName,
            "TabularSection"
        );
        xmltools::addGeneratedType(
            internalInfo,
            mGeneratedTypePrefix + "TabularSectionRow." + mParent->getName() + "." + mName,
            "TabularSectionRow"
        );

        // Свойства табличной части
        xmltools::addNameNode(properties, mName);
        xmltools::addLocalisedString(properties.append_child("Synonym"), mSynonym);
        xmltools::addCommentNode(properties, mComment);

        // Колонки табличной части
        for (auto col : mColumns) {
            col->makeNode(children);
        }
        
        return output;
    }

    string TabularSection::getQualifiedName() {
        return mParent->getQualifiedName() + ".TabularSection." + mName;
    }

    void TabularSection::generateConfigVersions(pugi::xml_node parent) {
        xmltools::addConfigVersion(
            parent,
            getQualifiedName(),
            mVersion
        );
        for (auto col : mColumns) {
            col->generateConfigVersions(parent);
        }
    }

    void TabularSection::addColumn(shared_ptr<TabularColumn> column) {
        mColumns.push_back(column);
    }
    //===================================//

    //==========Список реквизитов==========//
    PropertyList::PropertyList(shared_ptr<ObjectNode> parent)
        : mProperties{}
        , mParent{parent} {}

    void PropertyList::add(shared_ptr<Property> p
    ) {
        mProperties.push_back(p);
    }

    void PropertyList::addNodesForAll(pugi::xml_node parent) {
        for (auto p : mProperties) {
            p->makeNode(parent);
        }
    }

    void PropertyList::addConfigVersionForAll(
        pugi::xml_node parent
    ) {
        for (auto p : mProperties) {
            p->generateConfigVersions(parent);
        }
    }
    //=====================================//

    //==========Список табличных частей==========//
    TabularsList::TabularsList(shared_ptr<ObjectNode> parent)
        : mTabulars{}
        , mParent{parent} {}

    void TabularsList::add(shared_ptr<TabularSection> ts) {
        mTabulars.push_back(ts);
    }

    void TabularsList::addNodesForAll(
        pugi::xml_node parent
    ) {
        for (auto ts : mTabulars) {
            ts->makeNode(parent);
        }
    }

    void TabularsList::addConfigVersionForAll(
        pugi::xml_node parent
    ) {
        for (auto ts : mTabulars) {
            ts->generateConfigVersions(parent);
        }
    }
    //===========================================//

    //==========Язык==========//
    Language::Language(
        string name,
        lstring synonym,
        string comment,
        string version,
        shared_ptr<Configuration> parent,
        string code
    )
        : ObjectNode{name, synonym, comment, version, parent }
        , mCode{code} {}

    string Language::getQualifiedName() {
        return "Language." + mName;
    }

    void Language::exportToFiles(fs::path exportRoot) {
        auto doc    = ObjectNode::createDocument();
        auto obj    = this->makeNode(doc.child("MetaDataObject"));
        auto props  = obj.append_child("Properties");

        xmltools::addNameNode(props, mName);
        xmltools::addLocalisedString(props.append_child("Synonym"), mSynonym);
        xmltools::addCommentNode(props, mComment);
        xmltools::addSubNode(props, "LanguageCode", mCode);

        doc.save_file((exportRoot / "Languages" / (mName + ".xml")).c_str());
        spdlog::info("Выгружено: язык: {}", mName);
    }

    pugi::xml_node Language::makeNode(pugi::xml_node md) {
        auto output = md.append_child("Language");
        output.append_attribute("uuid").set_value(ids::getUUIDFor(getQualifiedName()));
        return output;
    }

    void Language::generateConfigVersions(pugi::xml_node parent) {
        xmltools::addConfigVersion(parent, getQualifiedName(), mVersion);
    }
    //========================//

    //==========Документ==========//
    Document::Document(
        string name,
        lstring synonym,
        string comment,
        string version,
        shared_ptr<Configuration> parent
    )
        : ObjectNode{name, synonym, comment, version, parent } {}

    string Document::getQualifiedName() {
        return "Document." + mName;
    }

    void Document::exportToFiles(fs::path exportRoot) {
        auto doc    = ObjectNode::createDocument();
        auto obj    = this->makeNode(doc.child("MetaDataObject"));
        
        auto internalInfo   = obj.append_child("InternalInfo");
        auto properties     = obj.append_child("Properties");
        auto children       = obj.append_child("ChildObjects");

        // Внутренняя информация
        xmltools::addGeneratedType(internalInfo, "DocumentObject."+mName, "Object");
        xmltools::addGeneratedType(internalInfo, "DocumentRef."+mName, "Ref");
        xmltools::addGeneratedType(internalInfo, "DocumentSelection."+mName, "Selection");
        xmltools::addGeneratedType(internalInfo, "DocumentList."+mName, "List");
        xmltools::addGeneratedType(internalInfo, "DocumentManager."+mName, "Manager");

        // Свойства
        xmltools::addNameNode(properties, mName);
        xmltools::addLocalisedString(properties.append_child("Synonym"), mSynonym);
        xmltools::addCommentNode(properties, mComment);

        // Реквизиты
        mProperties->addNodesForAll(children);

        // Табличные части
        mTabulars->addNodesForAll(children);

        doc.save_file((exportRoot / "Documents" / (mName + ".xml")).c_str());
        spdlog::info("Выгружено: документ: {}", mName);
    }

    pugi::xml_node Document::makeNode(pugi::xml_node md) {
        auto output = md.append_child("Document");
        output.append_attribute("uuid").set_value(ids::getUUIDFor(getQualifiedName()));
        return output;
    }

    void Document::generateConfigVersions(pugi::xml_node parent) {
        xmltools::addConfigVersion(parent, getQualifiedName(), mVersion);
        mProperties->addConfigVersionForAll(parent);
        mTabulars->addConfigVersionForAll(parent);
    }

    void Document::setPropertyList(shared_ptr<PropertyList> properties) {
        mProperties = properties;
    }

    void Document::setTabularsList(shared_ptr<TabularsList> tabulars) {
        mTabulars = tabulars;
    }
    //==============================//

    //==========Справочник==========//
    Catalog::Catalog(
        string name,
        lstring synonym,
        string comment,
        string version,
        shared_ptr<Configuration> parent
    )
        : ObjectNode{name, synonym, comment, version, parent } {}

    string Catalog::getQualifiedName() {
        return "Catalog." + mName;
    }

    void Catalog::exportToFiles(fs::path exportRoot) {
        auto doc    = ObjectNode::createDocument();
        auto obj    = this->makeNode(doc.child("MetaDataObject"));
        
        auto internalInfo   = obj.append_child("InternalInfo");
        auto properties     = obj.append_child("Properties");
        auto children       = obj.append_child("ChildObjects");

        // Внутренняя информация
        xmltools::addGeneratedType(internalInfo, "CatalogObject."+mName, "Object");
        xmltools::addGeneratedType(internalInfo, "CatalogRef."+mName, "Ref");
        xmltools::addGeneratedType(internalInfo, "CatalogSelection."+mName, "Selection");
        xmltools::addGeneratedType(internalInfo, "CatalogList."+mName, "List");
        xmltools::addGeneratedType(internalInfo, "CatalogManager."+mName, "Manager");

        // Свойства
        xmltools::addNameNode(properties, mName);
        xmltools::addLocalisedString(properties.append_child("Synonym"), mSynonym);
        xmltools::addCommentNode(properties, mComment);

        // Реквизиты
        mProperties->addNodesForAll(children);

        // Табличные части
        mTabulars->addNodesForAll(children);

        doc.save_file((exportRoot / "Catalogs" / (mName + ".xml")).c_str());
        spdlog::info("Выгружено: справочник: {}", mName);
    }

    pugi::xml_node Catalog::makeNode(pugi::xml_node md) {
        auto output = md.append_child("Catalog");
        output.append_attribute("uuid").set_value(ids::getUUIDFor(getQualifiedName()));
        return output;
    }

    void Catalog::generateConfigVersions(pugi::xml_node parent) {
        xmltools::addConfigVersion(parent, getQualifiedName(), mVersion);
        mProperties->addConfigVersionForAll(parent);
        mTabulars->addConfigVersionForAll(parent);
    }

    void Catalog::setPropertyList(shared_ptr<PropertyList> properties) {
        mProperties = properties;
    }

    void Catalog::setTabularsList(shared_ptr<TabularsList> tabulars) {
        mTabulars = tabulars;
    }
    //==============================//
    
    //==========Перечисление==========//
    //~ Enum::Enum(
        //~ std::string name,
        //~ lstring synonym,
        //~ std::string comment,
        //~ std::vector<std::shared_ptr<EnumElement>> elements
    //~ )
        //~ : ObjectNode{name, synonym, comment}
        //~ , mElements{elements} {}

    //~ void Enum::exportToFiles(fs::path exportRoot) {
        //~ auto doc = ObjectNode::createDocument();
        //~ auto obj = this->makeNode(doc.child("MetaDataObject"));

        //~ auto internalInfo   = obj.append_child("InternalInfo");
        //~ auto properties     = obj.append_child("Properties");
        //~ auto children       = obj.append_child("ChildObjects");

        //~ // Внутренняя информация
        //~ xmltools::addGeneratedType(internalInfo, "EnumRef."+mName, "Ref");
        //~ xmltools::addGeneratedType(internalInfo, "EnumList."+mName, "List");
        //~ xmltools::addGeneratedType(internalInfo, "EnumManager."+mName, "Manager");

        //~ // Свойства перечисления
        //~ xmltools::addNameNode(properties, mName);
        //~ xmltools::addLocalisedString(properties.append_child("Synonym"), mSynonym);
        //~ xmltools::addCommentNode(properties, mComment);

        //~ // Элементы перечисления
        //~ for (const auto& element : mElements) {
            //~ element->makeNode(children);
        //~ }

        //~ doc.save_file((exportRoot / "Enums" / (mName + ".xml")).c_str());
        //~ spdlog::info("Выгружено: перечисление: {}", mName);
    //~ }

    //~ pugi::xml_node Enum::makeNode(pugi::xml_node md) {
        //~ auto output = md.append_child("Enum");
        //~ output.append_attribute("uuid").set_value(ids::getUUID().c_str());
        //~ return output;
    //~ }

    //~ void Enum::generateConfigVersions(pugi::xml_node parent, std::string prefix) {
        //~ // prefix = Enum.
        //~ xmltools::addConfigVersion(parent, prefix + mName);
    //~ }
    //================================//

    //==========Конфигурация==========//
    Configuration::Configuration(
        string name,
        lstring synonym,
        string comment,
        string version,
        string vendor,
        string devVersion,
        string updatesAddress,
        string defaultLanguageName
    )
        : ObjectNode{name, synonym, comment, version, nullptr }
        , mLanguages{}
        , mCatalogs{}
        //~ , mDocuments{}
        //~ , mEnums{}
        , mVendor{vendor}
        , mDevVersion{devVersion}
        , mUpdatesAddress{updatesAddress}
        , mDefaultLanguageName{defaultLanguageName} {}

    string Configuration::getQualifiedName() {
        return "Configuration." + mName;
    }

    void Configuration::addLanguage(shared_ptr<Language> l) {
        mLanguages.push_back(l);
        // Это основной язык?
        if (l->getName() == mDefaultLanguageName) {
            mDefaultLanguageIndex = mLanguages.size() - 1;
        }
    }
    
    void Configuration::addCatalog(shared_ptr<Catalog> c) {
        mCatalogs.push_back(c);
    }
    
    void Configuration::addDocument(shared_ptr<Document> d) {
        mDocuments.push_back(d);
    }

    void Configuration::exportToFiles(fs::path exportRoot) {
        // Документ конфигурации
        auto doc = ObjectNode::createDocument();
        auto obj = this->makeNode(doc.child("MetaDataObject"));

        auto internalInfo   = obj.child("InternalInfo");
        auto properties     = obj.child("Properties");
        auto children       = obj.child("ChildObjects");

        // Обработка InternalInfo
        this->addContainedObject(internalInfo, "9cd510cd-abfc-11d4-9434-004095e12fc7");
        this->addContainedObject(internalInfo, "9fcd25a0-4822-11d4-9414-008048da11f9");
        this->addContainedObject(internalInfo, "e3687481-0a87-462c-a166-9f34594f9bba");
        this->addContainedObject(internalInfo, "9de14907-ec23-4a07-96f0-85521cb6b53b");
        this->addContainedObject(internalInfo, "51f2d5d8-ea4d-4064-8892-82951750031e");
        this->addContainedObject(internalInfo, "e68182ea-4237-4383-967f-90c1e3370bc7");
        this->addContainedObject(internalInfo, "fb282519-d103-4dd3-bc12-cb271d631dfc");

        // Обработка Properties
        // Основные:
        xmltools::addNameNode(properties, mName);
        xmltools::addLocalisedString(properties.append_child("Synonym"), mSynonym);
        xmltools::addCommentNode(properties, mComment);

        // Разработка:
        properties.append_child("Vendor").text().set(mVendor);
        properties.append_child("Version").text().set(mDevVersion);
        properties.append_child("UpdateCatalogAddress").text().set(mUpdatesAddress);

        // Основной язык
        auto defLanguage = properties.append_child("DefaultLanguage");
        defLanguage.text().set("Language." + mLanguages[mDefaultLanguageIndex]->getName());

        // Языки
        fs::create_directory(exportRoot / "Languages");
        for (auto lang : mLanguages) {
            lang->exportToFiles(exportRoot);
            children.append_child("Language").text().set(lang->getName());
        }
        // Справочники
        fs::create_directory(exportRoot / "Catalogs");
        for (auto catalog : mCatalogs) {
            catalog->exportToFiles(exportRoot);
            children.append_child("Catalog").text().set(catalog->getName());
        }
        // Справочники
        fs::create_directory(exportRoot / "Documents");
        for (auto doc : mDocuments) {
            doc->exportToFiles(exportRoot);
            children.append_child("Document").text().set(doc->getName());
        }
        // Перечисления
        //~ fs::create_directory(exportRoot / "Enums");
        //~ for (auto enumObj : mEnums) {
            //~ enumObj.exportToFiles(exportRoot);
            //~ children.append_child("Enum").text().set(enumObj.getName());
        //~ }

        doc.save_file((exportRoot / "Configuration.xml").c_str());
        spdlog::info("Выгружено: конфигурация: {}", mName);
    }

    pugi::xml_node Configuration::makeNode(pugi::xml_node md) {
        auto output = md.append_child("Configuration");
        output.append_attribute("uuid").set_value(ids::getUUIDFor(getQualifiedName()));
        output.append_child("InternalInfo");
        output.append_child("Properties");
        output.append_child("ChildObjects");
        return output;
    }

    void Configuration::generateConfigVersions(pugi::xml_node parent) {
        xmltools::addConfigVersion(parent, getQualifiedName(), mVersion);
        for (auto obj : mLanguages)
            obj->generateConfigVersions(parent);
        for (auto obj : mCatalogs)
            obj->generateConfigVersions(parent);
        for (auto obj : mDocuments)
            obj->generateConfigVersions(parent);
        //~ for (auto obj : mEnums)
            //~ obj.generateConfigVersions(parent, "Enum.");
    }

    // Добавляет ContainedObject в <InternalInfo> конфигурации
    void Configuration::addContainedObject(pugi::xml_node parent, std::string uuid) {
        pugi::xml_node containedObject = parent.append_child("xr:ContainedObject");
        containedObject.append_child("xr:ClassId").text().set(uuid);
        containedObject.append_child("xr:ObjectId").text().set(ids::getUUID());
    }
    //================================//
}