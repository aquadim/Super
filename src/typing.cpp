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

        // Переменная?
        std::string isVariableValue;
        if (mIsVariable) {
            isVariableValue = "Variable";
        } else {
            isVariableValue = "Fixed";
        }
        qualifiersNode.append_child("v8:AllowedLength").text().set(isVariableValue);
    }
}