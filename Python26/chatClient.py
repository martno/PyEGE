# -*- coding: cp1252 -*-

import urllib
externalIP = urllib.urlopen("http://www.whatismyip.com/automation/n09230945.asp").read()

import re

import wx
import xmpp


import ege

from threading import Thread

from time import sleep

from random import randint

VERSION = '0.1.5'

GAME_DESTINATION = 'game.py'
MAX_NO_OF_PLAYERS = 16



# for future versions of the chat:
# if a client wants to change a gamesetting from the chat, he sends '<ege>client:data'
# to the channel. After that the host will either resend the data again in the channel with the 
# message '<ege>host:[clientname]:data' or not send anything at all. This is so that the host can verify if the message is valid.
# All clients check that the sender that sends a host-message really is a host
# An announcement of assigning yourself as host is '<ege>client:requestingHostPrivileges'
# The first client that send this is the gamehost of the channel. 
# I wonder how to check who's first (especially if there are only two players in the channel)...






RESOURCE = 'ege_beta' + VERSION
DEFAULT_CONFERENCE_SERVER = 'conference.jabber.org'
DEFAULT_CONFERENCE_NICKNAME = 'PyEGE_user' + str( randint(0, 1024) )
DEFAULT_SERVER_PORT = 'NYI'

class Subscriber:   # Observer
    def __init__(self):
        self._publishers = {}
        
    def __destroy__(self):
        for publisher in self._publishers:
            publisher.detachSubscriber(self)
    
    def update(self, publisher, notificationType, *args):
        raise Exception('update() in Subscriber must be implemented.')
    
    def subscribe(self, publisher, name):
        self._publishers[name] = publisher
        publisher.attachSubscriber(self)
    
    def getPublisher(self, name):
        return self._publishers[name]
    

class Publisher:    # Observable
    def __init__(self):
        self._subscribers = []
        
    def notify(self, *args):
        for subscriber in self._subscribers:
            subscriber.update(self, *args)

    def attachSubscriber(self, newSubscriber):
        self._subscribers.append( newSubscriber )
        
    def detachSubscriber(self, subscriber):
        self._subscribers.remove( subscriber )


class User:
    def __init__(self, jid, presenceState):
        self.jid = jid
        self.presenceState = presenceState
        self.resources = [jid.getResource()]

    def addResource(self, resource):
        self.resources.append(resource)

    def rmResource(self, resource):
        self.resources.remove(resource)
        return self.resources

    def setPresence(self, presence):
        self.presenceState = presence

    def getBareJID(self):
        return self.jid.getStripped()

    def getJID(self):
        return self.jid

class Users:
    def __init__(self):
        self.users = {}

    def addUser(self, jid, presenceState):
        user = User(jid, presenceState)
        bareJID = jid.getStripped()

        existingUser = self.getUserFromBareJID(bareJID)
        if existingUser:
            existingUser.addResource(jid.getResource())
        else:
            self.users[bareJID] = user

    def getUserFromBareJID(self, bareJID):
        if bareJID in self.users:
            return self.users[bareJID]
        else:
            return None
        #for usersBareJID in self.users.keys():
        #    if bareJID == usersBareJID:
        #        return self.users[bareJID]
        #return None


class MUC:
    def __init__(self, mucName, nickname):
        self.mucName = mucName
        self.nickname = nickname
        self.roomJID = self.mucName + '/' + self.nickname
        self.joined = False
        self.mucUsers = []

    def getName(self):
        return self.mucName
        
    def getRoomJID(self):
        return self.roomJID

    def hasJoined(self):
        return self.joined

    def addUser(self, nickname):
        self.joined = True
        self.mucUsers.append( nickname )
        #mucUsers[user.getBareJID()] = user

    def getLocalNickname(self):
        return self.nickname


class MUCHandler:
    def __init__(self, xmppHandler):
        self.mucs = []
        self.xmppHandler = xmppHandler

    def addMUC(self, mucName, nickname):
        newMUC = MUC(mucName, nickname)
        self.mucs.append(newMUC)
        self.xmppHandler.joinGroupchat(newMUC.getRoomJID())
        return newMUC

    def leaveMUC(self, mucName):
        muc = self.getMUC(mucName)
        if muc != None:
            self.xmppHandler.leaveGroupchat(muc.getRoomJID())
            self.mucs.remove(muc)
        
    def getMUC(self, mucName):
        for muc in self.mucs:
            if muc.getName() == mucName:
                return muc
                break
        else:
            return None

    def hasMUC(self, mucName):
        for muc in self.mucs:
            if muc.getName() == mucName:
                return True
                break
        else:
            return False

class xmppHandler(Publisher, Subscriber):
    def __init__(self):
        Publisher.__init__(self)
        Subscriber.__init__(self)
        
        self.client = None
        self.users = Users()
        self.mucHandler = MUCHandler(self)
#        self.joinedMUCs = {}

#    def __del__(self):
#        self.client.disconnect()

    def connect(self, jidString, password):
        self.jidString = jidString
        self.localJID = xmpp.protocol.JID(jidString)
        
        self.client = xmpp.Client(self.localJID.getDomain(), debug = [])
        if not self.client.connect():
            print 'Error: Not connected'
            return False
        if not self.client.auth(self.localJID.getNode(), password, RESOURCE):
            print 'Error: Authentication failed'
            return False

        self.client.RegisterHandler('message', self.handleIncomingMsg)
        self.client.RegisterHandler('presence', self.handleIncomingPresence)

        self.client.sendInitPresence()
        return True
        

    def handleIncomingMsg(self, client, msg):
        message = msg.getBody()
        messageType = msg.getType()
        stripped = msg.getFrom().getStripped()


        if messageType == 'groupchat':
            #print '\n\nPayload:', msg.getPayload()[1].getName(), '\n\n'
            mucName = stripped
            nickname = msg.getFrom().getResource()
            payload = msg.getPayload()
            self.notify(self, 'incomingGroupchatMessage', mucName, nickname, message, payload)
        elif messageType == 'chat':
            bareJID = stripped
            self.notify(self, 'incomingPrivateMessage', bareJID, message)
        else:
            print "messageType", messageType, "received."
        
