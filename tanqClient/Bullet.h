#pragma once
#include "EntityBase.h"
#include <iostream>

class Bullet :
	public EntityBase
{
private:
	sf::Texture tex;
	sf::Sprite sprite;
public:
	Bullet(const char* tex, byte id, word x, word y);

	void setPosition(word x, word y) override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		target.draw(sprite, states);
	}
	byte getType() override { return Networking::ENTITY_TYPES::BULLET;  }
};

