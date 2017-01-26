#pragma once

class Engine;

class InputManager 
{
    friend class Engine;

    public:
        void setKey(int key, int action);
        bool checkKey(int key);

    protected:
        InputManager();
        ~InputManager();

    private:
        bool _keys[1024];
        
};