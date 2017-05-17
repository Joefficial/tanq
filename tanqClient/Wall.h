#pragma once
#include "EntityBase.h"
#include <iostream>
class Wall :
	public EntityBase
{
private:
	sf::Sprite sprite;
	sf::Texture tex;
public:
	Wall(const char* tex, byte id, word x, word y);

	void setPosition(word x, word y) override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		target.draw(sprite, states);
	}
	byte getType() override { return Networking::ENTITY_TYPES::WALL; }
};

