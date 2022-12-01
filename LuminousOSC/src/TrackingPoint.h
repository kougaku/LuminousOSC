#pragma once

#include <iostream>

class TrackingPoint {

public:
	//--------------------------------------------------------------
	TrackingPoint(int x, int y, int id, int max_life) {
		this->x = x;
		this->y = y;
		this->id = id;
		this->life = max_life;
		this->max_life = max_life;
	}

	//--------------------------------------------------------------
	void updatePosition(int new_x, int new_y, int recovery_amount) {
		this->x = new_x;
		this->y = new_y;
		int new_life = this->life + recovery_amount;
		this->life = (new_life > this->max_life) ? this->max_life : new_life;
	}

	//--------------------------------------------------------------
	void reduceLife(int reducing_amount) {
		int new_life = this->life - reducing_amount;
		this->life = (new_life < 0) ? 0 : new_life;
	}

	//--------------------------------------------------------------
	double distanceFrom(int x, int y) {
		return sqrt((this->x - x) * (this->x - x) + (this->y - y) * (this->y - y));
	}

	//--------------------------------------------------------------
	int getX() {
		return this->x;
	}

	//--------------------------------------------------------------
	int getY() {
		return this->y;
	}

	//--------------------------------------------------------------
	int getId() {
		return this->id;
	}

	//--------------------------------------------------------------
	int getLife() {
		return this->life;
	}

	//--------------------------------------------------------------
	bool isDead() {
		return (this->life <= 0);
	}

private:
    int x;
    int y;
    int id;
    int life;
    int max_life;
};
