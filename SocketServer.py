
import socket
import threading
import re
import time
MAX_MESSAGE_LEN=512
#ClientCommands
DO_NOTHING="0"
READ_DISTANCE="RD"
BEEP="beep"
TURNGREENLEDON="greenon"
TURNREDLEDON="redon"
#UsefulVars
BUTTONCLIENT="B"
ULTRASONICCLIENT="U"
BUTTONPRESSED="BP"
class ButtonClient:
    def __init__(self):
        self.DistanceRequest=False
        self.subjectFound=False
    def setDistanceRequest(self,newValue:bool):
        self.DistanceRequest=newValue
    def getDistanceRequest(self):
        return self.DistanceRequest
    def getSubjectFound(self):
        return self.subjectFound
    def receiveUltrasonicDistance(self,distanceMeasured):
        if(distanceMeasured>0 and distanceMeasured<100):
            self.subjectFound=True
        else:
            self.subjectFound=False
        self.DistanceRequest=False

    
class SocketServer:
    def __init__(self,PORT,ip):
        self.PORT=PORT
        self.Ip=ip
        self.ConnectionsbyADDR={}
        self.clientThreads={}
        self.buttonClient=ButtonClient()
        self.socketServer=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.socketServer.bind((ip,PORT))
    def start(self):
        print("Server Ip "+self.Ip)
        self.socketServer.listen()
        while True:
            Connection,addr=self.socketServer.accept()
            self.clientThreads[addr]=threading.Thread(target=self.handleClient, args=(addr,))
            self.ConnectionsbyADDR[addr]=Connection
            self.clientThreads[addr].start()

    def handleClient(self,address):
        print("New Connection: "+address[0])
        TypeOfDevice=self.receiveMessage(address)
        if(TypeOfDevice==BUTTONCLIENT):
            self.HandleButtonClient(address)
        elif(TypeOfDevice==ULTRASONICCLIENT):
            self.HandleUltrasonicSensorClient(address)

    def HandleUltrasonicSensorClient(self,address):
        if(self.buttonClient.getDistanceRequest()==False):
            self.sendMessage(DO_NOTHING,address)
            return 0
        self.sendMessage(READ_DISTANCE,address)
        distanceRetrieved=float(self.receiveMessage(address))
        self.buttonClient.receiveUltrasonicDistance(distanceRetrieved)
        if(self.buttonClient.getSubjectFound()):
            self.sendMessage(DO_NOTHING,address)
        else:
            self.sendMessage(BEEP,address)
            
    def HandleButtonClient(self,address):
        ButtonMSG=self.receiveMessage(address)
        ButtonMSG=str(ButtonMSG)
        if ButtonMSG==BUTTONPRESSED:
            self.buttonClient.setDistanceRequest(True)
            self.buttonClient.receiveUltrasonicDistance(50)
            while(self.buttonClient.getDistanceRequest()): pass  #wait UntilUltrasonicAnswers
            
            if(self.buttonClient.getSubjectFound()):
                self.sendMessage(TURNGREENLEDON,address)
            else:
                self.sendMessage(TURNREDLEDON,address)
        else:
            self.sendMessage(DO_NOTHING,address)

    def sendMessage(self,Message,address):
        self.ConnectionsbyADDR[address].send(Message.encode("utf-8"))
    def receiveMessage(self,address):
        answer=self.ConnectionsbyADDR[address].recv(MAX_MESSAGE_LEN).decode("utf-8")
        return re.sub(r'\r\n', '', answer)

def main():
    PORT=21000
    ip=socket.gethostbyname(socket.gethostname())
    Socket=SocketServer(PORT,ip) 
    Socket.start()

main()