#pragma once
#include "EntityBase.h"
#include "sfml/Graphics.hpp"
class Tanq :
	public EntityBase
{
private:
	static const sf::Color colors[8];
	static int color_idx;

	sf::Sprite sprite;
	sf::Texture tex;
public:
	Tanq(const char* tex, byte id, byte dir, word x, word y);

	void setPosition(word x, word y) override;
	void setDirection(byte direction) override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override { target.draw(sprite, states); }
	byte getType() override { return Networking::ENTITY_TYPES::TANK; }
};

