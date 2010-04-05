#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H



#include <physfs.h>

#include <string>

using namespace std;

// Se till att varje fil endast laddas en gång om inget annat anges
class ResourceManager	// : public Singleton	// maybe not, play several games at the same time...
{
public:
	ResourceManager();
	~ResourceManager();

	void loadApplication(char* appname);

	// endast returnera filen istället, låt ResourceManagern mappa filer mot fileHandles?
	// om filen inte hittade returnera isf NULL
	bool loadFile(char* filename, PHYSFS_file* fileHandle, char* fileBuffer);
	void closeFile(PHYSFS_file* fileHandle);



private:
	string appPath;
	string writePath;


};

#endif RESOURCE_MANAGER_H

