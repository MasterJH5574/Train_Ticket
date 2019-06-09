#include <iostream>
#include <fstream>
#include <functional>
#include "BP_Tree.hpp"
#include "vector.hpp"
#include "String.hpp"
#include "price.hpp"
#include "date.hpp"
#include "Train.hpp"
#include "Ticket.hpp"
#include "map.hpp"
using namespace std;

#ifndef _TrainManager
#define _TrainManager

class TrainManager {
    const Date beginDate = Date(2019, 6,1), endDate = Date(2019, 6, 30);
private:
    BP_Tree<trainID, Train> train_record;
    BP_Tree<TrainKey, TrainValue> station_record;
    BP_Tree<TicketKey, TicketVal> ticket_record;

    fstream route_file, ticket_price_file, ticket_left_file;
    char route[10], ticket_price[15], ticket_left[15];

    void query_ticket(const Train& nT, const Date& nD, int num1, int num2, std::istream& is, std::ostream& os)
    {
        const int initNum = 2000;
        int priceNum = nT.seat_num * nT.sta_num;
        Station* stArray = new Station[nT.sta_num];
        Price* priceArray = new Price[priceNum];
        route_file.seekg(nT.route_pos, std::ios::beg);
        ticket_price_file.seekg(nT.ticket_price_pos, std::ios::beg);
        route_file.read(reinterpret_cast<char*> (stArray), sizeof(Station) * nT.sta_num);
        ticket_price_file.read(reinterpret_cast<char*> (priceArray), sizeof(double)* priceNum);

        int daynum = nD - beginDate;
        int leftNum = nT.seat_num * nT.sta_num;                     // tl_num: ticket_left_num
        int* ticketList = new int[leftNum];                                    // ticketList: ticket_left_array
        ticket_left_file.seekg(nT.ticket_left_pos + (off_t) daynum * leftNum * sizeof(int), std::ios::beg);
        ticket_left_file.read(reinterpret_cast<char*> (ticketList), sizeof(int)* leftNum);

        Date D1 = nD, D2 = nD;
        if (stArray[num1].day == 1) ++D1;
        if (stArray[num2].day == 1) ++D2;
        else {
            if (stArray[num2].arriv < stArray[num2 - 1].depart) ++D2;
        }

        os  << nT.id << ' ' << stArray[num1].loc << ' ' << D1 << ' ' << stArray[num1].depart << ' ' << stArray[num2].loc << ' ' << D2 << ' ' << stArray[num2].arriv;
        os << ' ' << nT.seat_num << " ";
        for (int i = 0; i < nT.seat_num; ++i)
        {
            int ticket_left = initNum;
            Price sumPrice;
            for (int j = num1 + 1; j <= num2; ++j) {
                if (ticketList[j + i * nT.sta_num] < ticket_left) {
                    ticket_left = ticketList[j + i * nT.sta_num];
                }
                sumPrice += priceArray[j + i * nT.sta_num];
            }
            os << nT.seat[i] << ' ' << ticket_left << ' ' << sumPrice << " ";
        }

        delete[] stArray;
        delete[] priceArray;
        delete[] ticketList;
    }

