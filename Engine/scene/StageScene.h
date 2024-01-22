#pragma once
#include "IScene.h"
#include "../../App/command/ICommand.h"
#include "../../App/command/InputHandle.h"

#include <memory>

class StageScene :
    public IScene
{
private:
    std::unique_ptr <InputHandle> inputHandler_ = nullptr;
    ICommand* command_ = nullptr;
    
public:
    void Init()override;
    void Update()override;
    void Draw()override;
};

