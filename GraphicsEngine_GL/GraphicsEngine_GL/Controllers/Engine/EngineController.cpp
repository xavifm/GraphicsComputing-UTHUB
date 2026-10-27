#include "EngineController.h"
#include "ImguiController.h"


EngineController::EngineController(ImguiController* _imguiController)
{
    imguiController = _imguiController;
}

EngineController::~EngineController()
{
}

bool EngineController::Init() {
    return Controller::Init();
}

update_status EngineController::PreUpdate()
{
    return Controller::PreUpdate();
}

update_status EngineController::Update()
{
    if (imguiController != nullptr)
    {
        if (imguiController->RestartEngine)
        {
            RestartEngine();
            imguiController->RestartEngine = false;
        }
    }

    return Controller::Update();
}

bool EngineController::CleanUp()
{
    return Controller::CleanUp();
}

bool EngineController::RestartEngine()
{
    namespace fs = std::filesystem;

    const fs::path projectDir = fs::current_path();
    const fs::path buildDir = projectDir;

    auto runCommand = [](const char* const argv[]) -> bool
    {
        pid_t pid = fork();

        if (pid < 0)
            return false;

        if (pid == 0)
        {
            execvp(argv[0], const_cast<char* const*>(argv));
            _exit(127);
        }

        int status = 0;

        if (waitpid(pid, &status, 0) < 0)
            return false;

        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    };

    const std::string project = projectDir.string() + "/../";
    const std::string build = buildDir.string();

    const char* configureArgs[] = {
        "cmake",
        "-S", project.c_str(),
        "-B", build.c_str(),
        nullptr
    };

    if (!runCommand(configureArgs))
    {
        std::cerr << "Error configurant CMake\n";
        return false;
    }

    const char* buildArgs[] = {
        "cmake",
        "--build", build.c_str(),
        "-j", "4",
        nullptr
    };

    if (!runCommand(buildArgs))
    {
        std::cerr << "Error compilant el motor\n";
        return false;
    }

    const fs::path executable = buildDir / "GraphicsEngine_GL";

    const std::string exePath = executable.string();

    char* const args[] = {
        const_cast<char*>(exePath.c_str()),
        nullptr
    };

    execv(exePath.c_str(), args);

    std::cerr << "Error reiniciant el motor\n";
    return false;
}