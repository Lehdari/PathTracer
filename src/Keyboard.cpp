#include "Keyboard.hpp"


void Keyboard::keyPressed(sf::Keyboard::Key key) {
    keys_[key] = true;
    if (keyPressedFunctions_.find(key) != keyPressedFunctions_.end())
        keyPressedFunctions_[key]();
}

void Keyboard::keyReleased(sf::Keyboard::Key key) {
    keys_[key] = false;
}

void Keyboard::setKeyPressedFunction(sf::Keyboard::Key key, std::function<void()> function) {
    keyPressedFunctions_[key] = function;
}

void Keyboard::setKeyDownFunction(sf::Keyboard::Key key, std::function<void()> function) {
    keyDownFunctions_[key] = function;
}

bool Keyboard::isKeyDown(sf::Keyboard::Key key) const {
    return keys_.at(key);
}

void Keyboard::callKeyDownFunctions(void) {
    for (auto& f : keyDownFunctions_)
        if (keys_[f.first])
            f.second();
}
