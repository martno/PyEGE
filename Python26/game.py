import ege
import numpy



ege.newWindow(640, 480, ege.WINDOW)


snd1 = ege.Sound('BEEPPURE.WAV.wav', False)
# This is a Public Domain sound

img1 = ege.Image('bild1.PNG')
print img1.getWidth()

timer = ege.Timer()

ege.setClearColor((255, 255, 254))
#ce.showMouseCursor(True)


while not ege.getKey(ege.KEY_ESC):
	mouseState = ege.getMouseState()
	if mouseState['left']:
		snd1.play()
	packet = ege.receiveData()
	if packet:
		if packet['from'] != 'system':
			print packet
		if packet['type'] == 'connected':
			ege.sendData([3, 5, 'halo!'])
	img1.draw(100, 200, color=ege.RED, rotate=timer.getTime()*45)
	ege.drawEllipse((300, 300), (255, 255, 125), 20, 40, rotate=timer.getTime()*45)
	ege.drawPoints(((timer.getTime()*10, 300)), ege.BLACK, size = 10, antiAliasing=True)
	ege.update()




