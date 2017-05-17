#include "Tanq.h"
#include <iostream>

const sf::Color Tanq::colors[8]{ sf::Color(0xFF0000FF), sf::Color(0x0000FFFF), sf::Color(0xFFFF00FF), sf::Color(0x00FF00FF),
						         sf::Color(0xFF8000FF), sf::Color(0xFF00FFFF), sf::Color(0xFFFFFFFF), sf::Color(0xD0A070FF) };
int Tanq::color_idx = 0;

Tanq::Tanq(const char* tex, byte id, byte dir, word x, word y)
{
	if (!this->tex.loadFromFile(tex)) {
		std::cout << "ERROR!" << std::endl;
	}
	sprite.setTexture(this->tex, true);
	sprite.setOrigin(this->tex.getSize().x / 2, this->tex.getSize().y / 2);

	sprite.setColor(Tanq::colors[Tanq::color_idx++]);
	color_idx %= 8;

	setId(id);
	setDirection(dir);
	setPosition(x, y);
}


void Tanq::setPosition(word x, word y)
{
	EntityBase::setPosition(x + this->tex.getSize().x / 2, y + this->tex.getSize().y / 2);
	sprite.setPosition(this->getPosition());
}

void Tanq::setDirection(byte direction)
{
	EntityBase::setDirection(direction);
	sprite.setRotation(90 * direction);
}
