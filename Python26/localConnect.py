import ege

import game


MAX_NO_OF_PLAYERS = 16


ege.externalInitAll()



print '(S)erver or (C)lient?'

answer = raw_input()

if answer.upper() == 'S':
	isServer = True
elif answer.upper() == 'C':
	isServer = False
else:
	raise Exception("Invalid input:", answer, ", must be either 'S' or 'C'")




if isServer:
	ege.externalStartServer(MAX_NO_OF_PLAYERS, False, '')
else:
	ege.externalStartClient('127.0.0.1', False)


done = False


try:
	while not done:
		packet = ege.receiveData()
		if packet:
			print packet
			if packet['type'] == 'connected' or packet['type'] == 'incomingConnection':
				game.main()
				done = True
finally:
	ege.externalCloseAll()