    bool inList(char catalog, catalogList & list){
        int i = 0;if (list[0] == '\0') return 0;
        while (1) {
            if (catalog == list[i]) return 1;
            if (list[++i] == '\0') return 0;
        }
    }
    void show_ticket(const Train& train, int num1, int num2, Station* sta_list, const Date& date, std::istream& is, std::ostream& os){
        Date date1 = date, date2 = date;
        if (sta_list[num1].day == 1) ++date1;
        if (sta_list[num2].day == 1) ++date2;
        os << train.id << ' ' << sta_list[num1].loc << ' ' << date1 << ' ' << sta_list[num1].depart << ' ' << sta_list[num2].loc << ' ' << date2 << ' ' << sta_list[num2].arriv;
        int daynum = date - beginDate;
        int totnum = train.seat_num * train.sta_num;
        Price* priceArray = new Price[totnum];
        int* ticketList = new int[totnum];
        ticket_price_file.seekg(train.ticket_price_pos, std::ios::beg);
        ticket_left_file.seekg(train.ticket_left_pos + daynum * totnum * sizeof(int), std::ios::beg);
        ticket_price_file.read(reinterpret_cast<char*> (priceArray), sizeof(double) * totnum);
        ticket_left_file.read(reinterpret_cast<char*> (ticketList), sizeof(int) * totnum);

        os << ' ' << train.seat_num;
        for (int i = 0; i < train.seat_num; i++){
            Price ticket_price;
            int ticket_left = 2000;
            for (int j = num1 + 1; j <= num2; ++j){
                ticket_price += priceArray[j + i * train.sta_num];
                if (ticketList[j + i * train.sta_num] < ticket_left)
                    ticket_left = ticketList[j + i * train.sta_num];
            }
            os << ' ' << train.seat[i] << ' ' << ticket_left << ' ' << ticket_price;
        }
        delete[] priceArray;
        delete[] ticketList;
    }

public:
    TrainManager() :
            train_record("train_record", "ind_train_record"),
            station_record("station_record", "ind_station_record"),
            ticket_record("ticket_record", "ind_ticket_record"),
            route("route"),
            ticket_price("ticket_price"),
            ticket_left("ticket_left")
    {
        fstream file;
        file.open(route);
        if (!file) {
            train_record.initialize();
            station_record.initialize();
            ofstream out; out.open(route); out.close();
            out.open(ticket_price); out.close();
            out.open(ticket_left); out.close();
        }
        else file.close();
        route_file.open(route);
        ticket_left_file.open(ticket_left);
        ticket_price_file.open(ticket_price);
    }

    ~TrainManager() {
        route_file.close();
        ticket_left_file.close();
        ticket_price_file.close();
    }

    /*--------------------------------------------------------------*
     *																*
     *				            train								*
     *																*
     *--------------------------------------------------------------*/

    int add_train(istream &is, ostream &os) {
        Train train;
        is >> train.id >> train.name >> train.catalog >> train.sta_num >> train.seat_num;
        if (train_record.count(train.id) == 1) {
            is >> train.name >> train.catalog >> train.sta_num >> train.seat_num;
            for (int i = 0; i < train.seat_num; ++i) is >> train.seat[i];
            Station station; double ticket_price;
            for (int i = 0; i < train.sta_num; ++i) {
                is >> station.loc >> station.arriv >> station.depart >> station.stop;
                char ch;
                for (int j = 0; j < train.seat_num; ++j) is >> ticket_price;
            }
            return 0;
        }
        for (int i = 0; i < train.seat_num; ++i)
            is >> train.seat[i];
        Station* station = new Station[train.sta_num];
        Price* a_price = new Price[train.sta_num * train.seat_num];
        for (int i = 0; i < train.sta_num; ++i) {
            is >> station[i].loc >> station[i].arriv >> station[i].depart >> station[i].stop;
            if (i == 0)	station[i].day = 0;
            else {
                if (i == train.sta_num - 1) {
                    if (station[i].arriv < station[i - 1].depart) {
                        station[i].day = station[i - 1].day + 1;
                    }
                    else station[i].day = station[i - 1].day;
                }
                else {
                    if (station[i].depart < station[i - 1].depart) {
                        station[i].day = station[i - 1].day + 1;
                    }
                    else station[i].day = station[i - 1].day;
                }
            }
            for (int j = 0; j < train.seat_num; ++j)
                is >> a_price[i + j * train.sta_num];
            TrainKey train_key(station[i].loc, train.id);
            TrainValue train_value(i, train.catalog);
            station_record.insert(train_key, train_value);
        }
        route_file.seekp(0, ios::end);
        train.route_pos = route_file.tellp();
        route_file.write(reinterpret_cast<char*> (station), sizeof(Station) * train.sta_num);
        ticket_price_file.seekp(0, ios::end);
        train.ticket_price_pos = ticket_price_file.tellp();
        ticket_price_file.write(reinterpret_cast<char*> (a_price), (sizeof(Price)) * train.sta_num * train.seat_num);
        int day_num = 30;
        int ticket_left_num = day_num * train.seat_num * train.sta_num;
        int* a_ticket_left = new int[ticket_left_num];
        for (int i = 0; i < ticket_left_num; ++i)
            a_ticket_left[i] = 2000;
        ticket_left_file.seekp(0, ios::end);
        train.ticket_left_pos = ticket_left_file.tellp();
        ticket_left_file.write(reinterpret_cast<char*> (a_ticket_left), (sizeof(int)) * ticket_left_num);

        trainID trainid = train.id;
        train_record.insert(trainid, train);
        delete[] station;
        delete[] a_price;
        delete[] a_ticket_left;
        return 1;
    }

