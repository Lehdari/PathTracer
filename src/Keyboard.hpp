#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP


#include <SFML/Window/Keyboard.hpp>
#include <map>
#include <functional>


class Keyboard {
public:
    void keyPressed(sf::Keyboard::Key key);
    void keyReleased(sf::Keyboard::Key key);

    void setKeyPressedFunction(sf::Keyboard::Key key, std::function<void()> function);
    void setKeyDownFunction(sf::Keyboard::Key key, std::function<void()> function);

    bool isKeyDown(sf::Keyboard::Key key) const;
    void callKeyDownFunctions(void);

private:
    std::map<sf::Keyboard::Key, bool> keys_;
    std::map<sf::Keyboard::Key, std::function<void()>> keyPressedFunctions_;
    std::map<sf::Keyboard::Key, std::function<void()>> keyDownFunctions_;
};


#endif // KEYBOARD_HPP
