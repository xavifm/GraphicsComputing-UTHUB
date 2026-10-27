#include "Controller.h"
#include "ImguiController.h"
#include <sys/wait.h>
#include <unistd.h>

class EngineController : public Controller {
public:
    EngineController(ImguiController* _imguiController);
    ~EngineController();

    bool Init();
    update_status PreUpdate();
    update_status Update();
    bool CleanUp();

    bool RestartEngine();

    std::size_t MAX_PATH_LENGTH = 4096;

private:
    ImguiController* imguiController;
};
