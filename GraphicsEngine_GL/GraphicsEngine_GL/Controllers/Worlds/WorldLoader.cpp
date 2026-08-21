#include "WorldLoader.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "Model.h"
#include "Object/Object.h"
#include "Object/GameObject//GameObject.h"
#include "Model/Model.h"


std::vector<GameObject*> WorldLoader::GetWorldObjects(std::string _fileName)
{
    std::vector<GameObject*> objects;

    std::ifstream file(_fileName);

    if (!file.is_open())
    {
        std::cerr << "WorldLoader: Could not open the file: "
                  << _fileName << std::endl;

        return objects;
    }

    std::string token;

    while (file >> token)
    {
        if (token != "OBJECT")
            continue;

        std::string objectType;
        file >> objectType;

        Object* object = nullptr;

        // ---------------------------------------------------------
        // GAME OBJECT
        // ---------------------------------------------------------

        if (objectType == "GAMEOBJECT")
        {
            Vector3D position;
            Vector3D rotation;
            Vector3D size;

            position = { 0.0f, 0.0f, 0.0f };
            rotation = { 0.0f, 0.0f, 0.0f };
            size     = { 1.0f, 1.0f, 1.0f };

            GameObject* gameObject = new GameObject(position, size);

            while (file >> token)
            {
                if (token == "NAME")
                {
                    std::string name;
                    file >> std::quoted(name);

                    gameObject->SetName(name);
                }

                else if (token == "POSITION")
                {
                    file >> gameObject->position.x
                         >> gameObject->position.y
                         >> gameObject->position.z;
                }

                else if (token == "ROTATION")
                {
                    file >> gameObject->rotation.x
                         >> gameObject->rotation.y
                         >> gameObject->rotation.z;
                }

                else if (token == "SIZE")
                {
                    file >> gameObject->size.x
                         >> gameObject->size.y
                         >> gameObject->size.z;
                }

                // -------------------------------------------------
                // MODEL COMPONENT
                // -------------------------------------------------

                else if (token == "MODEL")
                {
                    Model* model = gameObject->AddComponent<Model>();

                    Vector3D modelPosition = { 0.0f, 0.0f, 0.0f };
                    Vector3D modelScale    = { 1.0f, 1.0f, 1.0f };

                    while (file >> token)
                    {
                        if (token == "MODEL_POSITION")
                        {
                            file >> modelPosition.x
                                 >> modelPosition.y
                                 >> modelPosition.z;
                        }

                        else if (token == "MODEL_SCALE")
                        {
                            file >> modelScale.x
                                 >> modelScale.y
                                 >> modelScale.z;
                        }

                        else if (token == "END_MODEL")
                        {
                            break;
                        }
                    }

                    //model->SetPosition(modelPosition);
                    //model->SetScale(modelScale);
                }

                else if (token == "END_OBJECT")
                {
                    objects.push_back(gameObject);
                    break;
                }
            }

            object = gameObject;
        }
    }

    file.close();

    return objects;
}


bool WorldLoader::SaveWorld(
    std::vector<GameObject*> _objects,
    std::string _fileName
)
{
    std::ofstream file(_fileName);

    if (!file.is_open())
    {
        std::cerr << "WorldLoader: Could not create the file: "
                  << _fileName << std::endl;

        return false;
    }

    for (Object* object : _objects)
    {
        if (object == nullptr)
            continue;

        // ---------------------------------------------------------
        // GAME OBJECT
        // ---------------------------------------------------------

        if (GameObject* gameObject = dynamic_cast<GameObject*>(object))
        {
            file << "OBJECT GAMEOBJECT\n";

            file << "NAME "
                 << std::quoted(gameObject->GetName())
                 << "\n";

            file << "POSITION "
                 << gameObject->position.x << " "
                 << gameObject->position.y << " "
                 << gameObject->position.z << "\n";

            file << "ROTATION "
                 << gameObject->rotation.x << " "
                 << gameObject->rotation.y << " "
                 << gameObject->rotation.z << "\n";

            file << "SIZE "
                 << gameObject->size.x << " "
                 << gameObject->size.y << " "
                 << gameObject->size.z << "\n";


            // -----------------------------------------------------
            // COMPONENTS
            // -----------------------------------------------------

            for (Component* component :
                 gameObject->GetFullComponentsList())
            {
                if (component == nullptr)
                    continue;

                // MODEL
                if (Model* model = dynamic_cast<Model*>(component))
                {
                    file << "MODEL\n";

                    Vector3D modelPosition = gameObject->position;
                    Vector3D modelScale    = gameObject->size;

                    file << "MODEL_POSITION "
                         << modelPosition.x << " "
                         << modelPosition.y << " "
                         << modelPosition.z << "\n";

                    file << "MODEL_SCALE "
                         << modelScale.x << " "
                         << modelScale.y << " "
                         << modelScale.z << "\n";

                    file << "END_MODEL\n";
                }
            }

            file << "END_OBJECT\n\n";
        }
    }

    file.close();

    return true;
}