#include "Timer.h"

#include <GL/GLee.h>	// Must be included before GLFW
#include <GL/glfw.h>	// For GLFW, OpenGL and GLU

Timer::Timer(double time, bool start) {
	if(start) {
		startTime = glfwGetTime() - time;
		running = true;
	}
	else {
		pausedAt = time;
		running = false;
	}
}

Timer::~Timer() {

}

void Timer::resume() {
	if(!running) {
		startTime = glfwGetTime() - pausedAt;
		running = true;
	}
}

void Timer::pause() {
	if(running) {
		pausedAt = glfwGetTime() - startTime;
		running = false;
	}
}

void Timer::togglePause() {
	if(running) {
		pause();
	}
	else {
		resume();
	}
}

void Timer::stop() {
	pause();
	setTime(0);
}

void Timer::setTime(double time) {
	if(running) {
		startTime = glfwGetTime() - time;
	}
	else {
		pausedAt = time;
	}
}

double Timer::getTime() {
	if(running) {
		return glfwGetTime() - startTime;
	}
	else {
		return pausedAt;
	}
}

void Timer::addTime(double time) {
	if(running) {
		startTime = startTime - time;	// startTime -= time
	}
	else {
		pausedAt = pausedAt + time;		// pausedAt += time
	}
}

bool Timer::isRunning() {
	return running;
}
