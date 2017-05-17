#include "Server.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------------
//Player collision

bool Server::WallCheckBottom(std::shared_ptr<Player> player){
	for (int i = 0; i < Map->walls.size(); i++)
	{
		if (player->bottom() > Map->walls[i]->top()-3 && player->left() < Map->walls[i]->right() && player->right() > Map->walls[i]->left() && player->top() < Map->walls[i]->top())
		{
			return true;
		}
	}
	return false;
}

bool Server::WallCheckUp(std::shared_ptr<Player> player)
{
	for (int i = 0; i < Map->walls.size(); i++)
	{
		if (player->top() < Map->walls[i]->bottom()+3 && player->left() < Map->walls[i]->right() && player->right()  > Map->walls[i]->left() && player->bottom() >= Map->walls[i]->bottom())
		{
			return true;
		}
	}
	return false;
}

bool Server::WallCheckLeft(std::shared_ptr<Player> player)
{
	for (int i = 0; i < Map->walls.size(); i++)
	{	
		if (player->left() < Map->walls[i]->right()+ 3 && player->top() < Map->walls[i]->bottom() && player->bottom()  > Map->walls[i]->top() && player->right() >= Map->walls[i]->right())
		{
			return true;
		}
		
	}
	return false;
}

bool Server::WallCheckRight(std::shared_ptr<Player> player)
{
	for (int i = 0; i < Map->walls.size(); i++)
	{
		if (player->right() > Map->walls[i]->left() - 3 && player->top() < Map->walls[i]->bottom() && player->bottom()  > Map->walls[i]->top() && player->left() < Map->walls[i]->left())
		{
			return true;
		}
		
	}
	return false;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------
// Bullet collision

bool Server::BulletCheckBottom(std::shared_ptr<Player> player) {
	for (int i = 0; i < Bullets.size(); i++)
	{
		if (player->bottom() > Bullets[i]->top() - 3 && player->left() < Bullets[i]->right() && player->right() > Bullets[i]->left() && player->top() < Bullets[i]->top())
		{
			return true;
		}
	}
	return false;
}

bool Server::BulletCheckUp(std::shared_ptr<Player> player)
{
	for (int i = 0; i < Bullets.size(); i++)
	{
		if (player->top() < Bullets[i]->bottom() + 3 && player->left() < Bullets[i]->right() && player->right()  > Bullets[i]->left() && player->bottom() >= Bullets[i]->bottom())
		{
			return true;
		}
	}
	return false;
}

bool Server::BulletCheckLeft(std::shared_ptr<Player> player)
{
	for (int i = 0; i < Bullets.size(); i++)
	{
		if (player->left() < Bullets[i]->right() + 3 && player->top() < Bullets[i]->bottom() && player->bottom()  > Bullets[i]->top() && player->right() >= Bullets[i]->right())
		{
			return true;
		}

	}
	return false;
}

bool Server::BulletCheckRight(std::shared_ptr<Player> player)
{
	for (int i = 0; i < Bullets.size(); i++)
	{
		if (player->right() > Bullets[i]->left() - 3 && player->top() < Bullets[i]->bottom() && player->bottom()  > Bullets[i]->top() && player->left() < Bullets[i]->left())
		{
			return true;
		}

	}
	return false;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// This is wall checking for bullets

bool Server::WallCheckBottom(std::shared_ptr<Entity>  player) {
	for (int i = 0; i < Map->walls.size(); i++)
	{
		if (player->bottom() > Map->walls[i]->top() - 1  && player->left() < Map->walls[i]->right() && player->right() > Map->walls[i]->left() && player->top() < Map->walls[i]->top())
		{
			return true;
		}
	}
	return false;
}

bool Server::WallCheckUp(std::shared_ptr<Entity>  player)
{
	for (int i = 0; i < Map->walls.size(); i++)
	{
		if (player->top() < Map->walls[i]->bottom() + 1 && player->left() < Map->walls[i]->right() && player->right()  > Map->walls[i]->left() && player->bottom() >= Map->walls[i]->bottom())
		{
			return true;
		}
	}
	return false;
}

bool Server::WallCheckLeft(std::shared_ptr<Entity>  player)
{
	for (int i = 0; i < Map->walls.size(); i++)
	{
		if (player->left() < Map->walls[i]->right() + 1 && player->top() < Map->walls[i]->bottom() && player->bottom()  > Map->walls[i]->top() && player->right() >= Map->walls[i]->right())
		{
			return true;
		}

	}
	return false;
}

bool Server::WallCheckRight(std::shared_ptr<Entity>  player)
{
	for (int i = 0; i < Map->walls.size(); i++)
	{
		if (player->right() > Map->walls[i]->left() - 1 && player->top() < Map->walls[i]->bottom() && player->bottom()  > Map->walls[i]->top() && player->left() < Map->walls[i]->left())
		{
			return true;
		}

	}
	return false;
}
