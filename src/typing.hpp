#ifndef TYPING_H
#define TYPING_H

// Структуры для хранения типов
#include <string>
#include <pugixml.hpp>

namespace typing {

    // Не конкретный тип
    class Type {
    public:
        Type(std::string id);
        virtual void addTypeNode(pugi::xml_node parent);
        virtual ~Type() {};
    protected:
        std::string mTypeId;
    };

    // Тип строка
    class String : public Type {
    public:
        String(int length, bool variable);
        void addTypeNode(pugi::xml_node parent) override;
    private:
        int mLength;
        bool mIsVariable;
    };
    
}

#endif