    int delete_train(istream& is, ostream& os) {
        trainID train_id;
        is >> train_id;
        Train train = train_record.zhoumo(train_id);

        if (train.sta_num == 0 || train.open == 1 || train.sale == 1) return 0;

        route_file.seekg(train.route_pos, ios::beg);
        Station* station = new Station[train.sta_num];
        route_file.read(reinterpret_cast<char*> (station), sizeof(Station) * train.sta_num);
        for (int i = 0; i < train.sta_num; ++i) {
            TrainKey bind_key(station[i].loc, train.id);
            station_record.erase(bind_key);
        }

        train_record.erase(train_id);
        delete[] station;
        return 1;
    }

    int modify_train(istream &is, ostream &os){
        trainID train_id;
        is >> train_id;
        Train train;
        try {
            train = train_record.zhoumo(train_id);
        }
        catch (...) {
            is >> train.name >> train.catalog >> train.sta_num >> train.seat_num;
            for (int i = 0; i < train.seat_num; ++i) is >> train.seat[i];
            Station station; double ticket_price;
            for (int i = 0; i < train.sta_num; ++i) {
                is >> station.loc >> station.arriv >> station.depart >> station.stop;

                for (int j = 0; j < train.seat_num; ++j) is >> ticket_price;
            }
            return 0;
        }
        // delete useless info frome is
        if (train.sta_num == 0 || train.open == 1 || train.sale == 1) {
            is >> train.name >> train.catalog >> train.sta_num >> train.seat_num;
            for (int i = 0; i < train.seat_num; ++i) is >> train.seat[i];
            Station station; double ticket_price;
            for (int i = 0; i < train.sta_num; ++i) {
                is >> station.loc >> station.arriv >> station.depart >> station.stop;

                for (int j = 0; j < train.seat_num; ++j) is >> ticket_price;
            }
            return 0;
        }

        //delete old train
        route_file.seekg(train.route_pos, ios::beg);
        Station* station = new Station[train.sta_num];
        route_file.read(reinterpret_cast<char*> (station), sizeof(Station) * train.sta_num);
        for (int i = 0; i < train.sta_num; ++i) {
            TrainKey bind_key(station[i].loc, train.id);
            station_record.erase(bind_key);
        }
        //add_new Train
        is >> train.name >> train.catalog >> train.sta_num >> train.seat_num;
        for (int i = 0; i < train.seat_num; ++i)
            is >> train.seat[i];
        Price* a_price = new Price[train.sta_num * train.seat_num];
        for (int i = 0; i < train.sta_num; ++i) {
            is >> station[i].loc >> station[i].arriv >> station[i].depart >> station[i].stop;
            if (i == 0) {
                station[i].day = 0;
            }
            else {
                bool f;
                if (station[i - 1].arriv == Time("xx:xx")) f = station[i].arriv > station[i - 1].depart;
                else f = station[i].arriv > station[i - 1].arriv;
                if (f) station[i].day = station[i - 1].day;
                else station[i].day = station[i - 1].day + 1;
            }
            char ch;
            for (int j = 0; j < train.seat_num; ++j)
                is >> a_price[i + j * train.sta_num];
        }
        route_file.seekp(0, ios::end);
        train.route_pos = route_file.tellp();
        route_file.write(reinterpret_cast<char*> (station), sizeof(Station) * train.sta_num);
        ticket_price_file.seekp(0, ios::end);
        train.ticket_price_pos = ticket_price_file.tellp();
        ticket_price_file.write(reinterpret_cast<char*> (a_price), (sizeof(double)) * train.sta_num * train.seat_num);
        int day_num = 30;
        int ticket_left_num = day_num * train.seat_num * train.sta_num;
        int* a_ticket_left = new int[ticket_left_num];
        for (int i = 0; i < ticket_left_num; ++i)
            a_ticket_left[i] = 2000;
        ticket_left_file.seekp(0, ios::end);
        train.ticket_left_pos = ticket_left_file.tellp();
        ticket_left_file.write(reinterpret_cast<char*> (a_price), (sizeof(int)) * ticket_left_num);

        train_record.modify(train.id, train);
        delete[] station;
        delete[] a_price;
        delete[] a_ticket_left;
        return 1;
    }

    int sale_train(istream &is, ostream &os) {
        trainID train_id;
        is >> train_id;
        Train train = train_record.zhoumo(train_id);
        if (train.sta_num == 0 || train.open == 1) return 0;
        train.open = 1;
        train_record.modify(train_id, train);
        return 1;
    }