#        if stripped in self.joinedMUCs:
#            mucName = stripped
#            nickname = msg.getFrom().getResource()
#            self.notify(self, 'incomingGroupchatMessage', mucName, nickname, message)
#        else:
#            bareJID = stripped
#            self.notify(self, 'incomingPrivateMessage', bareJID, message)

    def handleIncomingPresence(self, client, msg):
        remoteJID = msg.getFrom()
        presence = msg.getType()

        stripped = remoteJID.getStripped()
        
        
        if self.mucHandler.hasMUC(stripped):
            username = msg.getFrom().getResource()
            print '\n\n', username, 'joined', stripped, '\n\n'
            muc = self.mucHandler.getMUC(stripped)
            muc.addUser( username )
#            self.joinedMUCs[domain].addUser( username )

            self.notify(self, 'incomingGroupchatPresence', stripped, username, presence)
        else:
            user = self.users.getUserFromBareJID( remoteJID.getStripped() )
            if user:
                user.setPresence( presence )
            else:
                self.users.addUser(remoteJID, presence)

            self.notify(self, 'incomingUserPresence', str( remoteJID.getStripped() ))

    def sendChatMsg(self, recipient, msg):
        self.client.send( xmpp.protocol.Message(recipient, msg, 'chat') )

    def sendGroupchatMessage(self, muc, message):
        self.client.send( xmpp.protocol.Message(muc.getName(), message, 'groupchat') )
        
    def process(self):
        if self.client:
            self.client.Process(0.01)

    def getUserFromBareJID(self, bareJID):
        return self.users.getUserFromBareJID(bareJID)

    def getLocalBareJID(self):
        return self.localJID.getStripped()

    def getClient(self):
        return self.client

    def joinGroupchat(self, roomJID):
        print 'Joining', roomJID
        self.client.send( xmpp.Presence(to = roomJID) )

    def leaveGroupchat(self, roomJID):
        self.client.send( xmpp.Presence(to = roomJID, typ = 'unavailable') )

    def update(self, publisher, *args):
        #print 'xmppHandler args:', args
        notificationType = args[1]
        if publisher == self.getPublisher('joinedRoomsPanel'):
            if notificationType == 'conferenceSelected':
                mucName = args[2]
                roomnick = args[3]
                if not self.mucHandler.hasMUC(mucName):
#                if mucName not in self.joinedMUCs:
                    newMUC = self.mucHandler.addMUC(mucName, roomnick)
                    #roomJID = newMUC.getRoomJID()
#                    self.joinedMUCs[mucName] = MUC(mucName, roomnick)
#                    roomJID = self.joinedMUCs[mucName].getRoomJID()
                    #self.joinGroupchat(roomJID)







class LoginPanel(wx.Panel):
    def __init__(self, parent, xmppHandler):
        wx.Panel.__init__(self, parent)
        self.mParent = parent

        self.xmppHandler = xmppHandler
        
        self.vbox = wx.BoxSizer(wx.VERTICAL)
        self.hboxUN = wx.BoxSizer(wx.HORIZONTAL)
        self.hboxPW = wx.BoxSizer(wx.HORIZONTAL)
    
        self.unText = wx.StaticText(self, -1, 'Username:')
        self.unTextCtrl = wx.TextCtrl(self, -1, '', (-1, -1), (-1, -1), wx.TE_PROCESS_ENTER | wx.TE_NOHIDESEL)
        self.pwText = wx.StaticText(self, -1, 'Password:')
        self.pwTextCtrl = wx.TextCtrl(self, -1, '', (-1, -1), (-1, -1), wx.TE_PROCESS_ENTER | wx.TE_NOHIDESEL | wx.TE_PASSWORD)
        self.connectButton = wx.Button(self, -1, 'Login')
        self.registerButton = wx.Button(self, -1, 'Register account')
        #self.strOnlineStatus =  wx.StaticText(self, -1, 'Status: Offline')

        hboxStatus = wx.BoxSizer(wx.HORIZONTAL)
        self.statusText =  wx.StaticText(self, -1, 'Status: ')
        self.onlineStatusText =  wx.StaticText(self, -1, 'Offline')
        hboxStatus.Add(self.statusText)
        hboxStatus.Add(self.onlineStatusText)

        self.connectGauge = wx.Gauge(self, -1, 100, (-1, -1), (-1, -1), wx.GA_HORIZONTAL | wx.GA_SMOOTH)

        self.Bind(wx.EVT_TEXT_ENTER, self.OnLogin, id = self.unTextCtrl.GetId())
        self.Bind(wx.EVT_TEXT_ENTER, self.OnLogin, id = self.pwTextCtrl.GetId())
        self.Bind(wx.EVT_BUTTON, self.OnLogin, id = self.connectButton.GetId())
        self.Bind(wx.EVT_BUTTON, self.OnRegister, id = self.registerButton.GetId())

        self.hboxUN.Add(self.unText, 0, wx.ALIGN_BOTTOM)
        self.hboxUN.Add(self.unTextCtrl, 1, wx.EXPAND)

        self.hboxPW.Add(self.pwText, 0, wx.ALIGN_BOTTOM)
        self.hboxPW.Add(self.pwTextCtrl, 1, wx.EXPAND)

        self.vbox.Add(self.hboxUN, 0, wx.EXPAND)
        self.vbox.Add(self.hboxPW, 0, wx.EXPAND)
        self.vbox.Add(self.connectButton, 0, wx.EXPAND)
        self.vbox.Add(hboxStatus, 0, wx.EXPAND)
        self.vbox.Add(self.connectGauge, 0, wx.EXPAND)
        self.vbox.Add(self.registerButton, 0, wx.EXPAND)
        
        self.SetSizer(self.vbox)

    def OnLogin(self, event):
        jidString = self.unTextCtrl.GetValue()
        password = self.pwTextCtrl.GetValue()
        connected = self.xmppHandler.connect(jidString, password)
        if connected:
            self.connectGauge.SetValue(100)
            self.onlineStatusText.SetLabel('Online')
            loginText = \
