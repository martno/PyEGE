#ifndef TIMER_H
#define TIMER_H

#include <GL/GLee.h>	// Must be included before GLFW
#include <GL/glfw.h>	// For GLFW, OpenGL and GLU

class Timer {
public:
	Timer(double time=0, bool start=true);
	~Timer();

	void resume();
	void pause();
	void togglePause();
	void stop();
	void setTime(double time);
	double getTime();
	void addTime(double time);
	bool isRunning();
	
private:
	bool running;
	double startTime;
	double pausedAt;
};


#endif TIMER_H
