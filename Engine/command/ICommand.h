#pragma once

class ICommand
{
public:
    virtual ~ICommand();
    virtual void Exec() = 0;
};

class MoveUpCommand : public ICommand {
public:
    void Exec() override;
};

class MoveDownCommand : public ICommand {
public:
    void Exec() override;
};

class MoveLeftCommand : public ICommand {
public:
    void Exec() override;
};

class MoveRightCommand : public ICommand {
public:
    void Exec() override;
};
