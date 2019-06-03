# -*- coding: utf-8 -*-

import socket

host = "127.0.0.1"
port = 8888


def send(message):
    global s
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(600)
        s.connect((host, port))
        s.send(message.encode('utf-8'))
        res = s.recv(1024).decode('utf-8')
        s.close()
        return res
    finally:
        s.close()


'''
def send(message):
    global s
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(600)
        s.connect((host, port))
        s.send(message.encode('utf-8'))

        res_list = []
        while True:
            d = s.recv(1024)
            if d:
                print(d)
                res_list.append(d.decode('utf-8'))
            else:
                break
        res = ''.join(res_list)
        s.close()
        return res
    finally:
        s.close()
'''


def login(user_id, password):
    x = int(send("login {} {}".format(user_id, password)))
    print(x)
    return x


def register(username, password, email, phone):
    return int(send("register {} {} {} {}".format(username, password, email, phone)))


def query_profile(user_id):
    return send("query_profile {}".format(user_id))


def query_password(user_id):
    return send("query_password {}".format(user_id))


def modify_profile(user_id, username, password, email, phone):
    return int(send("modify_profile {} {} {} {} {}".format(user_id, username, password, email, phone)))


def get_privilege(user_id):
    res = query_profile(user_id).split(' ')
    return int(res[3])


def modify_privilege(id1, id2, privilege):
    return int(send("modify_privilege {} {} {}".format(id1, id2, privilege)))


def query_ticket(source, terminal, date, catalog):
    res = send("query_ticket_frontend {} {} {} {}".format(source, terminal, date, catalog)).split(" ")
    tot_line = int(res[0])
    if tot_line == -1:
        table = '<tr><td colspan="15"><p class="h5">查询非法，请重新查询</p></td></tr>'
        return table
    if tot_line == 0:
        table = '<tr><td colspan="15"><p class="h5">没有找到符合要求的车次</p></td></tr>'
        return table

    index = 1
    table = ""
    for i in range(0, tot_line):
        seat_cnt = 0

        table += "<tr><td>" + res[index] + "</td>"
        table += "<td>" + res[index + 1] + "<br>" + res[index + 4] + "</td>"
        table += "<td>" + res[index + 2] + " " + res[index + 3] + \
                 "<br>" + res[index + 5] + " " + res[index + 6] + "</td>"

        ticket_type = {"商务座": (0, 0),
                       "特等座": (0, 0),
                       "一等座": (0, 0),
                       "二等座": (0, 0),
                       "高级软卧": (0, 0),
                       "软卧": (0, 0),
                       "动卧": (0, 0),
                       "硬卧": (0, 0),
                       "软座": (0, 0),
                       "硬座": (0, 0),
                       "无座": (0, 0)
                       }

        item_cnt = int(res[index + 7])
        for j in range(0, item_cnt):
            typename = res[index + 7 + 3 * j + 1]
            if typename not in ticket_type.keys():
                continue
            cnt = int(res[index + 7 + 3 * j + 2])
            seat_cnt += cnt
            price = float(res[index + 7 + 3 * j + 3])
            ticket_type[typename] = (cnt, price)

        for value in ticket_type.values():
            if value[0] == 0:
                table += "<td>无</td>"
            else:
                table += "<td>" + str(value[0]) + "<br>" + str(value[1]) + "元</td>"
        if seat_cnt != 0:
            table += '<td><button class="btn btn-primary btn-buy" '
            table += 'data-toggle="modal" data-target="#buy_modal">购买</button></td></tr>'
        else:
            table += '<td><button class="btn btn-secondary" disabled>购买</button></td></tr>'
        index += 8 + 3 * item_cnt

    return table


