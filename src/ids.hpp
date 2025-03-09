#ifndef IDS_H
#define IDS_H

// Файл для генерации разного рода идентификаторов
#include <string>
#include <uuid_v4.h>

using namespace std;

namespace ids {
    // Возвращает строку из 40 символов для версии объекта в выгрузке
    string getConfigurationVersionString();

    // Возвращает строковую презентацию UUID
    string getUUID();

    // Возвращает UUID. Для каждого seed генерируется отдельный UUID
    string getUUIDFor(string seed);
}

#endif