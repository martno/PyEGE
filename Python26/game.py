import ege
import numpy


print 'beginning of game.py 1'
ege.newWindow(640, 480, ege.WINDOW)
print 'beginning of game.py 2'

snd1 = ege.Sound('BEEPPURE.WAV.wav', False)
# This is a Public Domain sound

img1 = ege.Image('bild1.PNG')
print img1.getWidth()

timer = ege.Timer()

ege.setClearColor((255, 255, 254))
#ce.showMouseCursor(True)

ege.sendData([3, 5, 'halo!'])
while not ege.getKey(ege.KEY_ESC):
	#ege.sendData([3, 5, 'halo!'])
	mouseState = ege.getMouseState()
	if mouseState['left']:
		snd1.play()
	packet = ege.receiveData()
	if packet:
		print packet
	img1.draw(100, 200, color=ege.RED, rotate=timer.getTime()*45)
	ege.drawEllipse((300, 300), (255, 255, 125), 20, 40, rotate=timer.getTime()*45)
	#ege.drawPoints(((timer.getTime()*10, 300)), ege.BLACK, size = 10, antiAliasing=True)
	ege.update()