    void query_train(istream &is, ostream &os) {
        trainID train_id;
        is >> train_id;
        Train train;
        try {
            train = train_record.zhoumo(train_id);
        }catch (...) {
            os << "0 ";
            return;
        }
        if (train.open == 0) {
            os << "0 ";
            return;
        }
        os << train.id << ' ' << train.name << ' ' << train.catalog << ' ' << train.sta_num << ' ' << train.seat_num;
        for (int i = 0; i < train.seat_num; ++i)
            os << ' ' << train.seat[i];
        os << " ";
        route_file.seekg(train.route_pos, ios::beg);
        Station* station = new Station[train.sta_num];
        route_file.read(reinterpret_cast<char*> (station), sizeof(Station) * train.sta_num);

        ticket_price_file.seekg(train.ticket_price_pos, ios::beg);
        int ticket_num = train.seat_num * train.sta_num;
        Price* a_price = new Price[ticket_num];
        ticket_price_file.read(reinterpret_cast<char*> (a_price), sizeof(Price) * ticket_num);

        for (int i = 0; i < train.sta_num; ++i) {
            os << station[i].loc << ' ' << station[i].arriv << ' ' << station[i].depart << ' ' << station[i].stop;
            for (int j = 0; j < train.seat_num; ++j)
                os << ' ' << a_price[i + j * train.sta_num];
	    if (i != train.sta_num - 1)
	      os << " ";
        }
        delete[] a_price;
        delete[] station;
        return;
    }

    /*--------------------------------------------------------------*
     *																*
     *				            ticket								*
     *																*
     *--------------------------------------------------------------*/

    void load_station(Station *station, const Train& train) {
        route_file.seekg(train.route_pos, ios::beg);
        route_file.read(reinterpret_cast<char*> (station), sizeof(Station) * train.sta_num);
    }

    int find_seat_index(const Train& current_train, Seat &s) {
        for (int i = 0; i < current_train.seat_num; ++i)
            if (s == current_train.seat[i])
                return i;
        return current_train.seat_num;
    }
    int find_station_index(Station* sta, const int& num, const location &loc) {
        for (int i = 0; i < num; ++i) {
            if (sta[i].loc == loc) {
                return i;
            }
        }
        return num;
    }

    void load_ticket_left(int *ticket_left, int start, int num) {
        ticket_left_file.seekg(start, ios::beg);
        ticket_left_file.read(reinterpret_cast<char*> (ticket_left), sizeof(int) * num);
    }

    int buy_ticket(istream& is, ostream& os) {
        userID id; int num;  trainID train_id; location depart, terminal;  Date date; Seat kind;
        is >> id >> num >> train_id >> depart >> terminal >> date >> kind;
        if (date < beginDate || date > endDate || depart == terminal)
            return 0;

        Train train = train_record.zhoumo(train_id);
        if (train.sta_num == 0 || train.open == 0) return 0;
        //which seat
        int seat_pos = find_seat_index(train, kind);
        if (seat_pos == train.seat_num) return 0;

        Date train_date = date;
        Station * station = new Station[train.sta_num];
        load_station(station, train);
        //which station
        int depart_pos = find_station_index(station, train.sta_num, depart), terminal_pos = 0;
        for (int i = train.sta_num - 1; i >= 0; --i)
            if (station[i].loc == terminal) {
                terminal_pos = i;
                break;
            }
        if (depart_pos >= terminal_pos) {
            delete [] station;
            return 0;
        }
        //which day
        int day_pos = date - beginDate;
        //find the pariticular day of departrue
        int* a_ticket_left = new int[train.sta_num];
        load_ticket_left(a_ticket_left, train.ticket_left_pos + (day_pos * train.sta_num * train.seat_num + seat_pos * train.sta_num) * sizeof(int), train.sta_num);
        int min_left = 2000;
        for (int i = depart_pos + 1; i <= terminal_pos; ++i)
            if (a_ticket_left[i] < min_left) min_left = a_ticket_left[i];
        if (min_left < num) {
            delete[] a_ticket_left;
            delete[] station;
            return 0;
        }
        for (int i = depart_pos + 1; i <= terminal_pos; ++i)
            a_ticket_left[i] -= num;
        ticket_left_file.seekp(train.ticket_left_pos + (day_pos * train.seat_num * train.sta_num + seat_pos * train.sta_num) * sizeof(int), std::ios::beg);
        ticket_left_file.write(reinterpret_cast<char*> (a_ticket_left), sizeof(int) * train.sta_num);

        TicketKey ticketkey(id, date, train.catalog, train.id, depart, terminal);
        TicketVal ticketval = ticket_record.zhoumo(ticketkey);
        //new ticket
        if (ticketval.depart == Time("xx:xx") && ticketval.arriv == Time("xx:xx")) {
            ticketval.begin_day = station[depart_pos].day;
            ticketval.end_day = station[terminal_pos].day;
            ticketval.depart = station[depart_pos].depart;
            ticketval.arriv = station[terminal_pos].arriv;
            for (int i = 0; i < train.seat_num; ++i)
                ticketval.seat[i] = train.seat[i];
            int price_num = train.seat_num * train.sta_num;
            Price * a_price = new Price[price_num];
            ticket_price_file.seekg(train.ticket_price_pos, ios::beg);
            ticket_price_file.read(reinterpret_cast<char*>(a_price), sizeof(Price) * price_num);

            for (int i = 0; i < train.seat_num; ++i) {
                ticketval.price[i] = 0;
                ticketval.num[i] = 0;
                for (int j = depart_pos + 1; j <= terminal_pos; ++j)
                    ticketval.price[i] += a_price[j + i * train.sta_num];
            }
            ticketval.num[seat_pos] += num;
            ticket_record.insert(ticketkey, ticketval);
            delete[]a_price;
        }
        else {
            //already bought the same ticket
            ticketval.num[seat_pos] += num;
            ticket_record.modify(ticketkey, ticketval);
        }
        delete[] station;
        delete[] a_ticket_left;
        return 1;
    };


