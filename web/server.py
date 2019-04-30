# server.py from Yi Gu

import socket, socketserver
import threading

stdout_lock = threading.Lock()


def format_address(address):
    return "{}:{}".format(address[0], address[1])


class MyServer(socketserver.BaseRequestHandler):

    def handle(self):
        print("(Connected) {}".format(format_address(self.client_address)))
        connection = self.request
        connection.settimeout(60)
        try:
            recv = connection.recv(1024)
            print("(Receive) {} {}".format(format_address(self.client_address), str(recv, encoding="utf-8")))

            global stdout_lock
            if stdout_lock.acquire():
                try:
                    tosend = bytes(input("--- Response? --- >>> "), encoding="utf-8")
                    connection.sendall(tosend)
                    print("(Send) {} {}".format(format_address(self.client_address), str(tosend, encoding="utf-8")))
                except BrokenPipeError:
                    print("(Error) Client disconnected")
                finally:
                    stdout_lock.release()
            else:
                print("(Error) Server busy")
        except socket.timeout:
            print("(Timeout) {}".format(format_address(self.client_address)))
        print("(Finish) {}".format(format_address(self.client_address)))


if __name__ == "__main__":
    server = socketserver.ThreadingTCPServer(("127.0.0.1", 8081), MyServer)
    server.serve_forever()

'''
# filename server.py

import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

host = "localhost"
port = 8888

s.bind((host, port))
s.listen(5)

while True:
    c, addr = s.accept()
    print("Connect to %s" % str(addr))

    data = c.recv(1024)
    print(data.decode('utf-8'))

    c.close()
'''