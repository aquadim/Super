#include "xmltools.hpp"
#include "ids.hpp"

namespace xmltools {

    void addSubNode(pugi::xml_node parent, std::string name, std::string value) {
        pugi::xml_node nameNode = parent.append_child(name);
        if (value.size() == 0) {
            return;
        }
        nameNode.text().set(value.c_str());
    }
    
    void addNameNode(pugi::xml_node parent, std::string value) {
        addSubNode(parent, "Name", value);
    }

    void addSynonymNode(pugi::xml_node parent, pugi::xml_node synonym) {
        pugi::xml_node synonymNode = parent.append_child("Synonym");
        addLocalisedString(
            synonymNode,
            parseLocalisedString(synonym.child("localised-string"))
        );
    }
    
    void addCommentNode(pugi::xml_node parent, std::string value) {
        addSubNode(parent, "Comment", value);
    }
    
    void addConfigVersion(
        pugi::xml_node configVersions,
        std::string objectName,
        std::string objectPrefix)
    {
        pugi::xml_node entry = configVersions.append_child("Metadata");
        entry.append_attribute("name").set_value(objectPrefix + '.' + objectName);
        entry.append_attribute("id").set_value(ids::getUUID());
        entry.append_attribute("configVersion").set_value(ids::getConfigurationVersionString());
    }

    void addChildObject(
        pugi::xml_node childrenNode,
        std::string objectName,
        std::string objectType)
    {
        pugi::xml_node childNode = childrenNode.append_child(objectType);
        childNode.text().set(objectName.c_str());
    }

    void addNamespaces(pugi::xml_node node) {
        node.append_attribute("xmlns").set_value("http://v8.1c.ru/8.3/MDClasses");
        node.append_attribute("xmlns:app").set_value("http://v8.1c.ru/8.2/managed-application/core");
        node.append_attribute("xmlns:cfg").set_value("http://v8.1c.ru/8.1/data/enterprise/current-config");
        node.append_attribute("xmlns:cmi").set_value("http://v8.1c.ru/8.2/managed-application/cmi");
        node.append_attribute("xmlns:ent").set_value("http://v8.1c.ru/8.1/data/enterprise");
        node.append_attribute("xmlns:lf").set_value("http://v8.1c.ru/8.2/managed-application/logform");
        node.append_attribute("xmlns:style").set_value("http://v8.1c.ru/8.1/data/ui/style");
        node.append_attribute("xmlns:sys").set_value("http://v8.1c.ru/8.1/data/ui/fonts/system");
        node.append_attribute("xmlns:v8").set_value("http://v8.1c.ru/8.1/data/core");
        node.append_attribute("xmlns:v8ui").set_value("http://v8.1c.ru/8.1/data/ui");
        node.append_attribute("xmlns:web").set_value("http://v8.1c.ru/8.1/data/ui/colors/web");
        node.append_attribute("xmlns:win").set_value("http://v8.1c.ru/8.1/data/ui/colors/windows");
        node.append_attribute("xmlns:xen").set_value("http://v8.1c.ru/8.3/xcf/enums");
        node.append_attribute("xmlns:xpr").set_value("http://v8.1c.ru/8.3/xcf/predef");
        node.append_attribute("xmlns:xr").set_value("http://v8.1c.ru/8.3/xcf/readable");
        node.append_attribute("xmlns:xs").set_value("http://www.w3.org/2001/XMLSchema");
        node.append_attribute("xmlns:xsi").set_value("http://www.w3.org/2001/XMLSchema-instance");
        // TODO: version - ?
    }

    std::unordered_map<std::string, std::string> parseLocalisedString(pugi::xml_node node) {
        std::unordered_map<std::string, std::string> output;
        for (pugi::xml_node lang = node.child("language"); lang; lang = lang.next_sibling("language")) {
            output[lang.attribute("id").as_string()] = lang.text().get();
        }
        return output;
    }

    void addLocalisedString(
        pugi::xml_node node,
        std::unordered_map<std::string, std::string> langMap
    )
    {
        for (auto it : langMap) {
            pugi::xml_node v8item = node.append_child("v8:item");
            v8item.append_child("v8:lang").text().set(it.first);
            v8item.append_child("v8:content").text().set(it.second);
        }
    }

    void addGeneratedType(
        pugi::xml_node node,
        std::string name,
        std::string category
    )
    {
        pugi::xml_node generatedTypeNode = node.append_child("xr:GeneratedType");
        generatedTypeNode.append_attribute("name").set_value(name);
        generatedTypeNode.append_attribute("category").set_value(category);
        generatedTypeNode.append_child("xr:TypeId").text().set(ids::getUUID());
        generatedTypeNode.append_child("xr:ValueId").text().set(ids::getUUID());
    }

    typing::Type* parseTypeNode(pugi::xml_node node) {
        std::string typeId = node.attribute("id").as_string();

        // Если это строка
        if (typeId == "std::string") {
            return new typing::String(
                node.attribute("length").as_int(),
                node.attribute("variable").as_bool()
            );
        }

        // Если это число
        if (typeId == "std::int") {
            return new typing::Integer(
                node.attribute("length").as_int(),
                node.attribute("onlyPositive").as_bool()
            );
        }
        
        // Если это плавающее
        if (typeId == "std::float") {
            return new typing::Float(
                node.attribute("length").as_int(),
                node.attribute("fractionLength").as_int(),
                node.attribute("onlyPositive").as_bool()
            );
        }

        // Получить пространства имён
        // Разделение строки https://stackoverflow.com/a/46931770/15146417
        std::string delimiter = "::";
        size_t posStart = 0, posEnd, delimLen = delimiter.length();
        std::string token;
        std::vector<std::string> tokens;
        while ((posEnd = typeId.find(delimiter, posStart)) != std::string::npos) {
            token = typeId.substr(posStart, posEnd - posStart);
            posStart = posEnd + delimLen;
            tokens.push_back(token);
        }
        tokens.push_back(typeId.substr(posStart));

        if (tokens.size() == 3) {
            // std::???::id
            if (tokens[1] == "CatalogRef") {
                return new typing::Ref("CatalogRef", tokens[2]);
            }
        }

        // Не понимаем что за тип
        std::cerr << "Неизвестный тип " + typeId << std::endl;
        return new typing::Type(typeId);
    }
}