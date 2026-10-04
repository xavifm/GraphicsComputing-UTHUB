#pragma once
#include "Component.h"
#include <iomanip>
#include <string>

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

struct Writer
{
    std::ostream& file;

    template<class T>
    void operator()(const char*, const T& value)
    {
        file << value << "\n";
    }

    void operator()(const char*, const std::string& value)
    {
        file << std::quoted(value) << "\n";
    }
};

template<class T>
class SerializableComponent : public Component
{
public:
    void Load(std::istream& file)
    {
        Reader reader{file};
        static_cast<T&>(*this).Fields(reader);
    }
    void Save(std::ostream& file) const override
    {
        Writer writer{file};
        static_cast<const T&>(*this).Fields(writer);
    }
};