"You have successfully logged in! You can join rooms (a.k.a. conferences) to the right. This is where you connect to other players to play online. \
You can't send games through this client today but this feature will hopefully be available very soon. Therefore you will have to send the games through another service.\
\nFor today the games are not secure so be sure that you trust the source of the games! \
However, as soon as there exist a working version of PyPy's sandbox for Windows this will be implemented in PyEGE and the games should from then on be secure. \
Please contact me if you know how to fix PyPy's sandbox for Windows.\
\nIf you need more help click on Help->PyEGE Help in the menubar."
            wx.MessageBox(loginText, 'Access granted')
        else:
            wx.MessageBox('Unable to connect.', 'Access denied')

    def OnRegister(self, event):
        registerText = \
'Wait! You might already have an account. You can login with almost any XMPP account. \
However, if you don\'t already have an XMPP account you can register one at http://www.jabber.org/create-an-account/'
        wx.MessageBox(registerText, 'Register account')
        

class RosterPanel(wx.Panel, Subscriber, Publisher):
    def __init__(self, parent, xmppHandler):
        Publisher.__init__(self)
        Subscriber.__init__(self)
        
        wx.Panel.__init__(self, parent)
        self.mParent = parent

        self.xmppHandler = xmppHandler

        vbox = wx.BoxSizer(wx.VERTICAL)
        hboxAU = wx.BoxSizer(wx.HORIZONTAL)

        rosterString =  wx.StaticText(self, -1, 'Contact list:')
        self.rosterList = wx.ListCtrl(self, -1, style = wx.LC_LIST | wx.LC_SINGLE_SEL)
        auText =  wx.StaticText(self, -1, 'Add user:')
        auTextCtrl = wx.TextCtrl(self, -1, 'NYI', (600, -1), (-1, -1), wx.TE_PROCESS_ENTER | wx.TE_NOHIDESEL)

        hboxAU.Add(auText, 0, wx.ALIGN_BOTTOM)
        hboxAU.Add(auTextCtrl, 1, wx.EXPAND)

        vbox.Add(rosterString, 0)
        vbox.Add(self.rosterList, 1, wx.EXPAND)
        vbox.Add(hboxAU, 0, wx.EXPAND)

        self.SetSizer(vbox)

        self.subscribe(xmppHandler, 'xmppHandler')

        self.Bind(wx.EVT_LIST_ITEM_ACTIVATED, self.OnActivated, id = self.rosterList.GetId())
        

    def OnActivated(self, event):
        id = self.rosterList.GetFocusedItem()
        label = self.rosterList.GetItemText(id)
        self.notify(self, 'contactSelected', label)

    def update(self, publisher, *args):
        #print 'args:', args
        notificationType = args[1]
        if publisher == self.xmppHandler:
            if notificationType == 'incomingUserPresence' or notificationType == 'incomingPrivateMessage':
                 self.rosterList.Append([args[2]])

                    


class InfoPanel(wx.Panel):
    def __init__(self, parent, xmppHandler):
        wx.Panel.__init__(self, parent)
        self.mParent = parent
        
        
        vbox = wx.BoxSizer(wx.VERTICAL)

        nb = wx.Notebook(self, -1)

        availableRoomsPanel = ChatAvailableRoomsPanel(nb, xmppHandler)
        joinedRoomsPanel = ChatJoinedRoomsPanel(nb, xmppHandler)
        groupChatPanel = InfoGroupPanel(nb, xmppHandler)
        pc = InfoPrivatePanel(nb, xmppHandler)

        nb.AddPage(availableRoomsPanel, 'Available rooms')
        nb.AddPage(joinedRoomsPanel, 'Joined rooms')
        nb.AddPage(groupChatPanel, 'Group chat')
        nb.AddPage(pc, 'Private chat')

        vbox.Add(nb, 1, wx.EXPAND)
        self.SetSizer(vbox)


