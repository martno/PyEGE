#include "ResourceManager.h"
#include <physfs.h>



ResourceManager::ResourceManager()
{
	int loadOK = PHYSFS_init(NULL);
	
	//if( !loadOK ) {
	//	cout << "could not initialize PhysicsFS" << endl;
	//}

	string mainPath( PHYSFS_getBaseDir() );
	mainPath.append("/main.edge");

	PHYSFS_addToSearchPath(mainPath.c_str(), 0);

}


ResourceManager::~ResourceManager()
{
	PHYSFS_deinit();
}



void ResourceManager::loadApplication(char* appname)
{
	// Add to search path
	appPath = string( PHYSFS_getBaseDir() );
	appPath.append("/apps/");
	appPath.append(appname);
	PHYSFS_addToSearchPath(appPath.c_str(), 1);

	// Set write dir
	writePath = string(appPath);
	writePath.append("/writedir/");
	PHYSFS_setWriteDir( writePath.c_str() );

}



bool ResourceManager::loadFile(char* filename, PHYSFS_file* fileHandle, char* fileBuffer)
{

	int fileExists = PHYSFS_exists(filename);
	if(fileExists) {
		fileHandle = PHYSFS_openRead(filename);
		PHYSFS_sint64 fileSize = PHYSFS_fileLength(fileHandle);

		//char* fileBuffer;	// if this is returned
		fileBuffer = new char[(unsigned int)fileSize];	//new char[PHYSFS_fileLength(fileHandle)];
		int lengthRead = PHYSFS_read(fileHandle, fileBuffer, 1, fileSize);

		return true;
	}
	else {
		return false;
	}

}


void ResourceManager::closeFile(PHYSFS_file* fileHandle)
{
	PHYSFS_close(fileHandle);
}