    struct Sel {
        Train train;
        int sPlace;
        int tPlace;
        Sel(const Train& _train, int _sPlace, int _tPlace) : train(_train), sPlace(_sPlace), tPlace(_tPlace) {}
    };
    bool check_catalog(char catalog, const catalogList &lst) {
        for (int i = 0; lst[i] != '\0'; ++i) {
            if (catalog == lst[i]) {
                return true;
            }
        }
        return false;
    }
    int query_ticket(std::istream & is = std::cin, std::ostream & os = std::cout) {
        location query_st, query_en; Date tmp;
        catalogList catalog_list;
        sjtu::vector<Sel> ansList;
        is >> query_st >> query_en >> tmp >> catalog_list;
        if (tmp < beginDate || tmp > endDate) return -1;
        if (query_st == query_en)return -1;


        sjtu::vector<sjtu::pair<TrainKey, TrainValue> > staArray1, staArray2;
        station_record.get_range(staArray1, TrainKey(query_st), [](const TrainKey &a, const TrainKey &b)->bool {return a.loc == b.loc; });
        station_record.get_range(staArray2, TrainKey(query_en), [](const TrainKey &a, const TrainKey &b)->bool {return a.loc == b.loc; });

        sjtu::vector<sjtu::pair<TrainKey, TrainValue> >::iterator P1 = staArray1.begin(), P2 = staArray2.begin();
        while (1)
        {
            if (P1 == staArray1.end() || P2 == staArray2.end()) break;
            trainID id1((*P1).first.train_id), id2((*P2).first.train_id);
            if (id1 != id2)
            {
                if (id1 < id2) P1++;
                else P2++;
                continue;
            }
            int num1 = (*P1).second.station_pos;
            int num2 = (*P2).second.station_pos;
            char catalog = (*P1).second.catalog;
            if (check_catalog(catalog, catalog_list)) {
                if (num1 >= num2)
                {
                    if (num2 == 0) {
                        Train train = train_record.zhoumo(id2);
                        if (train.sta_num == 0 || train.open == 0)
                        {
                            P1++, P2++; continue;
                        }
                        route_file.seekg(train.route_pos + sizeof(Station) * (train.sta_num - 1), std::ios::beg);
                        Station terminal;
                        route_file.read(reinterpret_cast<char *> (&terminal), sizeof(Station));
                        if (terminal.loc == query_en && num1 < train.sta_num - 1)
                            num2 = train.sta_num - 1;
                        else
                        {
                            P1++, P2++; continue;
                        }
                    }
                    else
                    {
                        P1++, P2++; continue;
                    }
                }
                Train train = train_record.zhoumo(id1);
                if (train.sta_num != 0 && train.open == 1) {
                    Sel Sel(train, num1, num2);
                    ansList.push_back(Sel);
                }
            }
            ++P1;
            ++P2;
        }

        os << ansList.size() << ' ';

        for (sjtu::vector<Sel>::iterator it = ansList.begin(); it != ansList.end(); it++) {
            query_ticket((*it).train, tmp, (*it).sPlace, (*it).tPlace, is, os);
        }

        return 1;
    }
    void get_transfer_train(const sjtu::vector<sjtu::pair<TrainKey, TrainValue> >from_lst, sjtu::vector<halfRoute> &to_lst, const catalogList &tran_lst) {
        for (int i = 0; i < from_lst.size(); ++i) {
            trainID train_id = from_lst[i].first.train_id;
            int station_index = from_lst[i].second.station_pos;
            char catalog = from_lst[i].second.catalog;
            if (!check_catalog(catalog, tran_lst)) continue;

            Train current_train = train_record.zhoumo(train_id);
            if (!current_train.sta_num || !current_train.open) continue;

            halfRoute tmp(current_train, station_index);
            tmp.sta_lst = new Station[current_train.sta_num];
            route_file.seekg(current_train.route_pos, std::ios::beg);
            route_file.read(reinterpret_cast<char *> (tmp.sta_lst), current_train.sta_num * sizeof(Station));
            to_lst.push_back(tmp);
        }
    }
    int make_transfer(const halfRoute & T1, const halfRoute & T2, int &pos1, int & pos2)
    {
        sjtu::map<location, int> posMap;
        for (int i = T1.sta_index + 1; i < T1.current_train.sta_num; i++)
            posMap[T1.sta_lst[i].loc] = i;
        int tagz = 0;
        for (int i = 0; i < T2.sta_index; ++i)
        {
            if (posMap.count(T2.sta_lst[i].loc) == 1)
            {
                pos1 = posMap[T2.sta_lst[i].loc];
                Time tmpPos1 = T1.sta_lst[pos1].arriv;
                pos2 = i;
                Time tmpPos2 = T2.sta_lst[pos2].depart;
                if (tmpPos1 != Time("xx:xx") && tmpPos2 != Time("xx:xx") && tmpPos1 < tmpPos2)
                {
                    tagz = 1;
                    break;
                }
            }
        }
        return tagz;
    }
    void query_transfer(istream &in = cin, ostream &os = cout) {
        location tran_st, tran_en;
        Date tran_date;
        catalogList tran_lst;
        in >> tran_st >> tran_en >> tran_date >> tran_lst;

        if (tran_date < beginDate || tran_date > endDate || tran_st == tran_en) {
            os << "-1 ";
            return ;
        }

        sjtu::vector< sjtu::pair<TrainKey, TrainValue> > tran_tmp1, tran_tmp2;
        //std::cout << "jin1" << std::endl;
        station_record.get_range(tran_tmp1, TrainKey(tran_st), [](const TrainKey &a, const TrainKey &b)->bool {return a.loc == b.loc; });
        //std::cout << "chu1" << std::endl;
        station_record.get_range(tran_tmp2, TrainKey(tran_en), [](const TrainKey &a, const TrainKey &b)->bool {return a.loc == b.loc; });
        //std::cout << "chu2" << std::endl;

        //std::cout << "end " << tran_tmp1.size() << ' ' << tran_tmp2.size() << std::endl;

        sjtu::vector<halfRoute>tran_lst1, tran_lst2;
        get_transfer_train(tran_tmp1, tran_lst1, tran_lst);
        get_transfer_train(tran_tmp2, tran_lst2, tran_lst);

        int index1 = -1, index2 = -1, loc1 = -1, loc2 = -1, res = 0x3f3f3f3f;
        for (int i = 0; i < tran_lst1.size(); ++i) {
            Time depart_time = tran_lst1[i].sta_lst[tran_lst1[i].sta_index].depart;
            if (depart_time == Time("xx:xx")) continue;
            for (int j = 0; j < tran_lst2.size(); ++j) {
                Time arrive_time = tran_lst2[j].sta_lst[tran_lst2[j].sta_index].arriv;
                if (arrive_time == Time("xx:xx") || arrive_time <= depart_time) continue;
                if (tran_lst1[i].current_train.id == tran_lst2[j].current_train.id) continue;
                int tmp_loc1, tmp_loc2;
                int delta = arrive_time - depart_time;
                if (delta < res) {
                    if (make_transfer(tran_lst1[i], tran_lst2[j], tmp_loc1, tmp_loc2)) {
                        res = delta;
                        index1 = i;
                        index2 = j;
                        loc1 = tmp_loc1;
                        loc2 = tmp_loc2;
                    }
                }
            }
        }

        if (res != 0x3f3f3f3f) {
            show_ticket(tran_lst1[index1].current_train, tran_lst1[index1].sta_index, loc1, tran_lst1[index1].sta_lst, tran_date, in, os);
            os << " ";
            show_ticket(tran_lst2[index2].current_train, loc2, tran_lst2[index2].sta_index, tran_lst2[index2].sta_lst, tran_date, in, os);
        }
        for (int i = 0; i < tran_lst1.size(); ++i) delete[] tran_lst1[i].sta_lst;
        for (int i = 0; i < tran_lst2.size(); ++i) delete[] tran_lst2[i].sta_lst;

        if (res >= 0x3f3f3f3f) {
            os << "-1 ";
            return;
        }
        return;
    }