class ChatAvailableRoomsPanel(wx.Panel, Publisher):
    def __init__(self, parent, xmppHandler):
        Publisher.__init__(self)
        
        wx.Panel.__init__(self, parent)
        self.mParent = parent

        self.xmppHandler = xmppHandler
        self.xmppHandler.subscribe(self, 'joinedRoomsPanel')    # not the best way to use publish-subscribe...
        

        self.hbox = wx.BoxSizer(wx.HORIZONTAL)
        self.rightBox = wx.BoxSizer(wx.VERTICAL)

        self.availableConferencesList = wx.ListCtrl(self, -1, style = wx.LC_REPORT | wx.LC_SINGLE_SEL)
        self.availableConferencesList.InsertColumn(0, 'Name')
        self.availableConferencesList.InsertColumn(1, 'JID')

        conferenceServerText = wx.StaticText(self, -1, 'Conference server:')
        self.findItemsTextCtrl = wx.TextCtrl(self, -1, DEFAULT_CONFERENCE_SERVER, (-1, -1), (-1, -1), wx.TE_PROCESS_ENTER | wx.TE_NOHIDESEL)
        self.findItemsButton = wx.Button(self, -1, 'Find items')
        conferenceNicknameText = wx.StaticText(self, -1, 'Roomnick:')
        self.roomNickTextCtrl = wx.TextCtrl(self, -1, DEFAULT_CONFERENCE_NICKNAME, (-1, -1), (-1, -1), wx.TE_PROCESS_ENTER | wx.TE_NOHIDESEL)
        horizontalLine1 = wx.StaticLine(self, -1)
        joinConferenceText = wx.StaticText(self, -1, 'Join conference:')
        self.joinConferenceTextCtrl = wx.TextCtrl(self, -1, 'pyege@conference.jabber.org', (-1, -1), (-1, -1), wx.TE_PROCESS_ENTER | wx.TE_NOHIDESEL)
        horizontalLine2 = wx.StaticLine(self, -1)
        filterText = wx.StaticText(self, -1, 'Filter: NYI')

        self.Bind(wx.EVT_LIST_ITEM_ACTIVATED, self.OnActivated, id = self.availableConferencesList.GetId())
        self.Bind(wx.EVT_TEXT_ENTER, self.OnFind, id = self.findItemsTextCtrl.GetId())
        self.Bind(wx.EVT_BUTTON, self.OnFind, id = self.findItemsButton.GetId())
        self.Bind(wx.EVT_TEXT_ENTER, self.OnJoinConference, id = self.joinConferenceTextCtrl.GetId())

        self.rightBox.Add(conferenceServerText, 0, wx.EXPAND)
        self.rightBox.Add(self.findItemsTextCtrl, 0, wx.EXPAND)
        self.rightBox.Add(self.findItemsButton, 0, wx.EXPAND)
        self.rightBox.Add(horizontalLine1, 0, wx.EXPAND | wx.ALL, 5)
        self.rightBox.Add(conferenceNicknameText, 0, wx.EXPAND)
        self.rightBox.Add(self.roomNickTextCtrl, 0, wx.EXPAND)
        self.rightBox.Add(joinConferenceText, 0, wx.EXPAND)
        self.rightBox.Add(self.joinConferenceTextCtrl, 0, wx.EXPAND)
        self.rightBox.Add(horizontalLine2, 0, wx.EXPAND | wx.ALL, 5)
        self.rightBox.Add(filterText, 0, wx.EXPAND)
        
        self.rightBox.SetMinSize( (200, -1) )

        self.hbox.Add(self.availableConferencesList, 1, wx.EXPAND)
        self.hbox.Add(self.rightBox, 0, wx.EXPAND | wx.ALL, 5)

        self.SetSizer(self.hbox)

    def OnFind(self, event):
        server = self.findItemsTextCtrl.GetValue()
        self.availableConferencesList.DeleteAllItems()
        self.findItems(server)

    def OnActivated(self, event):
        id = self.availableConferencesList.GetFocusedItem()
        item = self.availableConferencesList.GetItem(id, 1)
        label = item.m_text
        roomnick = self.roomNickTextCtrl.GetValue()
        self.notify(self, 'conferenceSelected', label, roomnick)

    def OnJoinConference(self, event):
        conferenceName = self.joinConferenceTextCtrl.GetValue()
        roomnick = self.roomNickTextCtrl.GetValue()
        self.notify(self, 'conferenceSelected', conferenceName, roomnick)

    def findItems(self, server):
        client = self.xmppHandler.getClient()
        self.availableConferences = xmpp.features.discoverItems(client, server)
        for conferenceInfo in self.availableConferences:
            try:
                if 'name' in conferenceInfo:
                    name = conferenceInfo['name']
                else:
                    name = 'N/A'
                jid = conferenceInfo['jid']
                self.availableConferencesList.Append( [name, jid] )
            except(UnicodeEncodeError):
                print 'Exception in conferenceInfo'


class ChatJoinedRoomsPanel(wx.Panel):
    def __init__(self, parent, xmppHandler):
        wx.Panel.__init__(self, parent)
        self.mParent = parent


        vbox = wx.BoxSizer(wx.VERTICAL)

        text =  wx.StaticText(self, -1, 'Events: NYI')

        vbox.Add(text, 1, wx.EXPAND)

        self.SetSizer(vbox)



class InfoGroupPanel(wx.Panel, Subscriber):
    def __init__(self, parent, xmppHandler):
        Subscriber.__init__(self)
        
        wx.Panel.__init__(self, parent)
        self.mParent = parent


        self.xmppHandler = xmppHandler
        self.subscribe(xmppHandler, 'xmppHandler')

        self.groupChats = {}

        self.vbox = wx.BoxSizer(wx.VERTICAL)

        self.closeGroupChatButton = wx.Button(self, -1, 'Close current group chat')
        self.noOpenGroupChat = wx.StaticText(self, -1, 'No group chat window opened. Groupchats can be found under the tab \'Available rooms\'.')

        self.vbox.Add(self.closeGroupChatButton, 0, wx.EXPAND)
        self.vbox.Add(self.noOpenGroupChat)

        self.Bind(wx.EVT_BUTTON, self.OnCloseGroupChat, id = self.closeGroupChatButton.GetId())

        self.currentVisibleChat = None

        self.SetSizer(self.vbox)


    def selectGroupChat(self, mucName): # maybe I should try to make a CardLayout class instead of implementing it directly
        self.noOpenGroupChat.Hide()
        if mucName not in self.groupChats.keys():
            self.addGroupChat(mucName)
        if self.currentVisibleChat:
            self.groupChats[self.currentVisibleChat].Hide()
        self.groupChats[mucName].Show()
        self.currentVisibleChat = mucName
        self.Layout()
        
    def addGroupChat(self, mucName):
        muc = self.xmppHandler.mucHandler.getMUC(mucName)
