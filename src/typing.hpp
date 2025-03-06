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
    
    // Ссылка на что-либо
    class Ref : public Type {
    public:
        Ref(std::string classId, std::string id);
    };

    // Тип строка
    class String : public Type {
    public:
        String(int length, bool variable);
        void addTypeNode(pugi::xml_node parent) override;
    protected:
        int mLength;
        bool mIsVariable;
    };
    
    // Тип число
    class Integer : public Type {
    public:
        Integer(int length, bool onlyPositive);
        void addTypeNode(pugi::xml_node parent) override;
    protected:
        int mLength;
        bool mIsOnlyPositive;
    };
    
    // Тип плавающее
    class Float : public Integer {
    public:
        Float(int length, int fractionLength, bool onlyPositive);
        void addTypeNode(pugi::xml_node parent) override;
    protected:
        int mFractionLength;
    };
    
}

#endif