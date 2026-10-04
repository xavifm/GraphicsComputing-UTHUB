#include <functional>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include "Object/GameObject/GameObject.h"

class ComponentRegistry
{
public:
    using Factory = std::function<Component*(GameObject&)>;

    static std::unordered_map<std::string, Factory>& Types()
    {
        static std::unordered_map<std::string, Factory> types;
        return types;
    }

    template<class T>
    static void Register(const std::string& name)
    {
        Types()[name] = [](GameObject& object) -> Component*
        {
            return object.AddComponent<T>();
        };
    }

    static Component* Create(
        const std::string& name,
        GameObject& object)
    {
        auto it = Types().find(name);

        if (it == Types().end())
        {
            throw std::runtime_error(
                "Not registered component! " + name);
        }

        return it->second(object);
    }
};
