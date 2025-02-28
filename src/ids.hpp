#ifndef IDS_H
#define IDS_H

// Файл для генерации разного рода идентификаторов
#include <string>
#include <uuid_v4.h>

namespace ids {
// Возвращает строку из 40 символов для версии объекта в выгрузке
std::string getConfigurationVersionString();

// Возвращает строковую презентацию UUID
std::string getUUID();
}

#endif