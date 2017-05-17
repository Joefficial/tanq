#include "Client.h"
#include "Icon.h"
#include "Wall.h"
#include "Bullet.h"
#include "Explosion.h"

Client::Client()
{
	connection = new Networking::ClientInterface();
	connection->SetClient(this);
}


Client::~Client()
{
	connection->commitSuicide();
	networkingThread.join();
	delete connection;
}

int Client::loop() {
	sf::RenderWindow window(sf::VideoMode(768, 768), "SFML window");
	window.setIcon(tanq_icon.width, tanq_icon.height, tanq_icon.pixel_data);
	window.setFramerateLimit(60);

	if (!loadResources(res)) {
		printf("One or more resources failed to load. The program will now exit.");
		_getch();

		return EXIT_FAILURE;
	}

	spbpSplash(res, window);

	if (!window.isOpen()) {
		return EXIT_SUCCESS;
	}

	menuScreen(res, window);

	res.musc_main.play();

	if (!window.isOpen()) {
		return EXIT_SUCCESS;
	}

	launchConnectionThread();

	sf::Sprite background;
	background.setTexture(res.game_bg);

	int handleCounter = 0;
	int handleModulus = 2;

	bool spaceWasDown = false;

	int bulletCountdown = 0;

	sf::Clock clock;

	// Start the game loop
	while (window.isOpen())
	{
		// Create entities
		{
			std::lock_guard<std::mutex> lock(mtx);
			for (auto entityUpdKVP : entities_to_create) {
				std::shared_ptr<EntityBase> entity = createEntity(entityUpdKVP.second);
				if (entity != nullptr) {
					entities.push_back(entity);
				}
			}
			entities_to_create.clear();
		}

		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window: exit
			if (event.type == sf::Event::Closed)
			{
				connection->LeaveGame();
				window.close();
			}
		}

		handleCounter++;

		if (handleCounter % handleModulus == 0) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
				connection->SendInput(Networking::USER_INPUT::UP);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
				connection->SendInput(Networking::USER_INPUT::LEFT);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
				connection->SendInput(Networking::USER_INPUT::DOWN);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
				connection->SendInput(Networking::USER_INPUT::RIGHT);
			}
			else if (!spaceWasDown && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
				spaceWasDown = true;
			}
			else if (spaceWasDown && !sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && bulletCountdown < 0) {
				connection->SendInput(Networking::USER_INPUT::FIRE);
				bulletCountdown = 1000;
				spaceWasDown = false;
			}
			handleCounter = 0;
		}

		// Update the music.
		if (res.musc_main.getStatus() == sf::Music::Status::Stopped &&
			res.musc_main_loop.getStatus() != sf::Music::Status::Playing) {
			res.musc_main_loop.setLoop(true);
			res.musc_main_loop.play();
		}
		// Clear screen
		window.clear(sf::Color(0xD0A050FF));
		window.draw(background);

		{
			std::vector<std::shared_ptr<EntityBase>> to_rem;
			std::lock_guard<std::mutex> lock(mtx);

			for (std::shared_ptr<EntityBase> entity : entities) {
				window.draw(*entity);
				entity->decrementTTL();
				if (entity->getTTL() == 0) {
					to_rem.push_back(entity);
				}
			}

			for (std::shared_ptr<EntityBase> rem : to_rem) {
				entities.erase(std::remove(entities.begin(), entities.end(), rem), entities.end());
			}
		}

		bulletCountdown -= clock.restart().asMilliseconds();

		// Update the window
		window.display();
	}
	return EXIT_SUCCESS;
}

bool Client::loadResources(Resources& res) {
	bool result = true;

	result &= res.chime.openFromFile("sound/chime.ogg");
	result &= res.musc_menu.openFromFile("sound/musc_menu.ogg");
	result &= res.musc_main.openFromFile("sound/musc_main.ogg");
	result &= res.musc_main_loop.openFromFile("sound/musc_main_loop.ogg");

	result &= res.shootbuff.loadFromFile("sound/shoot.wav");
	res.shoot = sf::Sound(res.shootbuff);

	result &= res.splash_spbp.loadFromFile("art/splash.png");
	result &= res.menu_bg.loadFromFile("art/menu.png");
	result &= res.game_bg.loadFromFile("art/background.png");

	return result;
}


void Client::launchConnectionThread() {
	networkingThread = std::thread(&Client::connectToServer, this);

}