#        muc = self.xmppHandler.joinedMUCs[mucName]
        iGcp = InfoGroupChatPanel(self, self.xmppHandler, muc)
        self.groupChats[mucName] = iGcp
        
        self.vbox.Add(iGcp, 1, wx.EXPAND)
        self.Layout()

    def OnCloseGroupChat(self, event):
        if self.currentVisibleChat:
            self.xmppHandler.mucHandler.leaveMUC( self.currentVisibleChat )
            self.groupChats[self.currentVisibleChat].Destroy()
            del self.groupChats[self.currentVisibleChat]
        chatKeys = self.groupChats.keys()
        if chatKeys:
            self.chats[ chatKeys[0] ].Show()
            self.currentVisibleChat = chatKeys[0]
        else:
            self.noOpenGroupChat.Show()
            self.currentVisibleChat = None
        self.Layout()

    def update(self, publisher, *args):
        notificationType = args[1]
        print '\n\nInfoGroupPanel args:', args, '\n\n'
        if publisher == self.getPublisher('xmppHandler'):
            if notificationType == 'incomingGroupchatMessage':
                mucName = args[2]
                nickname = args[3]
                message = args[4]
                payload = args[5]
                if mucName not in self.groupChats:  # only needed if old messages arrives and, at present, no-one is in the room
                    self.addGroupChat(mucName)
                    self.selectGroupChat(mucName)
                    self.mParent.SetSelection(2)
                self.groupChats[mucName].newMsg(nickname, message, payload)
            elif notificationType == 'incomingGroupchatPresence':
                mucName = args[2]
                nickname = args[3]
                print "nickname:", nickname
                presence = args[4]
                if mucName not in self.groupChats:
                    print 'incomingGroupchatPresence new muc'
                    self.addGroupChat(mucName)
                    self.selectGroupChat(mucName)
                    self.mParent.SetSelection(2)
                print '\n\nself.groupChats[mucName].newPresence(nickname, presence):'
                self.groupChats[mucName].newPresence(nickname, presence)
                    # must be placed here because there's a risk that the presence is forwarded to the groupchatpanel that is not yet created
                    #   before the grouppanel gets the presence
                    # change this so that grouppanel is a publisher which groupchatpanels subscribes to and presence and messages are forwarded
                    #   to the subscribers
                







