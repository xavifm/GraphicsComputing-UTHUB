#include "Component.h"
#include <iomanip>

struct Reader
{
    std::istream& file;

    template<class T>
    void operator()(const char* name, T& value)
    {
        file >> value;
    }

    void operator()(const char* name, std::string& value)
    {
        file >> std::quoted(value);
    }
};

template<class T>
class SerializableComponent : public Component
{
public:
    void Load(std::istream& file) override
    {
        Reader reader{file};
        static_cast<T&>(*this).Fields(reader);
    }
};

void Component::Update()
{
}

void Component::Destroy()
{
}

void Component::SetOwner(Object *newOwner)
{
    this->owner = newOwner;
}

Object * Component::GetOwner() const
{
    return this->owner;
}
