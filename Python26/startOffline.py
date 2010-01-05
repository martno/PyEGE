import ege

ege.externalInitAll()

try:
	execfile('game.py')
finally:
	ege.externalCloseAll()
