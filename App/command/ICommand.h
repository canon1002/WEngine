#pragma once
class Player;

class ICommand
{
public:
    virtual ~ICommand();
    virtual void Exec(Player& player) = 0;
};

class MoveUpCommand : public ICommand {
public:
    void Exec(Player& player) override;
};

class MoveUpLeftCommand : public ICommand {
public:
    void Exec(Player& player) override;
};

class MoveUpRightCommand : public ICommand {
public:
    void Exec(Player& player) override;
};

class MoveDownCommand : public ICommand {
public:
    void Exec(Player& player) override;
};

class MoveDownLeftCommand : public ICommand {
public:
    void Exec(Player& player) override;
};

class MoveDownRightCommand : public ICommand {
public:
    void Exec(Player& player) override;
};

class MoveLeftCommand : public ICommand {
public:
    void Exec(Player& player) override;
};

class MoveRightCommand : public ICommand {
public:
    void Exec(Player& player) override;
};
