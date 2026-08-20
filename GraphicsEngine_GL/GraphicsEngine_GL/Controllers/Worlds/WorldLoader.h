#ifndef GRAPHICSENGINE_GL_WORLDLOADER_H
#define GRAPHICSENGINE_GL_WORLDLOADER_H
#include <vector>
#include "Object/Object.h"


class WorldLoader {
public:
    std::vector<Object*> GetWorldObjects(std::string _fileName);
    bool SaveWorld(std::vector<Object*> _objects, std::string _fileName);
};


#endif
