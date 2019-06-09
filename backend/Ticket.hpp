#include "String.hpp"
#include "price.hpp"
#include "date.hpp"
#include <cstring>

#ifndef _TICKET
#define _TICKET

struct TicketKey {
	userID user_id; Date date; Catalog catalog; trainID train_id; location depart, terminal;
	TicketKey(const userID& _user_id = userID(), const Date& _date = Date(), const Catalog& _catalog = Catalog(), const trainID& _train_id = trainID(), const location& _depart = location(), const location& _terminal = location()) :
		user_id(_user_id), date(_date), catalog(_catalog), train_id(_train_id), depart(_depart), terminal(_terminal)
	{}
	bool operator<(const TicketKey& other) const {
		if (user_id > other.user_id) return 0;
		if (user_id < other.user_id) return 1;
		if (date > other.date) return 0;
		if (date < other.date) return 1;
		if (catalog > other.catalog) return 1;
		if (catalog < other.catalog) return 0;
		if (train_id > other.train_id) return 0;
		if (train_id < other.train_id) return 1;
		if (depart < other.depart) return 1;
		if (depart > other.depart) return 0;
		return terminal < other.terminal;
	}
};

struct TicketVal {
	int begin_day, end_day;
	Time depart, arriv;
	Seat seat[12];
	Price price[12];
	int num[12];
	TicketVal(const Time& _depart = Time("xx:xx"), const Time &_arriv = Time("xx:xx")) : depart(_depart), arriv(_arriv) {
		begin_day = end_day = 0;
		for (int i = 0; i < 12; ++i) num[i] = -1;
	}
};

struct halfRoute {
    Train current_train;
    int sta_index;
    Station *sta_lst;
    halfRoute() : sta_lst(NULL) {}
    halfRoute(const Train &t, int n, Station *lst = NULL) : current_train(t), sta_index(n), sta_lst(lst) {}
};


#endif