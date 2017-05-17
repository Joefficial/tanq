#include "Bullet.h"

Bullet::Bullet(const char * tex, byte id, word x, word y)
{
	if (!this->tex.loadFromFile(tex)) {
		std::cout << "ERROR!" << std::endl;
	}
	sprite.setTexture(this->tex, true);
	sprite.setOrigin(this->tex.getSize().x / 2, this->tex.getSize().y / 2);

	setId(id);
	setPosition(x, y);
}


void Bullet::setPosition(word x, word y)
{
	EntityBase::setPosition(x + this->tex.getSize().x / 2, y + this->tex.getSize().y / 2);
	sprite.setPosition(this->getPosition());
}