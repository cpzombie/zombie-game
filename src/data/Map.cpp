#include "data/Map.h"

#include <string>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include "util/Logger.h"
#include "entities/enemies/EnemyType.h"
#include "data/Settings.h"
#include "data/SaveData.h"

//Helper function, fills the given vector with textures from the files in the given directory
bool texturesFromDir(std::vector<sf::Texture>& ret, std::string path) {
	for (auto& file : std::filesystem::directory_iterator(path)) {
		sf::Texture t;
		if (t.loadFromFile(file.path().string())) ret.push_back(t);
		else Logger::log("NOTICE: Failed to load texture in dir " + path + ": " + file.path().string());
	}
	if (ret.size()) return true;
	else return false;
}

//Loads the textures, logs an error / uses default if missing, and throws a string if default is missing too
Map::Map(sf::RenderWindow* win, Settings* settings, int w, int h, std::string* back_path, std::vector<int>* e_counts, std::vector<std::vector<int>>* spawn_locs,
	std::vector<std::vector<std::string>>* proj_paths, std::vector<std::vector<std::string>>* enemy_paths) : window(win) {

	if (w) width = w;
	else width = 1600;

	if (h) height = h;
	else height = 900;

	if (!back_path || !background_texture.loadFromFile(*back_path)) {
		if (back_path) Logger::log(std::string("ERROR: Background texture not found: " + *back_path));
		if (!background_texture.loadFromFile(settings->getDefaultBackgroundTexturePath())) {
			Logger::log(std::string("FATAL ERROR: Default background texture not found!"));
			throw (std::string("Default background texture not found, please reinstall the program."));
		}
	}

	//Default enemy counts: 20 for each
	if (e_counts) enemy_counts = std::move(*e_counts);
	else {
		for (int i = 0; i < SaveData::ENEMY_COUNT; i++) enemy_counts.push_back(20);
	}

	//Default spawn locations: surrounding the map
	if (spawn_locs){
		for (auto& v : *spawn_locs) {
			if (v.size() == 4) spawn_locations.push_back(sf::IntRect(v[0], v[1], v[2], v[3]));
			else Logger::log("ERROR: Invalid number of sides for spawn location");
		}
	}
	else {
		spawn_locations.push_back(sf::IntRect(-100, -100, 100, height + 200));
		spawn_locations.push_back(sf::IntRect(width, -100, 100, height + 200));
		spawn_locations.push_back(sf::IntRect(0, -100, width, 100));
		spawn_locations.push_back(sf::IntRect(0, height, width, 100));
	}

	if (proj_paths) {
		projectile_textures.resize(SaveData::PROJECTILE_COUNT);
		for (int i = 0; i < proj_paths[i].size(); i++) {

			//Checks if map only specifies some, default is signalled with empty
			if (proj_paths[i].size()) {
				projectile_textures[i].resize(proj_paths[i].size());
				for (int j = 0; j < proj_paths[i].size(); j++) {
					if (!projectile_textures[i][j].loadFromFile((*proj_paths)[i][j])) {
						Logger::log(std::string("ERROR: Projectile texture not found: " + (*proj_paths)[i][j]));
						if (!projectile_textures[i][j].loadFromFile(settings->getDefaultProjectileTexturesDirs()[i] + "0.png")) {
							Logger::log(std::string("FATAL ERROR: Default projectile texture not found! Dir checked: " + settings->getDefaultProjectileTexturesDirs()[i]));
							throw (std::string("Default projectile texture not found, please reinstall the program."));
						}
					}
				}
			}
			else {
				if (!texturesFromDir(projectile_textures[i], settings->getDefaultProjectileTexturesDirs()[i])) {
					Logger::log(std::string("FATAL ERROR: Default projectile textures not found! Dir checked: " + settings->getDefaultProjectileTexturesDirs()[i]));
					throw (std::string("Default projectile textures not found, please reinstall the program."));
				}
			}
		}
	}
	else {
		projectile_textures.resize(SaveData::PROJECTILE_COUNT);
		for (int i = 0; i < SaveData::PROJECTILE_COUNT; i++) {
			if (!texturesFromDir(projectile_textures[i], settings->getDefaultProjectileTexturesDirs()[i])) {
				Logger::log(std::string("FATAL ERROR: Default projectile textures not found! Dir checked: " + settings->getDefaultProjectileTexturesDirs()[i]));
				throw (std::string("Default projectile texture not found, please reinstall the program."));
			}
		}
	}

	if (enemy_paths) {
		enemy_textures.resize(SaveData::ENEMY_COUNT);
		for (int i = 0; i < enemy_paths->size(); i++) {
			if (enemy_paths[i].size()) {
				enemy_textures[i].resize(enemy_paths[i].size());
				for (int j = 0; j < enemy_paths[i].size(); j++) {
					if (!enemy_textures[i][j].loadFromFile((*enemy_paths)[i][j])) {
						Logger::log(std::string("ERROR: Enemy texture not found: " + (*enemy_paths)[i][j]));
						if (!enemy_textures[i][j].loadFromFile(settings->getDefaultEnemyTexturesDirs()[i] + "0.png")) {
							Logger::log(std::string("FATAL ERROR: Default enemy texture not found! Dir checked: " + settings->getDefaultEnemyTexturesDirs()[i]));
							throw (std::string("Default enemy texture not found, please reinstall the program."));
						}
					}
				}
			}
			else {
				if (!texturesFromDir(enemy_textures[i], settings->getDefaultEnemyTexturesDirs()[i])) {
					Logger::log(std::string("FATAL ERROR: Default enemy textures not found! Dir checked: " + settings->getDefaultEnemyTexturesDirs()[i]));
					throw (std::string("Default enemy texture not found, please reinstall the program."));
				}
			}
		}
	}
	else {
		enemy_textures.resize(SaveData::ENEMY_COUNT);
		for (int i = 0; i < SaveData::ENEMY_COUNT; i++) {
			if (!texturesFromDir(enemy_textures[i], settings->getDefaultEnemyTexturesDirs()[i])) {
				Logger::log(std::string("FATAL ERROR: Default enemy textures not found! Dir checked: " + settings->getDefaultEnemyTexturesDirs()[i]));
				throw (std::string("Default enemy texture not found, please reinstall the program."));
			}
		}
	}

}