#pragma once
#include "Pacqets.h"
#include "sfml/System.hpp"
#include "sfml/Graphics.hpp"

#define MAX_TTL 10

class EntityBase : public sf::Drawable
{
private:
	byte id;
	sf::Vector2f position;
	byte direction;
	byte ttl = MAX_TTL;
public:
	virtual byte getId() const { return this->id; }
	virtual void setId(byte id) { this->id = id; };
	virtual sf::Vector2f getPosition() const { return this->position; }
	virtual void setPosition(word x, word y) { this->position = sf::Vector2f(x, y); }
	virtual byte getDirection() const { return this->direction; }
	virtual void setDirection(byte dir) { this->direction = dir; }
	virtual byte getTTL() const { return this->ttl; }
	virtual void resetTTL() { this->ttl = MAX_TTL; }
	virtual void setTTL(byte value) { this->ttl = value; }
	virtual void decrementTTL() { this->ttl--; }
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {}
	virtual byte getType() = 0;
};

