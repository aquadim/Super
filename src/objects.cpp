#include "objects.hpp"
#include <iostream>

namespace objects {

    ObjectNode::ObjectNode(
        std::string name,
        lstring synonym,
        std::string comment
    )
        : mName(name)
        , mSynonym(synonym)
        , mComment(comment) {}
    ObjectNode::~ObjectNode() {}
    std::string ObjectNode::getName() {
        return mName;
    }

    Property::Property(
        std::string name,
        lstring synonym,
        std::string comment,
        const typing::Type& type
    )
        : ObjectNode(name, synonym, comment)
        , mType(type) {}
    Property::~Property() {}

    PropertyList::PropertyList(std::vector<Property> properties)
        : mProperties(properties) {}
    PropertyList::PropertyList()
        : mProperties(std::vector<Property>()) {}
    void PropertyList::add(Property& p) {
        mProperties.push_back(p);
    }

    Configuration::Configuration(
        std::string name,
        lstring synonym,
        std::string comment,
        std::vector<Language> languages,
        std::vector<Catalog> catalogs
    )
        : ObjectNode(name, synonym, comment)
        , mLanguages(languages)
        , mCatalogs(catalogs) {}
    Configuration::~Configuration() {}
        
    Language::Language(
        std::string name,
        lstring synonym,
        std::string comment,
        std::string code
    )
        : ObjectNode(name, synonym, comment)
        , mCode(code) {}
    Language::~Language() {}
        
    Catalog::Catalog(
        std::string name,
        lstring synonym,
        std::string comment,
        PropertyList properties
    )
        : ObjectNode(name, synonym, comment)
        , mProperties(properties) {}
    Catalog::~Catalog() {}
}