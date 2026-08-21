#ifndef GRAPHICSENGINE_GL_WORLDLOADER_H
#define GRAPHICSENGINE_GL_WORLDLOADER_H
#include <vector>
#include "Object/Object.h"
#include "Object/GameObject/GameObject.h"


class WorldLoader {
public:
    std::vector<GameObject*> GetWorldObjects(std::string _fileName);
    bool SaveWorld(std::vector<GameObject*> _objects, std::string _fileName);
};


#endif