class InfoGroupChatPanel(wx.Panel, Subscriber):
    def __init__(self, parent, xmppHandler, muc):
       
        wx.Panel.__init__(self, parent)
        self.mParent = parent

        self.xmppHandler = xmppHandler

        self.muc = muc
        self.mucName = self.muc.getName()
        self.localNickname = self.muc.getLocalNickname()

        self.isHost = False
        self.hostName = 'N/A'
        self.hostSet = False
        self.hostIP = None
        self.serverBehindNAT = False
        #self.hostGUID = None
        #self.myGUIDString = None
        self.facilitatorAddress = None

        self.remoteInputPattern = '^<ege>(.+)$'
        self.extractInputsPattern = '^(.+?):(.*)$'
        self.extractHostPattern = '^(.+?):(.+)$'
        self.extractIPsWhenServerBehindNATPattern = '^(.+?):(.+)$'  # long variable name... =)


        hbox = wx.BoxSizer(wx.HORIZONTAL)
        chatVbox = wx.BoxSizer(wx.VERTICAL)
        hboxSend = wx.BoxSizer(wx.HORIZONTAL)
        rightBox = wx.BoxSizer(wx.VERTICAL)
        settingsBox = wx.BoxSizer(wx.VERTICAL)


        self.mucText = wx.StaticText(self, -1, 'MUC: ' + self.mucName)
        self.chat = wx.TextCtrl(self, -1, '', (-1, -1), (-1, -1), wx.TE_MULTILINE | wx.TE_READONLY | wx.TE_AUTO_URL | wx.TE_NOHIDESEL | wx.TE_RICH)
        self.chatBox = wx.TextCtrl(self, -1, '', (-1, -1), (-1, -1), wx.TE_PROCESS_ENTER | wx.TE_NOHIDESEL)
        self.sendButton = wx.Button(self, -1, 'Send message')

        self.Bind(wx.EVT_BUTTON, self.OnSend, id = self.sendButton.GetId())
        self.Bind(wx.EVT_TEXT_ENTER, self.OnSend, id = self.chatBox.GetId())

        self.usersText = wx.StaticText(self, -1, 'Users:')
        self.userList = wx.ListCtrl(self, -1, style = wx.LC_LIST | wx.LC_SINGLE_SEL)

        hostNameHbox = wx.BoxSizer(wx.HORIZONTAL)
        self.hostText = wx.StaticText(self, -1, 'Host: ')
        self.hostNameText = wx.StaticText(self, -1, self.hostName)
        hostNameHbox.Add(self.hostText)
        hostNameHbox.Add(self.hostNameText)

        self.startButton = wx.Button(self, -1, 'Start game as host')

        self.Bind(wx.EVT_BUTTON, self.OnStartGame, id = self.startButton.GetId())

        
        advancedBox = wx.StaticBox(self, -1, 'Advanced')
        advancedBoxSizer = wx.StaticBoxSizer(advancedBox, wx.VERTICAL)

        portHbox = wx.BoxSizer(wx.HORIZONTAL)
        self.portText = wx.StaticText(self, -1, 'Server port: ')
        self.portTextCtrl = wx.TextCtrl(self, -1, str(DEFAULT_SERVER_PORT), (-1, -1), (-1, -1), wx.TE_PROCESS_ENTER | wx.TE_NOHIDESEL)
        self.natCheckBox = wx.CheckBox(self, -1, 'Behind NAT')
        self.facilitatorAddressText = wx.StaticText(self, -1, 'Facilitator Address: ')
        self.facilitatorAddressTextCtrl = wx.TextCtrl(self, -1, '', (-1, -1), (-1, -1), wx.TE_PROCESS_ENTER | wx.TE_NOHIDESEL)

        portHbox.Add(self.portText)
        portHbox.Add(self.portTextCtrl, 1, wx.EXPAND)
        advancedBoxSizer.Add(portHbox)
        advancedBoxSizer.Add(self.natCheckBox)
        advancedBoxSizer.Add(self.facilitatorAddressText)
        advancedBoxSizer.Add(self.facilitatorAddressTextCtrl)

        settingsBox.Add(hostNameHbox, 0, wx.EXPAND)
        settingsBox.Add(self.startButton, 0, wx.EXPAND)
        settingsBox.Add(advancedBoxSizer, 0, wx.EXPAND)



        self.sendButton.SetMinSize((100, -1))
        self.userList.SetMinSize((200, -1))

        hboxSend.Add(self.chatBox, 1 , wx.EXPAND)
        hboxSend.Add(self.sendButton, 0, wx.EXPAND)

        chatVbox.Add(self.mucText, 0, wx.EXPAND)
        chatVbox.Add(self.chat, 1, wx.EXPAND)
        chatVbox.Add(hboxSend, 0, wx.EXPAND)

        rightBox.Add(self.usersText)
        rightBox.Add(self.userList, 1, wx.EXPAND)
        rightBox.Add(settingsBox, 1, wx.EXPAND)

        hbox.Add(chatVbox, 1, wx.EXPAND | wx.ALL, 5)
        hbox.Add(rightBox, 0, wx.EXPAND | wx.ALL, 5)

        self.SetSizer(hbox)

    def __del__(self):
        self.xmppHandler.detachSubscriber(self)

    def OnSend(self, event):
        msg = self.chatBox.GetValue()
        if msg: # chat box not empty
            self.xmppHandler.sendGroupchatMessage(self.muc, msg)
            #self.chat.AppendText(self.localNickname + ': ' + msg + '\n')
            self.chatBox.Clear()

    def newMsg(self, remoteNickname, message, payload):
        
        remoteInput = re.search(self.remoteInputPattern, message)
        if remoteInput: # special message
            delayed = len(payload)>=5 and payload[5].getName() == 'x'   # don't ask me what this 'x' really means, couldn't find 'delay' in the payload
                                                                        #   ... which is kinda weird...
            if not( delayed ):
                print 'remoteInput.groups()[0]:', remoteInput.groups()[0]
                extractedInputs = re.search(self.extractInputsPattern, remoteInput.groups()[0])
                inputType, data = extractedInputs.groups()
    
                if inputType == 'host': #   <ege>host:serverBehindNAT:hostIP[:facilitatorAddress]
                    #if not self.isHost:
                    serverBehindNATString, data = re.search(self.extractHostPattern, data).groups()
                    serverBehindNAT = (serverBehindNATString == 'True')
                    if serverBehindNAT:
                        print 'data:', data
                        hostIP, facilitatorAddress =  re.search(self.extractIPsWhenServerBehindNATPattern, data).groups()
                        print 'hostIP:', hostIP, 'faciliatorAddress:', facilitatorAddress
                        self.setHost(remoteNickname, hostIP, True, facilitatorAddress)
                    else:
                        hostIP = data
                        self.setHost(remoteNickname, hostIP, False)
                #elif inputType == 'client':
                else:
                    print 'Invalid inputType received:', inputType
        else:   # normal chat
            self.chat.AppendText(remoteNickname + ': ' + message + '\n')

    def newPresence(self, nickname, presence):
        self.userList.Append([nickname])
        if self.isHost:
            self._announceSelfAsHost()

    def setHost(self, hostName, hostIP, serverBehindNAT, facilitatorAddress=None):
        self.hostName = hostName
        self.hostSet = True
        self.hostIP = hostIP
        self.serverBehindNAT = serverBehindNAT
        self.facilitatorAddress = facilitatorAddress

        self.hostNameText.SetLabel(self.hostName)
        self.startButton.SetLabel('Start game as client')

    def OnStartGame(self, event):
        self.startButton.Hide()
        self.Layout()
        print 1
        if not self.hostSet:
            print 2
            self.isHost = True
            self.hostSet = True
            self.hostName = self.muc.getLocalNickname()
            self.serverBehindNAT = self.natCheckBox.GetValue()
            print 'self.serverBehindNAT:', self.serverBehindNAT

            print 2.1
            gh = GameHandler(GAME_DESTINATION)
            print 2.2
            self.facilitatorAddress = self.facilitatorAddressTextCtrl.GetValue()
            gh.startServer(MAX_NO_OF_PLAYERS, self.serverBehindNAT, self.facilitatorAddress)
            if self.serverBehindNAT:
                sleep(5)    # hopefully the server is connected to the facilitator before the clients are
            print 3
            self._announceSelfAsHost()
            print 4
        else:
            gh = GameHandler(GAME_DESTINATION)
            if self.serverBehindNAT:
                connected = gh.startClientThroughNAT(self.hostIP, self.facilitatorAddress)
                if not connected:
                    wx.MessageBox('Could not connect to the server.', 'Connection failed')
            else:
                connected = gh.startClientDirectConnect(self.hostIP)
                if not connected:
                    wx.MessageBox('Could not connect to the server.', 'Connection failed')

            
    def _announceSelfAsHost(self):
        print 'announces self as host!'
        if self.serverBehindNAT:
            self.xmppHandler.sendGroupchatMessage(self.muc, '<ege>host:' + 'True' + ':' + externalIP + ':' + self.facilitatorAddress)
        else:
            self.xmppHandler.sendGroupchatMessage(self.muc, '<ege>host:' + 'False' + ':' + externalIP)




