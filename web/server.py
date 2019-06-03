# -*- coding: utf-8 -*-

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
    server = socketserver.ThreadingTCPServer(("127.0.0.1", 8888), MyServer)
    server.serve_forever()
