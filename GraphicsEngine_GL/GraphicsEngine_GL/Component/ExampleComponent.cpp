#include "Object/GameObject/GameObject.h"
#include "ExampleComponent.h"
#include "Globals.h"

ExampleComponent::ExampleComponent()
{
}



ExampleComponent::~ExampleComponent()
{
}

void ExampleComponent::Update()
{
    SerializableComponent<ExampleComponent>::Update();
}