class InfoPrivatePanel(wx.Panel, Subscriber):
    def __init__(self, parent, xmppHandler):
        Subscriber.__init__(self)
        
        wx.Panel.__init__(self, parent)
        self.mParent = parent


        mIp = parent.GetParent()
        mOp = mIp.GetParent()
        mRp = mOp.GetParent().mRp
        self.subscribe(mRp, 'rosterPanel')

        self.xmppHandler = xmppHandler
        self.subscribe(xmppHandler, 'xmppHandler')
        
        self.chats = {}

        self.vbox = wx.BoxSizer(wx.VERTICAL)

        self.closeChatButton = wx.Button(self, -1, 'Close current chat')
        self.noOpenChat = wx.StaticText(self, -1, 'No chat window opened, click on a contact in your contact panel to the left to open a new chat.')

        self.vbox.Add(self.closeChatButton, 0, wx.EXPAND)
        self.vbox.Add(self.noOpenChat)

        self.Bind(wx.EVT_BUTTON, self.OnCloseChat, id = self.closeChatButton.GetId())

        self.currentVisibleChat = None

        self.SetSizer(self.vbox)

    def selectChat(self, bareJID):
        self.noOpenChat.Hide()
        if bareJID not in self.chats.keys():
            self.addChat(bareJID)
        if self.currentVisibleChat:
            self.chats[self.currentVisibleChat].Hide()
        self.chats[bareJID].Show()
        self.currentVisibleChat = bareJID
        self.Layout()
        
    def addChat(self, bareJID):
        user = self.xmppHandler.getUserFromBareJID(bareJID)
        iPcp = InfoPrivateChatPanel(self, self.xmppHandler, user)
        self.chats[bareJID] = iPcp
        
        self.vbox.Add(iPcp, 1, wx.EXPAND)
        self.Layout()

    def OnCloseChat(self, event):
        if self.currentVisibleChat:
            self.chats[self.currentVisibleChat].Destroy()
            del self.chats[self.currentVisibleChat]
        chatKeys = self.chats.keys()
        if chatKeys:
            self.chats[ chatKeys[0] ].Show()
            self.currentVisibleChat = chatKeys[0]
        else:
            self.noOpenChat.Show()
            self.currentVisibleChat = None
        self.Layout()

    def update(self, publisher, *args):
        #print 'args, InfoPrivatePanel:', args
        notificationType = args[1]
        if publisher == self.getPublisher('rosterPanel'):
            if notificationType == 'contactSelected':
                jidString = args[2]
                self.selectChat(jidString)
                self.mParent.SetSelection(3)
        elif publisher == self.getPublisher('xmppHandler'):
            if notificationType == 'incomingPrivateMessage':
                remoteBareJID = args[2]
                if remoteBareJID not in self.chats:
                    self.addChat(remoteBareJID)
                    self.selectChat(remoteBareJID)
                    self.mParent.SetSelection(3)
                message = args[3]
                self.chats[remoteBareJID].newMsg(remoteBareJID, message)
                
                


class InfoTestPanel(wx.Panel):
    def __init__(self, parent, xmppHandler):
        wx.Panel.__init__(self, parent)
        self.mParent = parent

        vbox = wx.BoxSizer(wx.VERTICAL)
        b = wx.StaticText(self, -1, 'yo')
        a = wx.TextCtrl(self, -1, '')
        vbox.Add(a, 1, wx.EXPAND)
        vbox.Add(b, 1, wx.EXPAND)


        self.SetSizer(vbox)



class InfoPrivateChatPanel(wx.Panel, Subscriber):
    def __init__(self, parent, xmppHandler, user):
        wx.Panel.__init__(self, parent)
        self.mParent = parent

        self.xmppHandler = xmppHandler

        self.user = user

        vbox = wx.BoxSizer(wx.VERTICAL)
        hboxSend = wx.BoxSizer(wx.HORIZONTAL)

        self.bareJID = self.user.getBareJID()

        self.jidText = wx.StaticText(self, -1, 'JID: ' + self.bareJID)
        self.chat = wx.TextCtrl(self, -1, '', (-1, -1), (-1, -1), wx.TE_MULTILINE | wx.TE_READONLY | wx.TE_AUTO_URL | wx.TE_NOHIDESEL | wx.TE_RICH)
        self.chatBox = wx.TextCtrl(self, -1, '', (-1, -1), (-1, -1), wx.TE_PROCESS_ENTER | wx.TE_NOHIDESEL)
        self.sendButton = wx.Button(self, -1, 'Send message')

        self.Bind(wx.EVT_BUTTON, self.OnSend, id = self.sendButton.GetId())
        self.Bind(wx.EVT_TEXT_ENTER, self.OnSend, id = self.chatBox.GetId())

        self.sendButton.SetMinSize((100, -1))

        hboxSend.Add(self.chatBox, 1 , wx.EXPAND)
        hboxSend.Add(self.sendButton)

        vbox.Add(self.jidText, 0, wx.EXPAND)
        vbox.Add(self.chat, 1, wx.EXPAND)
        vbox.Add(hboxSend, 0, wx.EXPAND)

        self.SetSizer(vbox)

    def __del__(self):
        self.xmppHandler.detachSubscriber(self)

    def OnSend(self, event):
        msg = self.chatBox.GetValue()
        localBareJID = self.xmppHandler.getLocalBareJID()
        if msg: # chat box not empty
            recipient = self.user.getBareJID()
            self.xmppHandler.sendChatMsg(recipient, msg)
            self.chat.AppendText(localBareJID + ': ' + msg + '\n')
            self.chatBox.Clear()

    def newMsg(self, remoteBareJID, message):
        self.chat.AppendText(remoteBareJID + ': ' + message + '\n')



ID_EXIT = 0
ID_PYEGE_HELP = 1
ID_ABOUT = 2


