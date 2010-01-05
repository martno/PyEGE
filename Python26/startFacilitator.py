import ege
ege.externalInitAll()
ege.externalStartFacilitator()

import urllib
externalIP = urllib.urlopen("http://www.whatismyip.com/automation/n09230945.asp").read()

print 'Your external IP is:'
print externalIP
print 
print 'Press ESC to exit'

ege.newWindow(100, 100)
while not ege.getKey(ege.KEY_ESC):
	packet = ege.receiveData()
	ege.update()

ege.externalCloseAll()

