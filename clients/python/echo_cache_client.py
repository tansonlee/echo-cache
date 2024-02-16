from socket_client import SocketClient

class RemoteCache:
    def __init__(self, ip: str, port: int):
        self.ip = ip
        self.port = port
        self.__establishConnection()
        self.maxRetries = 3

    def cleanup(self):
        if self.client is not None:
            self.__closeConnection()

    def get(self, key: str) -> str:
        response = self.__sendMessageWithRetries("get||" + key)
        if response is None:
            return ""
        return self.__parseGetResponse(response)

    def delete(self, key:str) -> bool:
        response = self.__sendMessageWithRetries("del||" + key)
        if response is None:
            return False
        return len(response) != 0

    def set(self, key:str, val: str) -> None:
        response = self.__sendMessageWithRetries("set||" + key + "||" + val)
        if response is None:
            return False
        return len(response) != 0

    def initiateAndCloseConnection(self) -> None:
        try:
            self.client.sendMessage("end")
        except:
            pass
        self.__closeConnection()

    def __establishConnection(self):
        self.client = SocketClient(self.ip, self.port)

    def __reestablishConnection(self):
        self.__closeConnection()
        self.__establishConnection()

    def __closeConnection(self):
        self.client.cleanup()
        self.client = None
    
    def __sendMessageWithRetries(self, message) -> str:
        retriesLeft = self.maxRetries
        while retriesLeft >= 1:
            try:
                self.client.sendMessage(message)
                response = self.client.receiveResponse()
                if len(response) == 0:
                    raise Exception("bad connection")
                return response
            except:
                self.__reestablishConnection()
            retriesLeft -= 1

        return None

    # 0||tanson -> "tanson"
    # 1||Value for key not found: name -> ""
    def __parseGetResponse(self, response):
        statusCode = int(response[0])
        if statusCode != 0:
            return ""
        return response[3:]