class OnlinePanel(wx.Frame):
    def __init__(self, parent, id, title, xmppHandler):
        wx.Frame.__init__(self, parent, id, title)

        self.xmppHandler = xmppHandler

        self.mParentOnline = wx.Panel(self, -1)

        self.statusBar = self.CreateStatusBar()

        menubar = wx.MenuBar()
        
        fileMenu = wx.Menu()
        quitItem = wx.MenuItem(fileMenu, ID_EXIT, '&Exit')
        fileMenu.AppendItem(quitItem)
        menubar.Append(fileMenu, '&File')
        
        helpMenu = wx.Menu()
        pyegeHelpItem = wx.MenuItem(helpMenu, ID_PYEGE_HELP, '&PyEGE Help')
        helpMenu.AppendItem(pyegeHelpItem)
        aboutItem = wx.MenuItem(helpMenu, ID_ABOUT, '&About')
        helpMenu.AppendItem(aboutItem)
        menubar.Append(helpMenu, '&Help')
        
        self.SetMenuBar(menubar)

        self.Bind(wx.EVT_MENU, self.OnQuit, id=quitItem.GetId())
        self.Bind(wx.EVT_MENU, self.OnAbout, id=aboutItem.GetId())
        self.Bind(wx.EVT_MENU, self.OnPyEGEHelp, id=pyegeHelpItem.GetId())

        self.hbox = wx.BoxSizer(wx.HORIZONTAL)

        self.mLip = LoginPanel(self.mParentOnline, self.xmppHandler)
        self.mRp = RosterPanel(self.mParentOnline, self.xmppHandler)
        self.mIp = InfoPanel(self.mParentOnline, self.xmppHandler)

        #self.xmppHandler.subscribe(

        self.vboxLeft = wx.BoxSizer(wx.VERTICAL)
        self.vboxRight = wx.BoxSizer(wx.VERTICAL)

        self.vboxLeft.Add(self.mLip, 0, wx.EXPAND | wx.ALL, 5)
        self.vboxLeft.Add(self.mRp, 1, wx.EXPAND | wx.ALL, 5)
        self.vboxLeft.SetMinSize((200, -1))

        self.vboxRight.Add(self.mIp, 1, wx.EXPAND | wx.ALL, 5)

        self.hbox.Add(self.vboxLeft, 0, wx.EXPAND)
        self.hbox.Add(self.vboxRight, 1, wx.EXPAND)

        self.Bind(wx.EVT_IDLE, self.OnIdle)


        self.SetMinSize((640, 480))
        self.mParentOnline.SetSizer(self.hbox)
        self.Centre()

        self.Show(True)

    def OnIdle(self, event):
        self.xmppHandler.process()
        event.RequestMore()
            
    def OnQuit(self, event):
        self.Close()

    def OnPyEGEHelp(self, event):
        helpText = \
"As this is a beta version not all features works and, of the existing features, not all of them works as they should. If something doesn't work, \
which have previously worked, the easiest way to get it working again is often just to restart the client. Features that do not work for today are \
labled 'NYI' (Not Yet Implemented). \
\nTo add contacts to your list you will have to download another XMPP client, add the contact there and then log back in to PyEGE. \
Your added contacts should now be visible under contact list below the login panel. However, only contacts that are online are visible in this list. \
\nFor a direct connect to work ('Behind NAT' is unchecked) the host should not be behind a NAT (a.k.a. router or hub). \
If you can't find a host that is not behind a NAT you might \
you might still be able to connect by using a so called NPT (NAT Punch-Through). However, for this to work you'll need a facilitator server that is not be behind a NAT. \
To connect through NPT a user, not behind a NAT, starts startFacilitator.bat. The facilitators IP will be displayed on the screen so that he or she can \
send this to the host. This will have to be done manually. The host clicks on the checkbox 'Behind NAT' so that it is activated and enters the facilitators IP. \
After that the host clicks on 'Start game as host'. After about five seconds the other users in the room will have this button changed to 'Start game as client.' \
When the clients click on this button they will hopefully be able to connect to the host through the facilitator. The facilitator can now shut down the server \
when the clients have connected to the host. \
\nFor more help see the 'FAQ' on the homepage."
        wx.MessageBox(helpText, 'PyEGE Help')

    def OnAbout(self, event):
        aboutText = 'PyEGE ' + VERSION + '\nDeveloper: Martin Nordström\nhttp://pyege.squarespace.com/\nhttp://github.com/pyege/PyEGE\nLicense: See license/lgpl.txt\n\n' + \
'''Dependencies:
OpenGL
http://www.opengl.org/

GLFW
http://glfw.sourceforge.net/

GLee
http://elf-stone.com/glee.php

DevIL
http://openil.sourceforge.net/

Python
http://python.org/

Cython
http://www.cython.org/

NumPy
http://numpy.scipy.org/

XMPP
http://xmpp.org/

xmpppy
http://xmpppy.sourceforge.net/

wxPython
http://www.wxpython.org/

RakNet (3.23)
http://www.jenkinssoftware.com/raknet/index.html

FMOD
FMOD Sound System', copyright © Firelight Technologies Pty, Ltd., 1994-2009.
http://www.fmod.org/

a-star-algorithm-implementation
http://code.google.com/p/a-star-algorithm-implementation/

See the directory 'dependencies' for more information'''\
        + '\n\nSpecial Thanks to:\nAnders Bennehag'
        
        wx.MessageBox(aboutText, 'About')






class GameHandler(Thread):
    def __init__(self, filename):
        Thread.__init__(self)
        ege.externalInitAll()
        self.filename = filename
        self.isServer = None

    def __del__(self):
        ege.externalCloseAll()

    def run(self):
        execfile(self.filename)	# change to sanboxed PyPy

    def startServer(self, maxConnectionsAllowed, behindNAT, facilitatorAddress=None):
        self.isServer = True
        print '2.2.1'
        ege.externalStartServer(maxConnectionsAllowed, behindNAT, facilitatorAddress)
        print '2.2.2'
        self.start()
        print '2.2.3'

    def startClientDirectConnect(self, serverIP):
        self.isServer = False

        ege.externalStartClient(serverIP, False)	# ege.externalStartClient(serverIP, False)
        connected = False
        for i in range(100):
            sleep(0.1)
            packet = ege.receiveData()
            if packet:
                print packet
            if packet and packet['type'] == 'connected':
                connected = True
                self.start()
                break
        return connected
    
        
    def startClientThroughNAT(self, serverIP, facilitatorAddress):    # all NAT stuff is pretty much spaghetti code =/... better work that out some day...
        self.isServer = False
        ege.externalStartClient(serverIP, True, facilitatorAddress)
        connected = False
        for i in range(100):
            sleep(0.1)
            packet = ege.receiveData()
            if packet:
                print packet
            if packet and packet['type'] == 'connected':
                connected = True
                self.start()
                break
        return connected




                
                

class ChatHandler(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        myXMPPHandler = xmppHandler()
        app = wx.PySimpleApp()
        mainFrame = OnlinePanel(None, wx.ID_ANY, 'PyEGE '+ VERSION, myXMPPHandler)
        app.MainLoop()

    def setGameHandler(self, gameHandler):
        self.gameHandler = gameHandler


ch = ChatHandler()
ch.start()



        
        


        
        






