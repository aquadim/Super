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

    lstring ObjectNode::getSynonym() {
        return mSynonym;
    }

    std::string ObjectNode::getComment() {
        return mComment;
    }
    //=====================================//

    //==========Реквизит==========//
    Property::Property(
        std::string name,
        lstring synonym,
        std::string comment,
        typing::Type* type)
        : ObjectNode{name, synonym, comment}
        , mType{type} {}

    pugi::xml_node Property::makeNode(pugi::xml_node md) {
        auto output         = md.append_child("Attribute");
        auto outputProps    = output.append_child("Properties");
        
        output.append_attribute("uuid").set_value(ids::getUUID().c_str());

        xmltools::addNameNode(outputProps, mName);
        xmltools::addLocalisedString(outputProps.append_child("Synonym"), mSynonym);
        xmltools::addCommentNode(outputProps, mComment);
        xmltools::addTypeNode(outputProps, mType);
        
        return output;
    }

    void Property::generateConfigVersions(pugi::xml_node parent, std::string prefix) {
        xmltools::addConfigVersion(parent, prefix + mName);
    }
    //============================//
    
    //==========Элемент перечисления==========//
    EnumElement::EnumElement(
        std::string name,
        lstring synonym,
        std::string comment)
        : ObjectNode{name, synonym, comment} {}

    pugi::xml_node EnumElement::makeNode(pugi::xml_node md) {
        auto output = md.append_child("EnumValue");
        output.append_attribute("uuid").set_value(ids::getUUID().c_str());

        auto outputProps = output.append_child("Properties");
        xmltools::addNameNode(outputProps, mName);
        xmltools::addLocalisedString(outputProps.append_child("Synonym"), mSynonym);
        xmltools::addCommentNode(outputProps, mComment);
        
        return output;
    }

    void EnumElement::generateConfigVersions(pugi::xml_node parent, std::string prefix) {
        // prefix = Enum.Пол.
        xmltools::addConfigVersion(parent, prefix + "EnumValue." + mName);
    }
    //========================================//

    //==========Колонка табличной части==========//
    TabularColumn::TabularColumn(
        std::string name,
        lstring synonym,
        std::string comment,
        typing::Type* type)
        : ObjectNode{name, synonym, comment}
        , mType{type} {}

    pugi::xml_node TabularColumn::makeNode(pugi::xml_node md) {
        auto output   = md.append_child("Attribute");
        auto colProps = output.append_child("Properties");

        output.append_attribute("uuid").set_value(ids::getUUID());

        xmltools::addNameNode(colProps, mName);
        xmltools::addLocalisedString(colProps.append_child("Synonym"), mSynonym);
        xmltools::addCommentNode(colProps, mComment);
        xmltools::addTypeNode(colProps, mType);

        return output;
    }

    void TabularColumn::generateConfigVersions(pugi::xml_node parent, std::string prefix) {
        xmltools::addConfigVersion(parent, prefix + ".Attribute." + mName);
    }
    //===========================================//

    //==========Табличная часть==========//
    TabularSection::TabularSection(
        std::string name,
        lstring synonym,
        std::string comment,
        std::vector<std::shared_ptr<TabularColumn>> columns)
        : ObjectNode{name, synonym, comment}
        , mColumns{columns} {}

    pugi::xml_node TabularSection::makeNode(pugi::xml_node md) {
        auto output         = md.append_child("TabularSection");
        output.append_child("InternalInfo");
        auto properties     = output.append_child("Properties");
        auto children       = output.append_child("ChildObjects");
        
        output.append_attribute("uuid").set_value(ids::getUUID().c_str());

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

    void TabularSection::generateConfigVersions(pugi::xml_node parent, std::string prefix) {
        // prefix = Catalog.Номенклатура
        xmltools::addConfigVersion(parent, prefix + ".TabularSection." + mName);
        for (const auto& col : mColumns) {
            col->generateConfigVersions(parent, prefix + ".TabularSection." + mName);
        }
    }
    //===================================//

    //==========Список реквизитов==========//
    PropertyList::PropertyList(std::vector<std::shared_ptr<Property>> properties)
        : mProperties{properties} {}

    PropertyList::PropertyList()
        : mProperties{} {}

    void PropertyList::add(
        const std::string& name,
        lstring synonym,
        const std::string& comment,
        typing::Type* type)
    {
        mProperties.push_back(
            std::make_shared<Property>(name, synonym, comment, type)
        );
    }

    void PropertyList::addNodesForAll(pugi::xml_node parent) {
        for (const auto& p : mProperties) {
            p->makeNode(parent);
        }
    }

    void PropertyList::addConfigVersionForAll(
        pugi::xml_node parent,
        std::string prefix
    ) {
        // prefix = Catalog.Номенклатура
        for (const auto& p : mProperties) {
            p->generateConfigVersions(parent, prefix + ".Attribute.");
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

    void TabularsList::addConfigVersionForAll(
        pugi::xml_node parent,
        std::string prefix
    ) {
        // prefix = Catalog.Номенклатура
        for (auto ts : mTabulars) {
            ts.generateConfigVersions(parent, prefix);
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
        output.append_attribute("uuid").set_value(ids::getUUID().c_str());
        return output;
    }

    void Language::generateConfigVersions(pugi::xml_node parent, std::string prefix) {
        xmltools::addConfigVersion(parent, prefix + mName);
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
        mTabulars.addNodesForAll(children, "Catalog", mName);

        doc.save_file((exportRoot / "Catalogs" / (mName + ".xml")).c_str());
        spdlog::info("Выгружено: справочник: {}", mName);
    }

    pugi::xml_node Catalog::makeNode(pugi::xml_node md) {
        auto output = md.append_child("Catalog");
        output.append_attribute("uuid").set_value(ids::getUUID().c_str());
        return output;
    }

    void Catalog::generateConfigVersions(pugi::xml_node parent, std::string prefix) {
        // prefix + mName = Catalog.Номенклатура
        auto entry = xmltools::addConfigVersion(parent, prefix + mName);
        mProperties.addConfigVersionForAll(entry, prefix + mName);
        mTabulars.addConfigVersionForAll(entry, prefix + mName);
    }
    //==============================//
    
    //==========Документ==========//
    Document::Document(
        std::string name,
        lstring synonym,
        std::string comment,
        PropertyList properties,
        TabularsList tabulars
    )
        : ObjectNode{name, synonym, comment}
        , mProperties{properties}
        , mTabulars{tabulars} {}

    void Document::exportToFiles(fs::path exportRoot) {
        auto doc = ObjectNode::createDocument();
        auto obj = this->makeNode(doc.child("MetaDataObject"));

        auto internalInfo   = obj.append_child("InternalInfo");
        auto properties     = obj.append_child("Properties");
        auto children       = obj.append_child("ChildObjects");

        // Внутренняя информация
        xmltools::addGeneratedType(internalInfo, "DocumentObject."+mName, "Object");
        xmltools::addGeneratedType(internalInfo, "DocumentRef."+mName, "Ref");
        xmltools::addGeneratedType(internalInfo, "DocumentSelection."+mName, "Selection");
        xmltools::addGeneratedType(internalInfo, "DocumentList."+mName, "List");
        xmltools::addGeneratedType(internalInfo, "DocumentManager."+mName, "Manager");

        // Свойства документа
        xmltools::addNameNode(properties, mName);
        xmltools::addLocalisedString(properties.append_child("Synonym"), mSynonym);
        xmltools::addCommentNode(properties, mComment);

        // Реквизиты
        mProperties.addNodesForAll(children);

        // Табличные части
        mTabulars.addNodesForAll(children, "Document", mName);

        doc.save_file((exportRoot / "Documents" / (mName + ".xml")).c_str());
        spdlog::info("Выгружено: документ: {}", mName);
    }

    pugi::xml_node Document::makeNode(pugi::xml_node md) {
        auto output = md.append_child("Document");
        output.append_attribute("uuid").set_value(ids::getUUID().c_str());
        return output;
    }

    void Document::generateConfigVersions(pugi::xml_node parent, std::string prefix) {
        auto entry = xmltools::addConfigVersion(parent, prefix + mName);
        mProperties.addConfigVersionForAll(entry, prefix + mName);
        mTabulars.addConfigVersionForAll(entry, prefix + mName);
    }
    //============================//
    
    //==========Перечисление==========//
    Enum::Enum(
        std::string name,
        lstring synonym,
        std::string comment,
        std::vector<std::shared_ptr<EnumElement>> elements
    )
        : ObjectNode{name, synonym, comment}
        , mElements{elements} {}

    void Enum::exportToFiles(fs::path exportRoot) {
        auto doc = ObjectNode::createDocument();
        auto obj = this->makeNode(doc.child("MetaDataObject"));

        auto internalInfo   = obj.append_child("InternalInfo");
        auto properties     = obj.append_child("Properties");
        auto children       = obj.append_child("ChildObjects");

        // Внутренняя информация
        xmltools::addGeneratedType(internalInfo, "EnumRef."+mName, "Ref");
        xmltools::addGeneratedType(internalInfo, "EnumList."+mName, "List");
        xmltools::addGeneratedType(internalInfo, "EnumManager."+mName, "Manager");

        // Свойства перечисления
        xmltools::addNameNode(properties, mName);
        xmltools::addLocalisedString(properties.append_child("Synonym"), mSynonym);
        xmltools::addCommentNode(properties, mComment);

        // Элементы перечисления
        for (const auto& element : mElements) {
            element->makeNode(children);
        }

        doc.save_file((exportRoot / "Enums" / (mName + ".xml")).c_str());
        spdlog::info("Выгружено: перечисление: {}", mName);
    }

    pugi::xml_node Enum::makeNode(pugi::xml_node md) {
        auto output = md.append_child("Enum");
        output.append_attribute("uuid").set_value(ids::getUUID().c_str());
        return output;
    }

    void Enum::generateConfigVersions(pugi::xml_node parent, std::string prefix) {
        // prefix = Enum.
        xmltools::addConfigVersion(parent, prefix + mName);
    }
    //================================//

    //==========Конфигурация==========//
    Configuration::Configuration(
        std::string name,
        lstring synonym,
        std::string comment,
        std::string vendor,
        std::string version,
        std::string updatesAddress,
        std::string defaultLanguageName,
        std::vector<Language> languages,
        std::vector<Catalog> catalogs,
        std::vector<Document> documents,
        std::vector<Enum> enums
    )
        : ObjectNode{name, synonym, comment}
        , mLanguages{languages}
        , mCatalogs{catalogs}
        , mDocuments{documents}
        , mEnums{enums}
        , mVendor{vendor}
        , mVersion{version}
        , mUpdatesAddress{updatesAddress}
    {
        // Поиск основного языка
        int counter = 0;
        bool found = false;
        for (auto l : mLanguages) {
            if (l.getName() == defaultLanguageName) {
                mDefaultLanguageIndex = counter;
                found = true;
                break;
            }
            counter++;
        }
        if (!found) {
            spdlog::warn("Не установлен основной язык конфигурации");
            mDefaultLanguageIndex = 0;
        }
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
        properties.append_child("Version").text().set(mVersion);
        properties.append_child("UpdateCatalogAddress").text().set(mUpdatesAddress);

        // Основной язык
        auto defLanguage = properties.append_child("DefaultLanguage");
        defLanguage.text().set("Language." + mLanguages[mDefaultLanguageIndex].getName());

        // Языки
        fs::create_directory(exportRoot / "Languages");
        for (auto lang : mLanguages) {
            lang.exportToFiles(exportRoot);
            children.append_child("Language").text().set(lang.getName());
        }
        // Справочники
        fs::create_directory(exportRoot / "Catalogs");
        for (auto catalog : mCatalogs) {
            catalog.exportToFiles(exportRoot);
            children.append_child("Catalog").text().set(catalog.getName());
        }
        // Документы
        fs::create_directory(exportRoot / "Documents");
        for (auto document : mDocuments) {
            document.exportToFiles(exportRoot);
            children.append_child("Document").text().set(document.getName());
        }
        // Перечисления
        fs::create_directory(exportRoot / "Enums");
        for (auto enumObj : mEnums) {
            enumObj.exportToFiles(exportRoot);
            children.append_child("Enum").text().set(enumObj.getName());
        }

        doc.save_file((exportRoot / "Configuration.xml").c_str());
        spdlog::info("Выгружено: конфигурация: {}", mName);
    }

    pugi::xml_node Configuration::makeNode(pugi::xml_node md) {
        auto output = md.append_child("Configuration");
        output.append_attribute("uuid").set_value(ids::getUUID().c_str());
        output.append_child("InternalInfo");
        output.append_child("Properties");
        output.append_child("ChildObjects");
        return output;
    }

    void Configuration::generateConfigVersions(pugi::xml_node parent, std::string prefix) {
        xmltools::addConfigVersion(parent, prefix + mName);
        for (auto obj : mLanguages)
            obj.generateConfigVersions(parent, "Language.");
        for (auto obj : mCatalogs)
            obj.generateConfigVersions(parent, "Catalog.");
        for (auto obj : mDocuments)
            obj.generateConfigVersions(parent, "Document.");
        for (auto obj : mEnums)
            obj.generateConfigVersions(parent, "Enum.");
    }

    // Добавляет ContainedObject в <InternalInfo> конфигурации
    void Configuration::addContainedObject(pugi::xml_node parent, std::string uuid) {
        pugi::xml_node containedObject = parent.append_child("xr:ContainedObject");
        containedObject.append_child("xr:ClassId").text().set(uuid);
        containedObject.append_child("xr:ObjectId").text().set(ids::getUUID());
    }
    //================================//
}