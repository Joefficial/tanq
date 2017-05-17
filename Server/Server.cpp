#include "Server.h"
#include <mutex>

void Server::UserConnectedCallback(uchar ID)
{

	std::lock_guard<std::mutex> lock(mtx);
	if (Players.size() < 4)
	{
		if (Players.size() % 2 == 0) {
			std::shared_ptr<Player> newPlayer = std::make_shared<Player>(ID , TOTAL_SIZE-75, TOTAL_SIZE-75);
			Players.push_back(newPlayer);
			return;
		}
		std::shared_ptr<Player> newPlayer = std::make_shared<Player>(ID);
		Players.push_back(newPlayer);
	}
}

std::shared_ptr<Player> Server::getPlayer(uchar ID)
{
	for (auto p : Players)
	{
		if (p->ID == ID)
		{
			return p;
		}
	}
}

void Server::UserInputCallback(Networking::KeyPress keyPress, uchar ID)
{
	std::lock_guard<std::mutex> lock(mtx);
	auto player = getPlayer(ID);

	std::shared_ptr<Entity> bullet_up = nullptr;
	std::shared_ptr<Entity> bullet_down = nullptr;
	std::shared_ptr<Entity> bullet_left = nullptr;
	std::shared_ptr<Entity> bullet_right = nullptr;

	switch (keyPress.input)
	{
	case Networking::UP:
		if (!player->IsCollidedNorth && !player->IsDead)
		{
			player->_y_coordinate -= PLAYER_SPEED;
			player->dir = Networking::UP;
		}
		break;
	case Networking::DOWN:
		if (!player->IsCollidedSouth && !player->IsDead)
		{
			player->_y_coordinate += PLAYER_SPEED;
			player->dir = Networking::DOWN;
		}
		break;
	case Networking::LEFT:
		if (!player->IsCollidedLeft && !player->IsDead)
		{
			player->_x_coordinate -= PLAYER_SPEED;
			player->dir = Networking::RIGHT;
		}
		break;
	case Networking::RIGHT:
		if (!player->IsCollidedRight && !player->IsDead)
		{
			player->_x_coordinate += PLAYER_SPEED;
			player->dir = Networking::LEFT;
		}
		break;
	case Networking::FIRE:
		if (Bullets.size() < 51 && !player->IsDead) {
			switch (player->dir)
			{
			case Networking::UP:
				bullet_up = std::make_shared<Entity>(player->_x_coordinate + 14, player->_y_coordinate - 4, 4, 4, Networking::UP, CAP_WALLS + 1 + Bullets.size());
				Bullets.push_back(bullet_up);
				break;
			case Networking::DOWN:
				bullet_down = std::make_shared<Entity>(player->_x_coordinate + 14, player->_y_coordinate + GRID_SIZE + 4, 4, 4, Networking::DOWN, CAP_WALLS + 1 + Bullets.size());
				Bullets.push_back(bullet_down);
				break;
			case Networking::RIGHT:
				bullet_left = std::make_shared<Entity>(player->_x_coordinate - 4, player->_y_coordinate + 14, 4, 4, Networking::LEFT, CAP_WALLS + 1 + Bullets.size());
				Bullets.push_back(bullet_left);
				break;
			case Networking::LEFT:
				bullet_right = std::make_shared<Entity>(player->_x_coordinate + GRID_SIZE + 4, player->_y_coordinate + 14, 4, 4, Networking::RIGHT, CAP_WALLS + 1 + Bullets.size());
				Bullets.push_back(bullet_right);
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
}

void Server::SomeoneLeftCallback(uchar ID)
{
	std::lock_guard<std::mutex> lock(mtx);
	for (int i = 0; i < Players.size(); i++)
	{
		if (Players[i]->ID == ID)
		{
			PlayerToRemove.push_back(Players[i]);
		}
	}
}

void Server::StartNetworkThread()
{
	ServerThread = std::thread(&Server::RunThread, this);
}

void Server::RunThread()
{
	Online = true;
	Network->loop();
}

void Server::CommitSuicide()
{
	IsDead = true;
	// TODO: Tell Joe to die
}

void Server::RemoveBullets()
{
	std::lock_guard<std::mutex> lock(mtx);
	for (std::shared_ptr<Entity> rem : BulletToRemove) {
		Bullets.erase(std::remove(Bullets.begin(), Bullets.end(), rem), Bullets.end());
		//BulletToRemove.erase(std::remove(BulletToRemove.begin(), BulletToRemove.end(), rem), BulletToRemove.end());
	}
}

void Server::RemovePlayers()
{
	std::lock_guard<std::mutex> lock(mtx);
	for (std::shared_ptr<Player> rem : PlayerToRemove) {
		Players.erase(std::remove(Players.begin(), Players.end(), rem), Players.end());
		// PlayerToRemove.erase(std::remove(PlayerToRemove.begin(), PlayerToRemove.end(), rem), PlayerToRemove.end());
	}
}