    bool refund_ticket(istream& is, ostream& os) {
        userID refund_user_id; trainID refund_train_id; Seat refund_seat; location refund_st, refund_en; Date refund_date;
        int refund_total;
        is >> refund_user_id >> refund_total >> refund_train_id >> refund_st >> refund_en >> refund_date >> refund_seat;

        Train current_train = train_record.zhoumo(refund_train_id);

        if (!current_train.sta_num || !current_train.seat_num || refund_date < beginDate || refund_date > endDate) return false;

        int refund_seat_index = find_seat_index(current_train, refund_seat);
        if (refund_seat_index == -1) return false;

        TicketKey current_ticket(refund_user_id, refund_date, current_train.catalog, current_train.id, refund_st, refund_en);
        TicketVal current_order;
        try {
            current_order = ticket_record.zhoumo(current_ticket);
        } catch(...) {
            return 0;
        }

        if (current_order.num[refund_seat_index] >= refund_total) {
            current_order.num[refund_seat_index] -= refund_total;
        }
        else return false;//������Ʊ������

        //�Ӷ�������ɾ�����ж�Ʊ�Ƿ��Ѿ��˹�
        int j = 0;
        for (; j < current_train.seat_num; ++j) {
            if (current_order.num[j] > 0) {
                break;
            }
        }
        if (j >= current_train.seat_num) {
            //Ʊ���˹�
            ticket_record.erase(current_ticket);
        }
        else {
            ticket_record.modify(current_ticket, current_order);
        }

        Station* tmp = new Station[current_train.sta_num];
        route_file.seekg(current_train.route_pos, std::ios::beg);
        route_file.read(reinterpret_cast<char*>(tmp), sizeof(Station) * current_train.sta_num);
        int refund_st_index = find_station_index(tmp, current_train.sta_num, refund_st);
        int refund_en_index = find_station_index(tmp, current_train.sta_num, refund_en);
        delete[] tmp;

        int date_delta = refund_date - beginDate;
        int* tmp2 = new int[current_train.sta_num];
        int p = current_train.ticket_left_pos + sizeof(int) * ((date_delta * current_train.seat_num + refund_seat_index) * current_train.sta_num);

        ticket_left_file.seekg(p, std::ios::beg);
        ticket_left_file.read(reinterpret_cast<char*>(tmp2), sizeof(int) * current_train.sta_num);

        for (int i = refund_st_index + 1; i <= refund_en_index; ++i) {
            tmp2[i] += refund_total;
        }

        ticket_left_file.seekp(p, std::ios::beg);
        ticket_left_file.write(reinterpret_cast<char*>(tmp2), sizeof(int) * current_train.sta_num);
        delete[] tmp2;
        return 1;
    }

