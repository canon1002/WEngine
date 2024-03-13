#pragma once
#include "IScene.h"

class StageScene :
    public IScene
{
private:
    
public:

    void Finalize()override;
    void Init()override;
    void Update()override;
    void Draw()override;
};

