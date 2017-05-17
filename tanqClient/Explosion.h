#pragma once
#include "sfml/Graphics.hpp"
#include "EntityBase.h"

const byte TYPE_EXPLOSION = 99;

class Explosion : public EntityBase
{
private:
	sf::Texture tex_1;
	sf::Sprite sprite_1;
	sf::Texture tex_2;
	sf::Sprite sprite_2;
public:
	Explosion(const char* tex_1, const char* tex_2, byte id, word x, word y);

	void setPosition(word x, word y) override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		sf::Sprite sprite = (getTTL() % 4 < 2) ? sprite_1 : sprite_2;
		target.draw(sprite, states);
	}
	byte getType() override { return TYPE_EXPLOSION;  }
};