    bool stop_func1(const TrainKey &l, const TrainKey &r) {
        return (l.loc == r.loc);
    }

    void query_order(istream &is, ostream &os) {
        userID id;
        Date date;
        catalogList cataloglist;
        is >> id >> date >> cataloglist;
        if (date < beginDate || date > endDate) {
            os << "-1 ";
        }
        sjtu::vector<sjtu::vector<sjtu::pair<TicketKey, TicketVal>>> all_ticket;

        for (int i = 0; cataloglist[i] != '\0'; ++i) {
            TicketKey ticketkey(id, date, cataloglist[i]);
            sjtu::vector<sjtu::pair<TicketKey, TicketVal>> certain_ticket;
            auto stop = [](const TicketKey &l, const TicketKey &r) {
                if (l.user_id == r.user_id && l.date == r.date && l.catalog == r.catalog) return 1;
                else return 0;
            };
            ticket_record.get_range(certain_ticket, ticketkey, stop);
            if (!certain_ticket.empty())
                all_ticket.push_back(certain_ticket);
        }
        int ticket_num = 0;
        for (int i = 0; i < all_ticket.size(); ++i) ticket_num += all_ticket[i].size();
        os << ticket_num << ' ';
        for (int i = 0; i < all_ticket.size(); ++i) {
            for (int j = 0; j < all_ticket[i].size(); ++j) {
                Date date1 = date, date2 = date;
                if (all_ticket[i][j].second.begin_day == 1) ++date1;
                if (all_ticket[i][j].second.end_day == 1) ++date2;
                os << all_ticket[i][j].first.train_id << ' ' << all_ticket[i][j].first.depart << ' ' << date1 << ' '
                   << all_ticket[i][j].second.depart << ' '
                   << all_ticket[i][j].first.terminal << ' ' << date2 << ' ' << all_ticket[i][j].second.arriv;
                int seat_num = 0;
                for (int l = 0; all_ticket[i][j].second.num[l] != -1; ++l)
                    ++seat_num;
                os << ' ' << seat_num;
                for (int l = 0; all_ticket[i][j].second.num[l] != -1; ++l)
                    os << ' ' << all_ticket[i][j].second.seat[l] << ' ' << all_ticket[i][j].second.num[l] << ' '
                       << all_ticket[i][j].second.price[l];
		if (i != all_ticket.size() - 1 || j != all_ticket[i].size() - 1)
		    os << ' ';
            }
        }
    }