void Client::connectToServer() {
	connection->ConnectToServer();
	connected = true;
}

std::shared_ptr<EntityBase> Client::createEntity(Networking::EntityUpdate info)
{
	std::shared_ptr<EntityBase> e = nullptr;
	switch (info.entity_type) {
	case Networking::ENTITY_TYPES::TANK:
		e =
			std::make_shared<Tanq>("art/tanq.png", info.id, info.dir, info.x, info.y);
		break;
	case Networking::ENTITY_TYPES::WALL:
		e =
			std::make_shared<Wall>("art/wall.png", info.id, info.x, info.y);
		break;
	case Networking::ENTITY_TYPES::BULLET:
		e =
			std::make_shared<Bullet>("art/bullet.png", info.id, info.x, info.y);
		res.shoot.play();
		break;
	}

	return e;
}

void Client::EntityUpdateCallback(Networking::EntityUpdate info)
{
	std::lock_guard<std::mutex> lock(mtx);
	bool hasEntity = false;
	std::shared_ptr<EntityBase> entity = nullptr;
	for (std::shared_ptr<EntityBase> ent : entities) {
		if (ent->getId() == info.id && ent->getType() != TYPE_EXPLOSION) {
			if (ent->getType() != info.entity_type) {
				std::cout << "Error! Differing entity types!" << std::endl;
			}
			hasEntity = true;
			entity = ent;
			break;
		}
	}

	if (!hasEntity) {
		//std::shared_ptr<EntityBase> ent = createEntity(info);
		//entities.push_back(ent);
		entities_to_create[info.id] = info;
	}
	else {
		entity->setDirection(info.dir);
		entity->setPosition(info.x, info.y);
		entity->resetTTL();
	}
}

void Client::DeathCallback(Networking::Death info)
{
	std::lock_guard<std::mutex> lock(mtx);
	causeExplosion(info.id);
}

void Client::KillCallback(Networking::Kill info)
{
	causeExplosion(info.id);
}

void Client::causeExplosion(int id) {
	sf::Vector2f pos = getEntityPosition(id);
	std::shared_ptr<EntityBase> e = std::make_shared<Explosion>("art/explode_1.png", "art/explode_2.png", -1, pos.x, pos.y);
	entities.push_back(e);
}

sf::Vector2f Client::getEntityPosition(int id) {
	for (std::shared_ptr<EntityBase> ent : entities) {
		if (ent->getId() == id) {
			return ent->getPosition();
		}
	}
}


void Client::spbpSplash(Resources& res, sf::RenderWindow& window) {
	res.chime.play();
	res.chime.setVolume(70);
	sf::Sprite spbpSplash;

	spbpSplash.setTexture(res.splash_spbp);
	sf::Vector2f splashPos(0, 768);
	spbpSplash.setPosition(splashPos);
	double delta = 0;

	sf::Clock clock;

	bool start = false;
	while (window.isOpen() && (res.chime.getStatus() == sf::SoundSource::Status::Playing || delta < 2750) && !start) {
		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window: exit
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::KeyReleased) {
				start = true;
			}
		}

		float y = spbpSplash.getPosition().y;
		if (y > 0) spbpSplash.move(0, std::max(-(float)delta / 75.0f, -y));


		// Clear screen
		window.clear();
		// Draw the texture
		window.draw(spbpSplash);

		// Update the window
		window.display();

		delta += clock.restart().asMilliseconds();
	}

	res.chime.stop();
}

void Client::menuScreen(Resources& res, sf::RenderWindow& window) {
	res.musc_menu.setLoop(true);
	res.musc_menu.play();
	sf::Sprite menuBG;
	//Tanq tanq("art/tanq.png", 0, 0, 10, 10);

	menuBG.setTexture(res.menu_bg);

	//tanq.setPosition(sf::Vector2f(10, 10));
	double deltaTime = 0;

	sf::Clock clock;

	bool loop = true;

	while (loop && window.isOpen()) {
		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window: exit
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::KeyReleased) {
				loop = false;
			}
		}

		// Clear screen
		window.clear();

		// Draw the texture
		window.draw(menuBG);
		//tanq.draw(window);

		// Update the window
		window.display();

		deltaTime += clock.restart().asMilliseconds();
	}

	res.musc_menu.stop();
	//entities.clear();
}