def query_transfer(source, terminal, date, catalog):
    res = send("query_transfer_frontend {} {} {} {}".format(source, terminal, date, catalog)).split(" ")
    if res[0] == "-1":
        table = '<tr><td colspan="15"><p class="h5">查询非法，或没有找到符合要求的车次</p></td></tr>'
        return table

    index = 0
    table = ""
    for i in range(0, 2):
        seat_cnt = 0

        table += "<tr><td>" + res[index] + "</td>"
        table += "<td>" + res[index + 1] + "<br>" + res[index + 4] + "</td>"
        table += "<td>" + res[index + 2] + " " + res[index + 3] + \
                 "<br>" + res[index + 5] + " " + res[index + 6] + "</td>"

        ticket_type = {"商务座": (0, 0),
                       "特等座": (0, 0),
                       "一等座": (0, 0),
                       "二等座": (0, 0),
                       "高级软卧": (0, 0),
                       "软卧": (0, 0),
                       "动卧": (0, 0),
                       "硬卧": (0, 0),
                       "软座": (0, 0),
                       "硬座": (0, 0),
                       "无座": (0, 0)
                       }

        item_cnt = int(res[index + 7])
        for j in range(0, item_cnt):
            typename = res[index + 7 + 3 * j + 1]
            if typename not in ticket_type.keys():
                continue
            cnt = int(res[index + 7 + 3 * j + 2])
            seat_cnt += cnt
            price = float(res[index + 7 + 3 * j + 3])
            ticket_type[typename] = (cnt, price)

        for value in ticket_type.values():
            if value[0] == 0:
                table += "<td>无</td>"
            else:
                table += "<td>" + str(value[0]) + "<br>" + str(value[1]) + "元</td>"
        if seat_cnt != 0:
            table += '<td><button class="btn btn-primary btn-buy" '
            table += 'data-toggle="modal" data-target="#buy_modal">购买</button></td></tr>'
        else:
            table += '<td><button class="btn btn-secondary" disabled>购买</button></td></tr>'
        index += 8 + 3 * item_cnt

    return table


def buy_ticket(user_id, train_id, source, terminal, date, seat):
    return int(send("buy_ticket {} {} {} {} {} {} {}".format(user_id, str(1), train_id,
                                                             source, terminal, date, seat)))


def query_order(user_id, date, catalog):
    res = send("query_order_frontend {} {} {}".format(user_id, date, catalog)).split(' ')
    tot_line = int(res[0])
    if tot_line == -1:
        table = '<tr><td colspan="6"><p class="h5">查询非法，您没有购买车票</p></td></tr>'
        return table

    if tot_line == 0:
        table = '<tr><td colspan="6"><p class="h5">您没有购买车票</p></td></tr>'
        return table

    index = 1
    table = ""
    for i in range(0, tot_line):
        ticket_type = {"商务座": 0,
                       "特等座": 0,
                       "一等座": 0,
                       "二等座": 0,
                       "高级软卧": 0,
                       "软卧": 0,
                       "动卧": 0,
                       "硬卧": 0,
                       "软座": 0,
                       "硬座": 0,
                       "无座": 0
                       }

        item_cnt = int(res[index + 7])
        for j in range(0, item_cnt):
            typename = res[index + 7 + 3 * j + 1]
            if typename not in ticket_type.keys():
                continue
            cnt = int(res[index + 7 + 3 * j + 2])
            ticket_type[typename] = cnt

        for key, value in ticket_type.items():
            if value != 0:
                table += "<tr><td>" + res[index] + "</td>"
                table += "<td>" + res[index + 1] + "<br>" + res[index + 4] + "</td>"
                table += "<td>" + res[index + 2] + " " + res[index + 3] + \
                         "<br>" + res[index + 5] + " " + res[index + 6] + "</td>"
                table += "<td>" + key + "</td>"
                table += "<td>" + str(value) + "</td>"
                table += '<td><button class="btn btn-primary btn-refund" '
                table += 'data-toggle="modal" data-target="#refund_modal">退票</button></td></tr>'

        index += 8 + 3 * item_cnt

    return table


def query_order_all(user_id, catalog):
    res = send("query_order_all {} {}".format(user_id, catalog)).split(" ")
    tot_line = int(res[0])
    if tot_line == -1:
        table = '<tr><td colspan="6"><p class="h5">查询非法，您没有购买车票</p></td></tr>'
        return table

    if tot_line == 0:
        table = '<tr><td colspan="6"><p class="h5">您没有购买车票</p></td></tr>'
        return table

    index = 1
    table = ""
    for i in range(0, tot_line):
        ticket_type = {"商务座": 0,
                       "特等座": 0,
                       "一等座": 0,
                       "二等座": 0,
                       "高级软卧": 0,
                       "软卧": 0,
                       "动卧": 0,
                       "硬卧": 0,
                       "软座": 0,
                       "硬座": 0,
                       "无座": 0
                       }

        item_cnt = int(res[index + 7])
        for j in range(0, item_cnt):
            typename = res[index + 7 + 3 * j + 1]
            if typename not in ticket_type.keys():
                continue
            cnt = int(res[index + 7 + 3 * j + 2])
            ticket_type[typename] = cnt

        for key, value in ticket_type.items():
            if value != 0:
                table += "<tr><td>" + res[index] + "</td>"
                table += "<td>" + res[index + 1] + "<br>" + res[index + 4] + "</td>"
                table += "<td>" + res[index + 2] + " " + res[index + 3] + \
                         "<br>" + res[index + 5] + " " + res[index + 6] + "</td>"
                table += "<td>" + key + "</td>"
                table += "<td>" + str(value) + "</td>"
                table += '<td><button class="btn btn-primary btn-refund" '
                table += 'data-toggle="modal" data-target="#refund_modal">退票</button></td></tr>'

        index += 8 + 3 * item_cnt

    return table


