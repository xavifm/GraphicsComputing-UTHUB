#include <functional>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <typeindex>

#include "ImGuiFieldVisitor.h"
#include "Object/GameObject/GameObject.h"

class ComponentRegistry
{
public:
    using Factory = std::function<Component*(GameObject&)>;
    using FieldDrawer = std::function<void(Component&, ImGuiFieldVisitor&)>;

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

        Drawers()[std::type_index(typeid(T))] = [](Component& component, ImGuiFieldVisitor& visitor)
        {
            static_cast<T&>(component).Fields(visitor);
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

    static bool VisitFields(
    Component& component,
    ImGuiFieldVisitor& visitor)
    {
        auto it = Drawers().find(
            std::type_index(typeid(component)));

        if (it == Drawers().end())
        {
            return false;
        }

        it->second(component, visitor);
        return true;
    }

private:
    static std::unordered_map<std::type_index, FieldDrawer>& Drawers()
    {
        static std::unordered_map<std::type_index, FieldDrawer> drawers;
        return drawers;
    }

};
