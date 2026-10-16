#pragma once

#include <string>
#include <vector>
#include "imgui.h"

class ImGuiFieldVisitor
{
public:
    void operator()(const char* name, std::string& value)
    {
        std::vector<char> buffer(
            std::max<std::size_t>(4096, value.size() + 1024),
            '\0'
        );

        std::copy(value.begin(), value.end(), buffer.begin());

        if (ImGui::InputText(name, buffer.data(), buffer.size()))
        {
            value.assign(buffer.data());
        }
    }

    void operator()(const char* name, float& value)
    {
        ImGui::DragFloat(name, &value, 0.05f);
    }

    void operator()(const char* name, int& value)
    {
        ImGui::InputInt(name, &value);
    }

    void operator()(const char* name, bool& value)
    {
        ImGui::Checkbox(name, &value);
    }

    template<class T>
    void operator()(const char* name, T&)
    {
        ImGui::TextDisabled("%s: tipus no suportat", name);
    }
};