def refund_ticket(user_id, train_id, source, terminal, date, seat):
    return int(send("refund_ticket {} {} {} {} {} {}".format(user_id, str(1), train_id, source, terminal, date, seat)))


def add_train(train_id, train_name, train_type, ticket, station, arrive_time, start_time, overstop_time, bussiness_seat_price, principal_seat_price, first_seat_price, second_seat_price, soft2_bed_price, soft_bed_price, D_bed_price, hard_bed_price, soft_seat_price, hard_seat_price, no_seat_price):
    total_station = len(station)
    total_ticket = len(ticket)
    s = ("add_train {} {} {} {} {} ".format(train_id, train_name, train_type, total_station, total_ticket)) + " ".join(ticket)
    for i in range(total_station):
        now = "\n" + station[i] + " " + arrive_time[i] + " " + start_time[i] + " " + overstop_time[i]
        if bussiness_seat_price:
            now = now + " " + bussiness_seat_price[i]
        if principal_seat_price:
            now = now + " " + principal_seat_price[i]
        if first_seat_price:
            now = now + " " + first_seat_price[i]
        if second_seat_price:
            now = now + " " + second_seat_price[i]
        if soft2_bed_price:
            now = now + " " + soft2_bed_price[i]
        if soft_bed_price:
            now = now + " " + soft_bed_price[i]
        if D_bed_price:
            now = now + " " + D_bed_price[i]
        if hard_bed_price:
            now = now + " " + hard_bed_price[i]
        if soft_seat_price:
            now = now + " " + soft_seat_price[i]
        if hard_seat_price:
            now = now + " " + hard_seat_price[i]
        if no_seat_price:
            now = now + " " + no_seat_price[i]
        s = s + now
    return int(send(s))


def modify_train(train_id, train_name, train_type, ticket, station, arrive_time, start_time, overstop_time, bussiness_seat_price, principal_seat_price, first_seat_price, second_seat_price, soft2_bed_price, soft_bed_price, D_bed_price, hard_bed_price, soft_seat_price, hard_seat_price, no_seat_price):
    total_station = len(station)
    total_ticket = len(ticket)
    s = ("add_train {} {} {} {} {} ".format(train_id, train_name, train_type, total_station, total_ticket)) + " ".join(ticket)
    for i in range(total_station):
        now = "\n" + station[i] + " " + arrive_time[i] + " " + start_time[i] + " " + overstop_time[i]
        if bussiness_seat_price:
            now = now + " " + bussiness_seat_price[i]
        if principal_seat_price:
            now = now + " " + principal_seat_price[i]
        if first_seat_price:
            now = now + " " + first_seat_price[i]
        if second_seat_price:
            now = now + " " + second_seat_price[i]
        if soft2_bed_price:
            now = now + " " + soft2_bed_price[i]
        if soft_bed_price:
            now = now + " " + soft_bed_price[i]
        if D_bed_price:
            now = now + " " + D_bed_price[i]
        if hard_bed_price:
            now = now + " " + hard_bed_price[i]
        if soft_seat_price:
            now = now + " " + soft_seat_price[i]
        if hard_seat_price:
            now = now + " " + hard_seat_price[i]
        if no_seat_price:
            now = now + " " + no_seat_price[i]
        s = s + now
    return int(send(s))


def sale_train(train_id):
    return int(send("sale_train {}".format(train_id)))


def delete_train(train_id):
    return int(send("delete_train {}".format(train_id)))


def exist_train(train_id):
    return int(send("exist_train {}".format(train_id)))


def query_train(train_id):
    return send("query_train {}".format(train_id))