
import socket # library used for server purposes
import threading #library used for multiple users
import re
from time import sleep
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
MINDISTANCETODETECTPERSON=0
MAXDISTANCETODETECTPERSON=50
class ButtonClient: #class used for getting distance and returning parameters based on that
    def __init__(self,minDistance=0,maxDistance=100): #if distance doesnt have a value is automatically put on 0 and 100
        self.DistanceRequest=False
        self.subjectFound=False
        self.minDistance=minDistance
        self.maxDistance=maxDistance
    def setDistanceRequest(self,newValue:bool):
        self.DistanceRequest=newValue
    def getDistanceRequest(self):
        return self.DistanceRequest
    def getSubjectFound(self):
        return self.subjectFound
    def receiveUltrasonicDistance(self,distanceMeasured):
        if(distanceMeasured>=self.minDistance and distanceMeasured<=self.maxDistance):
            self.subjectFound=True
        else:
            self.subjectFound=False
        self.DistanceRequest=False

    
class SocketServer: #class we use for starting the server
    def __init__(self,PORT,ip):
        self.PORT=PORT
        self.Ip=ip
        self.ConnectionsbyADDR={}
        self.clientThreads={}
        self.buttonClient=ButtonClient(MINDISTANCETODETECTPERSON,MAXDISTANCETODETECTPERSON) # we set our parameters for our person detection
        self.socketServer=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.socketServer.bind((ip,PORT)) #binding our ip and port initialized in main()
    def start(self):
        print("Server Ip "+self.Ip)
        self.socketServer.listen() #we start hearing client connection
        while True:
            Connection,addr=self.socketServer.accept()
            self.clientThreads[addr]=threading.Thread(target=self.handleClient, args=(addr,))
            self.ConnectionsbyADDR[addr]=Connection #we save our client connection using variable address
            self.clientThreads[addr].start() #we initialize a thread with each new client

    def handleClient(self,address):
        print("New Connection: "+address[0])
        TypeOfDevice=self.receiveMessage(address)
        
        if(TypeOfDevice==BUTTONCLIENT): #depending of our type of device we get our client type
            print("Button Client Connected")
            self.HandleButtonClient(address)
        elif(TypeOfDevice==ULTRASONICCLIENT):
            print("Ultra Sonic Connected")
            self.HandleUltrasonicSensorClient(address)

    def HandleUltrasonicSensorClient(self,address): #for our ultrasonicsensor client we get the distance and beep based on the value
        if(self.buttonClient.getDistanceRequest()==False):
            self.sendMessage(DO_NOTHING,address)
            return 0
        self.sendMessage(READ_DISTANCE,address)
        sleep(1)
        clientAnswer=self.receiveMessage(address)
        if(clientAnswer==''):
            distanceRetrieved=0
        else:
            distanceRetrieved=float(clientAnswer)
        print("Distance Retrieved from ultrasonic client: ")
        print(distanceRetrieved)
        self.buttonClient.receiveUltrasonicDistance(distanceRetrieved)
        if(self.buttonClient.getSubjectFound()):
            self.sendMessage(DO_NOTHING,address)
        else:
            self.sendMessage(BEEP,address)
            
    def HandleButtonClient(self,address): #for our button client we turn a led on green or red based on the distance value returned in our sensor client
        ButtonMSG=self.receiveMessage(address)
        ButtonMSG=str(ButtonMSG)
        if ButtonMSG==BUTTONPRESSED:
            self.buttonClient.setDistanceRequest(True)
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
        return re.sub(r'\r\n', '', answer) #we return an string without carriage characters

def main():
    PORT=21000 #port chosen for our server
    ip=socket.gethostbyname(socket.gethostname()) #we get our server IP
    Socket=SocketServer(PORT,ip) # we bind that IP to our server class
    Socket.start()

main()