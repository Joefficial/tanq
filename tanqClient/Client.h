#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <cmath>
#include "ClientInterface.h"
#include <thread>
#include "IClient.h"
#include "EntityBase.h"
#include "Tanq.h"
#include <mutex>

struct Resources {
	sf::Music chime;
	sf::Music musc_menu;
	sf::Music musc_main;
	sf::Music musc_main_loop;

	sf::SoundBuffer shootbuff;
	sf::Sound shoot;

	sf::Texture splash_spbp;
	sf::Texture menu_bg;
	sf::Texture tanq_tx;
	sf::Texture game_bg;
};

class Client : public Networking::IClient
{
private:
	std::thread networkingThread;
	Networking::ClientInterface* connection;
	std::vector<std::shared_ptr<EntityBase>> entities;
	std::map<int, Networking::EntityUpdate> entities_to_create;
	Resources res;

	std::mutex mtx;

	bool connected = false;

	sf::Vector2f getEntityPosition(int id);
	void causeExplosion(int id);
public:
	Client();
	~Client();

	int loop();
	bool loadResources(Resources& resources);
	void spbpSplash(Resources& resources, sf::RenderWindow& window);
	void menuScreen(Resources& resources, sf::RenderWindow& window);
	void launchConnectionThread();
	void connectToServer();
	std::shared_ptr<EntityBase> createEntity(Networking::EntityUpdate info);
	virtual void EntityUpdateCallback(Networking::EntityUpdate info);
	virtual void DeathCallback(Networking::Death info);
	virtual void KillCallback(Networking::Kill info);
};

