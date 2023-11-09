from socket_client import SocketClient

class RemoteCache:

    def __init__(self, ip: str, port: int):
        self.ip = ip
        self.port = port
        self.__establishConnection()
        self.maxRetries = 3

        pass

    def cleanup(self):
        if self.client is not None:
            self.__closeConnection()

    def get(self, key: str) -> str:
        pass

    def delete(self, key:str) -> None:
        pass

    def set(self, key:str, val: str) -> None:
        pass

    def initiateAndCloseConnection(self) -> None:
        pass

    def __establishConnection(self):
        pass

    def __reestablishConnection(self):
        pass

    def __closeConnection(self):
        pass
