# filename client.py

import socket

host = "127.0.0.1"
port = 8888


# send and fetch
def send_to_socket(message):
    try:
        _socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        _socket.settimeout(10)
        _socket.connect((host, port))
        _socket.send(message.encode('utf-8'))

        _list = []
        while True:
            d = _socket.recv(1024)
            if d:
                _list.append(d)
            else:
                break
        res = ''.join(_list)
        _socket.close()
        return res
    finally:
        _socket.close()

def main():
    return ""