    int exist_train(istream &is, ostream &os){
        trainID trainid;
        is >> trainid;
        return train_record.count(trainid);
    }
	void query_order_alldate(istream &is, ostream &os) {
		userID id;
		Date date(2019,6,1);
		catalogList cataloglist;
		is >> id >> cataloglist;
		sjtu::vector<sjtu::vector<sjtu::pair<TicketKey, TicketVal>>> all_ticket;
		sjtu::vector<Date> ndate;
		for(int i=1;i<=30;i++)
		{
			for (int i = 0; cataloglist[i] != '\0'; ++i) {
				TicketKey ticketkey(id, date, cataloglist[i]);
				sjtu::vector<sjtu::pair<TicketKey, TicketVal>> certain_ticket;
				auto stop = [](const TicketKey &l, const TicketKey &r) {
					if (l.user_id == r.user_id && l.date == r.date && l.catalog == r.catalog) return 1;
					else return 0;
				};
				ticket_record.get_range(certain_ticket, ticketkey, stop);
				if (!certain_ticket.empty())
				{
					all_ticket.push_back(certain_ticket);
					ndate.push_back(date);
				}
			}
			++date;
		}
		int ticket_num = 0;
		for (int i = 0; i < all_ticket.size(); ++i) ticket_num += all_ticket[i].size();
		os << ticket_num;
		for (int i = 0; i < all_ticket.size(); ++i) {
			date=ndate[i];
			for (int j = 0; j < all_ticket[i].size(); ++j) {
				Date date1 = date, date2 = date;
				if (all_ticket[i][j].second.begin_day == 1) ++date1;
				if (all_ticket[i][j].second.end_day == 1) ++date2;
				os << " " << all_ticket[i][j].first.train_id << ' ' << all_ticket[i][j].first.depart << ' ' << date1 << ' '
					<< all_ticket[i][j].second.depart << ' '
					<< all_ticket[i][j].first.terminal << ' ' << date2 << ' ' << all_ticket[i][j].second.arriv;
                int cnt = 0;
				for (int l = 0; all_ticket[i][j].second.num[l] != -1; ++l)
                    cnt++;
                os << " " << cnt;
				for (int l = 0; all_ticket[i][j].second.num[l] != -1; ++l)
					os << ' ' << all_ticket[i][j].second.seat[l] << ' ' << all_ticket[i][j].second.num[l] << ' ' <<
        all_ticket[i][j].second.price[l];
                /*
                if (i != all_ticket.size() - 1 || j != all_ticket[i].size() - 1)
                    os << " ";
                */
			}
		}
	}
    
};

#endif
