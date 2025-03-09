#ifndef TYPING_H
#define TYPING_H

// Встроенные типы 1С
#include <string>
#include <pugixml.hpp>
#include <memory>

using namespace std;

namespace typing {

    // Не конкретный тип
    class Type {
    public:
        Type(string id);
        virtual void addTypeNode(pugi::xml_node parent);
        virtual ~Type() {};
    protected:
        string mTypeId;
    };
    
    // Ссылка на что-либо
    class Ref : public Type {
    public:
        Ref(string classId, string id);
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