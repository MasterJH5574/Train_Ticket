# filename client.py

import socket

host = "127.0.0.1"
port = 8888


def send(message):
    global s
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(60)
        s.connect((host, port))
        s.send(message.encode('utf-8'))

        res_list = []
        while True:
            d = s.recv(1024)
            if d:
                res_list.append(d.decode('utf-8'))
            else:
                break
        res = ''.join(res_list)
        s.close()
        return res
    finally:
        s.close()


def login(user_id, password):
    return send("login {} {}\n\0".format(user_id, password))


def register(username, password, email, phone):
    return send("register {} {} {} {}\n\0".format(username, password, email, phone))


def query_profile(user_id):
    return send("query_profile {}\n\0".format(user_id))
