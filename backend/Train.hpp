#pragma once
#include "String.hpp"
#include "price.hpp"
#include "date.hpp"

#ifndef _TRAIN
#define _TRAIN

struct Train {
	trainID id;
	trainName name;
	Catalog catalog;
	int sta_num, seat_num;
	off_t route_pos, ticket_price_pos, ticket_left_pos;
	Seat seat[12];
	int open, sale;
	Train() { sta_num = seat_num = sale = open = 0; }
};

struct TrainKey {
	trainID train_id; location loc;
	TrainKey(const location& _loc = location(), const trainID& _train_id = trainID()) : loc(_loc), train_id(_train_id) {
	}
	bool operator == (const TrainKey& other) {
		return (loc == other.loc && train_id == other.train_id);
	}
	bool operator < (const TrainKey & other) const {
        if (loc < other.loc) return 1;
        else if (loc > other.loc) return 0;
        else return train_id < other.train_id;
	}
};

struct TrainValue {
	int station_pos; Catalog catalog;
	TrainValue(int& _station_pos, Catalog& _catalog) {
		station_pos = _station_pos, catalog = _catalog;
	}
};

struct Station {
	location loc; Time arriv, depart, stop;
	int day;
};


#endif