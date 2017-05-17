#include "Explosion.h"
#include <iostream>


Explosion::Explosion(const char * tex_1, const char* tex_2, byte id, word x, word y)
{
	if (!this->tex_1.loadFromFile(tex_1)) {
		std::cout << "ERROR!" << std::endl;
	}
	if (!this->tex_2.loadFromFile(tex_2)) {
		std::cout << "ERROR!" << std::endl;
	}
	sprite_1.setTexture(this->tex_1, true);
	sprite_1.setOrigin(this->tex_1.getSize().x / 2, this->tex_1.getSize().y / 2);
	sprite_2.setTexture(this->tex_2, true);
	sprite_2.setOrigin(this->tex_2.getSize().x / 2, this->tex_2.getSize().y / 2);

	this->setTTL(120);

	setId(id);
	setPosition(x, y);
}


void Explosion::setPosition(word x, word y)
{
	EntityBase::setPosition(x + this->tex_1.getSize().x / 2, y + this->tex_2.getSize().y / 2);
	sprite_1.setPosition(this->getPosition());
	sprite_2.setPosition(this->getPosition());
}