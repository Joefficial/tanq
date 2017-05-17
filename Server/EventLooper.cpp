#include "Server.h"
#include <mutex>

int main()
{

	auto myMap = new Mapping();
	auto myServer = new Server();
	myServer->Map = myMap;

	myServer->Map->InitMap();
	myServer->Map->ConstructMap(myMap->BuildPhysicalMap("map.pbm"));

	myServer->StartNetworkThread();

	while (!myServer->IsDead)
	{
		int player_iter = 0;
		int bullet_iter = 0;
		// Player Drawer
		{
			std::lock_guard<std::mutex> lock(myServer->mtx);
			for (std::shared_ptr<Player>  p : myServer->Players)
			{
				if (!p->IsDead)
				{
					p->IsCollidedLeft = myServer->WallCheckLeft(p);
					p->IsCollidedRight = myServer->WallCheckRight(p);
					p->IsCollidedNorth = myServer->WallCheckUp(p);
					p->IsCollidedSouth = myServer->WallCheckBottom(p);

					p->IsDead = myServer->BulletCheckLeft(p);
					p->IsDead = myServer->BulletCheckRight(p);
					p->IsDead = myServer->BulletCheckUp(p);
					p->IsDead = myServer->BulletCheckBottom(p);

					if (p->IsDead)
						myServer->Network->BroadcastDeath(p->ID);
				}


				for each (std::shared_ptr<Player> var in myServer->Players)
				{
					if (!var->IsDead) {
						myServer->Network->SendUpdate(Networking::TANK, var->_x_coordinate, var->_y_coordinate, var->dir, p->ID, var->ID, myServer->Players.size(), player_iter);
					}
				}

				for each (std::shared_ptr<Entity> var in myServer->Bullets)
				{
					var->isCollidedLeft = myServer->WallCheckLeft(var);
					var->isCollidedRight = myServer->WallCheckRight(var);
					var->isCollidedTop = myServer->WallCheckUp(var);
					var->isCollidedBottom = myServer->WallCheckBottom(var);

					// Bullet Despawner
					if (var->left() > TOTAL_SIZE || var->right() < 0 || var->top() > TOTAL_SIZE || var->bottom() < 0) {
						for (int i = 0; i < myServer->Bullets.size(); i++)
						{
							if (myServer->Bullets[i]->_id == var->_id)
							{
								myServer->BulletToRemove.push_back(var);
								break;
							}
						}
					}
					else if (!var->isCollidedRight && !var->isCollidedLeft && !var->isCollidedBottom && !var->isCollidedTop)
					{
						myServer->Network->SendUpdate(Networking::BULLET, var->_x_coordinate, var->_y_coordinate, var->dir, p->ID, var->_id, myServer->Bullets.size(), bullet_iter);
					}
					else
					{
						for (int i = 0; i < myServer->Bullets.size(); i++)
						{
							if (myServer->Bullets[i]->_id == var->_id)
							{
								myServer->BulletToRemove.push_back(var);
							}
						}
					}
					bullet_iter++;
				}

				player_iter++;
			}
		}

		// Wall Drawer
		int iter = 0;
		{
			std::lock_guard<std::mutex> lock(myServer->mtx);
			for (Entity* w : myServer->Map->walls)
			{
				for (std::shared_ptr<Player> p : myServer->Players)
				{
					myServer->Network->SendUpdate(Networking::WALL, w->_x_coordinate, w->_y_coordinate, Networking::UP, p->ID, w->_id, myServer->Map->walls.size(), iter);
				}
				iter++;
			}
		}

		// Updater
		{
			std::lock_guard<std::mutex> lock(myServer->mtx);
			for (std::shared_ptr<Entity>  b : myServer->Bullets)
			{
				switch (b->dir)
				{
				case Networking::UP:
					if (!b->isCollidedTop) {
						b->_y_coordinate -= 7;
					}
					break;
				case Networking::DOWN:
					if (!b->isCollidedBottom) {
						b->_y_coordinate += 7;
					}
					break;
				case Networking::LEFT:
					if (!b->isCollidedLeft) {
						b->_x_coordinate -= 7;
					}
					break;
				case Networking::RIGHT:
					if (!b->isCollidedRight) {
						b->_x_coordinate += 7;
					}
					break;
				}
			}
		}

		myServer->RemoveBullets();
		myServer->RemovePlayers();

		SleepEx(50, false);
	}

	delete myServer;
	delete myMap;
	return 0;
}
