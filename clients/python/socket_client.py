import socket

class SocketClient:
    def __init__(self, ip: str, port: int):
        self.connectionSucceeded = True
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((ip, port))

    def cleanup(self) -> None:
        self.socket.close()

    def sendMessage(self, message:str) -> None:
        self.socket.sendall(message)

    def receiveResponse(self) -> str:
        data = self.socket.recv(1024)
        return data
