#pragma once
#include "../Networqing/ServerInterface.h"
#include "../Networqing/IServer.h"
#include "Player.h"
#include "Mapping.h"
#include <thread>
#include <algorithm>
#include <vector>
#include <mutex>

const int PLAYER_SPEED = 1;

class Server : Networking::IServer
{
public:
	std::mutex mtx;

	std::thread ServerThread;
	Networking::ServerInterface* Network;
	std::vector<std::shared_ptr<Player>> Players;
	std::vector<std::shared_ptr<Entity>> Bullets;

	std::vector<std::shared_ptr<Player>> PlayerToRemove;
	std::vector<std::shared_ptr<Entity>> BulletToRemove;

	Mapping* Map;

	bool IsDead = false;
	bool Online = false;

	Server()
	{
		Network = new Networking::ServerInterface();
		Network->SetServer(this);
		Players.reserve(4);
		Bullets.reserve(51);
	}

	~Server()
	{
		ServerThread.join();
		delete Network;
	}

	// Callbacks for Joe

	void UserConnectedCallback(uchar ID);
	void UserInputCallback(Networking::KeyPress input, uchar ID);
	void SomeoneLeftCallback(uchar ID);

	std::shared_ptr<Player> getPlayer(uchar ID);

	void StartNetworkThread();
	void RunThread();

	void CommitSuicide();
	void RemoveBullets();
	void RemovePlayers();

	// Runs in event loop
	bool WallCheckUp(std::shared_ptr<Player>  player);
	bool WallCheckBottom(std::shared_ptr<Player>  player);
	bool WallCheckLeft(std::shared_ptr<Player>  player);
	bool WallCheckRight(std::shared_ptr<Player>  player);

	bool BulletCheckUp(std::shared_ptr<Player>  player);
	bool BulletCheckBottom(std::shared_ptr<Player>  player);
	bool BulletCheckLeft(std::shared_ptr<Player>  player);
	bool BulletCheckRight(std::shared_ptr<Player>  player);


	bool WallCheckUp(std::shared_ptr<Entity> player);
	bool WallCheckBottom(std::shared_ptr<Entity>  player);
	bool WallCheckLeft(std::shared_ptr<Entity>  player);
	bool WallCheckRight(std::shared_ptr<Entity>  player);
};
