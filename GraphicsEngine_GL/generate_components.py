import json
import re
import sys
from pathlib import Path


def generate(project_path: Path, output_path: Path):
    project_path = project_path.resolve()
    output_path = output_path.resolve()

    # Llegeix l'engine.projectfile.
    project = json.loads(project_path.read_text(encoding="utf-8"))
    components = project["components"]

    includes = []
    registrations = []
    registered_names = set()

    for component in components:
        class_name = component["class"]
        registry_name = component["name"]
        header = component["header"]

        # Accepta Model, Game::Model, etc.
        if not re.fullmatch(
            r"[A-Za-z_][A-Za-z0-9_]*(?:::[A-Za-z_][A-Za-z0-9_]*)*",
            class_name
        ):
            raise ValueError(f"Nom de classe no vàlid: {class_name}")

        if not re.fullmatch(
            r"[A-Za-z_][A-Za-z0-9_.:]*",
            registry_name
        ):
            raise ValueError(f"Nom de registre no vàlid: {registry_name}")

        if registry_name in registered_names:
            raise ValueError(f"Component duplicat: {registry_name}")

        # Les rutes del JSON han d'utilitzar '/'.
        if not header or any(char in header for char in '"\n\r\\'):
            raise ValueError(f"Ruta de header no vàlida: {header}")

        header_path = project_path.parent / header

        if not header_path.is_file():
            raise FileNotFoundError(f"No existeix: {header_path}")

        registered_names.add(registry_name)

        include = f'#include "{header}"'

        if include not in includes:
            includes.append(include)

        registrations.append(
            f'    ComponentRegistry::Register<{class_name}>'
            f'("{registry_name}");'
        )

    # Construeix el codi C++.
    code = "\n".join([
        "// Generat automàticament. No editar.",
        '#include "RegisterComponents.h"',
        '#include "Component/ComponentRegistry.h"',
        "",
        *includes,
        "",
        "void RegisterProjectComponents()",
        "{",
        *registrations,
        "}",
        "",
    ])

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(code, encoding="utf-8")

    print(f"[Generator] Creat: {output_path}")
    print(f"[Generator] Components registrats: {len(components)}")


def main():
    if len(sys.argv) != 3:
        print(
            "Ús: python generate_components.py "
            "<engine.projectfile> <RegisterComponents.cpp>",
            file=sys.stderr
        )
        return 1

    try:
        generate(Path(sys.argv[1]), Path(sys.argv[2]))
    except (OSError, ValueError, KeyError, TypeError) as error:
        print(f"[Generator] Error: {error}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())