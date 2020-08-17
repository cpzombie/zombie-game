#include <SFML/Graphics.hpp>
#include <string>
#include "menus/buttons/Button.h"
#include "util/Logger.h"

Button::Button(Settings* settings, sf::RenderWindow* win, std::string path, float x, float y, float w, float h) : window(win) {
	if (!texture.loadFromFile(path)) {
		Logger::logErr(std::string("ERROR: Button texture not found: " + path));
		if (!texture.loadFromFile(settings->default_button_texture_path)) {
			Logger::logErr(std::string("FATAL ERROR: Default button texture not found!"));
			throw (std::string("Default button texture not found, please reinstall the program."));
		}
	}
}