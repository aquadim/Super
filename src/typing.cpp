#include "typing.hpp"

namespace typing {

    // -- Неконкретный тип -- //
    Type::Type(std::string id)
        : mTypeId(id)
    {}
    void Type::addTypeNode(pugi::xml_node parent) {
        pugi::xml_node typeNode         = parent.append_child("Type");
        pugi::xml_node v8typeNode       = typeNode.append_child("v8:Type");
        v8typeNode.text().set(mTypeId);
    }
    
    // -- Ссылка -- //
    Ref::Ref(std::string classId, std::string id)
        : Type("cfg:" + classId + "." + id)
    {}
    
    // -- std::string -- //
    String::String(int length, bool variable)
        : Type("xs:string")
        , mLength(length)
        , mIsVariable(variable)
    {}
    void String::addTypeNode(pugi::xml_node parent) {
        pugi::xml_node typeNode         = parent.append_child("Type");
        pugi::xml_node v8typeNode       = typeNode.append_child("v8:Type");
        pugi::xml_node qualifiersNode   = typeNode.append_child("v8:StringQualifiers");

        v8typeNode.text().set(mTypeId);

        // Длина строки
        qualifiersNode.append_child("v8:Length").text().set(mLength);

        qualifiersNode.append_child("v8:AllowedLength").text().set(
            mIsVariable ? "Variable" : "Fixed"
        );
    }
    
    // -- std::int -- //
    Integer::Integer(int length, bool onlyPositive)
        : Type("xs:decimal")
        , mLength(length)
        , mIsOnlyPositive(onlyPositive)
    {}
    void Integer::addTypeNode(pugi::xml_node parent) {
        pugi::xml_node typeNode         = parent.append_child("Type");
        pugi::xml_node v8typeNode       = typeNode.append_child("v8:Type");
        pugi::xml_node qualifiersNode   = typeNode.append_child("v8:NumberQualifiers");

        v8typeNode.text().set(mTypeId);

        // Длина числа
        qualifiersNode.append_child("v8:Digits").text().set(mLength);
        qualifiersNode.append_child("v8:FractionDigits").text().set(0);
        qualifiersNode.append_child("v8:AllowedLength").text().set(
            mIsOnlyPositive ? "Any" : "Nonnegative"
        );
    }
    
    // -- std::float -- //
    Float::Float(int length, int fractionLength, bool onlyPositive)
        : Integer(length, onlyPositive)
        , mFractionLength(fractionLength)
    {}
    void Float::addTypeNode(pugi::xml_node parent) {
        pugi::xml_node typeNode         = parent.append_child("Type");
        pugi::xml_node v8typeNode       = typeNode.append_child("v8:Type");
        pugi::xml_node qualifiersNode   = typeNode.append_child("v8:NumberQualifiers");

        v8typeNode.text().set(mTypeId);

        // Длина числа
        qualifiersNode.append_child("v8:Digits").text().set(mLength);
        qualifiersNode.append_child("v8:FractionDigits").text().set(mFractionLength);
        qualifiersNode.append_child("v8:AllowedLength").text().set(
            mIsOnlyPositive ? "Any" : "Nonnegative"
        );
    }
}