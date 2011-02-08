
#import numpy


### CONSTANTS ###

#enum TriangleForms { SEPARATE, STRIP, FAN } :
SEPARATE = 0
STRIP = 1
FAN = 2

MAX_SIZE_OF_PACKET = 256

WINDOW = False
FULLSCREEN = True


MOUSE_BUTTON_LEFT = 0
MOUSE_BUTTON_RIGHT = 1
MOUSE_BUTTON_MIDDLE = 2

KEY_UNKNOWN      =-1
KEY_SPACE        =32
KEY_SPECIAL      =256
KEY_ESC          =(KEY_SPECIAL+1)
KEY_F1           =(KEY_SPECIAL+2)
KEY_F2           =(KEY_SPECIAL+3)
KEY_F3           =(KEY_SPECIAL+4)
KEY_F4           =(KEY_SPECIAL+5)
KEY_F5           =(KEY_SPECIAL+6)
KEY_F6           =(KEY_SPECIAL+7)
KEY_F7           =(KEY_SPECIAL+8)
KEY_F8           =(KEY_SPECIAL+9)
KEY_F9           =(KEY_SPECIAL+10)
KEY_F10          =(KEY_SPECIAL+11)
KEY_F11          =(KEY_SPECIAL+12)
KEY_F12          =(KEY_SPECIAL+13)
KEY_F13          =(KEY_SPECIAL+14)
KEY_F14          =(KEY_SPECIAL+15)
KEY_F15          =(KEY_SPECIAL+16)
KEY_F16          =(KEY_SPECIAL+17)
KEY_F17          =(KEY_SPECIAL+18)
KEY_F18          =(KEY_SPECIAL+19)
KEY_F19          =(KEY_SPECIAL+20)
KEY_F20          =(KEY_SPECIAL+21)
KEY_F21          =(KEY_SPECIAL+22)
KEY_F22          =(KEY_SPECIAL+23)
KEY_F23          =(KEY_SPECIAL+24)
KEY_F24          =(KEY_SPECIAL+25)
KEY_F25          =(KEY_SPECIAL+26)
KEY_UP           =(KEY_SPECIAL+27)
KEY_DOWN         =(KEY_SPECIAL+28)
KEY_LEFT         =(KEY_SPECIAL+29)
KEY_RIGHT        =(KEY_SPECIAL+30)
KEY_LSHIFT       =(KEY_SPECIAL+31)
KEY_RSHIFT       =(KEY_SPECIAL+32)
KEY_LCTRL        =(KEY_SPECIAL+33)
KEY_RCTRL        =(KEY_SPECIAL+34)
KEY_LALT         =(KEY_SPECIAL+35)
KEY_RALT         =(KEY_SPECIAL+36)
KEY_TAB          =(KEY_SPECIAL+37)
KEY_ENTER        =(KEY_SPECIAL+38)
KEY_BACKSPACE    =(KEY_SPECIAL+39)
KEY_INSERT       =(KEY_SPECIAL+40)
KEY_DEL          =(KEY_SPECIAL+41)
KEY_PAGEUP       =(KEY_SPECIAL+42)
KEY_PAGEDOWN     =(KEY_SPECIAL+43)
KEY_HOME         =(KEY_SPECIAL+44)
KEY_END          =(KEY_SPECIAL+45)
KEY_KP_0         =(KEY_SPECIAL+46)
KEY_KP_1         =(KEY_SPECIAL+47)
KEY_KP_2         =(KEY_SPECIAL+48)
KEY_KP_3         =(KEY_SPECIAL+49)
KEY_KP_4         =(KEY_SPECIAL+50)
KEY_KP_5         =(KEY_SPECIAL+51)
KEY_KP_6         =(KEY_SPECIAL+52)
KEY_KP_7         =(KEY_SPECIAL+53)
KEY_KP_8         =(KEY_SPECIAL+54)
KEY_KP_9         =(KEY_SPECIAL+55)
KEY_KP_DIVIDE    =(KEY_SPECIAL+56)
KEY_KP_MULTIPLY  =(KEY_SPECIAL+57)
KEY_KP_SUBTRACT  =(KEY_SPECIAL+58)
KEY_KP_ADD       =(KEY_SPECIAL+59)
KEY_KP_DECIMAL   =(KEY_SPECIAL+60)
KEY_KP_EQUAL     =(KEY_SPECIAL+61)
KEY_KP_ENTER     =(KEY_SPECIAL+62)
KEY_LAST         =KEY_KP_ENTER


WHITE  = (255, 255, 255)
RED    = (255,   0,   0)
GREEN  = (  0, 255,   0)
BLUE   = (  0,   0, 255)
YELLOW = (255, 255,   0)
PINK   = (255,   0, 255)
CYAN   = (  0, 255, 255)
BLACK  = (  0,   0,   0)



HIGH_PRIORITY = 1
MEDIUM_PRIORITY = 2
LOW_PRIORITY = 3

UNRELIABLE = 0
UNRELIABLE_SEQUENCED = 1
RELIABLE = 2
RELIABLE_ORDERED = 3
RELIABLE_SEQUENCED = 4



cdef extern from "stdlib.h":
    ctypedef unsigned long size_t 
    void free(void *ptr)
    void *malloc(size_t size)
    void *realloc(void *ptr, size_t size)
    size_t strlen(char *s)
    char *strcpy(char *dest, char *src)


#cdef extern from "arrayobject.h":
#    ctypedef int intp
#    ctypedef extern class numpy.numpy.ndarray [object PyArrayObject]:
#        cdef char *data
#        cdef int nd
#        cdef intp *dimensions
#        cdef intp *strides
#        cdef int flags


import numpy
cimport numpy

import cPickle
#cimport cPickle






cdef extern from "WindowHandler.h":
    ctypedef struct c_WindowHandler "WindowHandler":
        void cNewWindow(int width, int height, bint fullscreen)
        void cCloseWindow()
        void cSetWindowSize(int width, int height)
        double cGetTime()
        void cSetFPS(int newFPS)
        bint cGetKey(int key)
        void cGetMousePos(int *x, int *y)
        bint cGetMouseButton(int button)
        int cGetMouseWheel()
        void cShowMouseCursor(bint show)
        void cUpdate()
        void cDrawPoints(double *coords, int dimension, unsigned char *color, double size, bint antiAliasing)
        void cDrawLines(double *coords, int dimension, unsigned char *color, double width, bint loop, \
                                        bint stipple, int stippleFactor, int stipplePattern, bint antiAliasing)
        void cDrawTriangles(double *coords, int dimension, unsigned char *color, int form)
        void cDrawPolygon(double *coords, int dimension, unsigned char *color)
        void cDrawEllipse(double x, double y, double a, double b, double alpha , unsigned char *color)
        void cSetClearColor(unsigned char *color)
        
        void cLoadMap(int* cMap, int width, int height)
        int cComputePath(int startNodeX, int startNodeY, int endNodeX, int endNodeY)
        void cGetPath(int *pathData)
        
        #c_Image *newImage(char *filename)

        #void bindFBO(GLuint texName, GLuint width, GLuint height);
        #void releaseFBO();
    c_WindowHandler *new_WindowHandler "new WindowHandler" ()
    void del_WindowHandler "delete" (c_WindowHandler *wh)








#cdef class WindowHandler:
#    cdef c_WindowHandler *thisPtr
#    def __cinit__(self):
#        self.thisPtr = new_WindowHandler()


#    def __dealloc__(self):
#        del_WindowHandler(self.thisPtr)

#    def newWindow(self, int width, int height, bint fullscreen):
#        self.thisPtr.cNewWindow(width, height, fullscreen)

#    def update(self):
#        self.thisPtr.cUpdate()






cdef extern from "ConnectionHandler.h":
    ctypedef struct c_ConnectionHandler "ConnectionHandler":
        char *startServer(int maxConnectionsAllowed, bint behindNAT, char *facilitatorAddress)
        void startFacilitator()
        char *startClient(char *serverIP, bint serverBehindNAT, char *facilitatorAddress)
        #void natSetRemoteGUID(char *remoteGUIDString)
        void sendData(char *pickledObject, int priority, int reliability, int channel, bint timeStamp, char *svUser, bint svBroadcast)
        void receiveData(char *type, char *data, int *delay, bint *timeStamped, char *fromUser, bint *packetArrived)
        bint getIsServer()
    c_ConnectionHandler *new_ConnectionHandler "new ConnectionHandler" ()
    void del_ConnectionHandler "delete" (c_ConnectionHandler *conH)



def externalStartServer(int maxConnectionsAllowed, bint behindNAT, facilitatorAddress=''):
#    cdef char *guidString
#    guidString = conH.startServer(maxConnectionsAllowed, behindNAT, facilitatorAddress)
#    if guidString != NULL:
#        pGUIDString = guidString	# to python object
#        return pGUIDString
#    else:
#        return None
    conH.startServer(maxConnectionsAllowed, behindNAT, facilitatorAddress)



def externalStartFacilitator():
    conH.startFacilitator()


def externalStartClient(char *serverIP, bint serverBehindNAT, facilitatorAddress=''):
#    cdef char *guidString
#    guidString = conH.startClient(serverIP, serverBehindNAT, facilitatorAddress)
#    if guidString != NULL:
#        pGUIDString = guidString	# to python object
#        return pGUIDString
#    else:
#        return None
    conH.startClient(serverIP, serverBehindNAT, facilitatorAddress)





#def externalNATSetRemoteGUID(remoteGUIDString):
#    conH.natSetRemoteGUID(remoteGUIDString)


def sendData(data, int priority=HIGH_PRIORITY, int reliability=RELIABLE_ORDERED, int channel=0, bint timeStamp=True, svUsers=None, bint svBroadcast=True):
    pickledObject = cPickle.dumps(data)

    cdef char *cSvUsers

    if( len(pickledObject) < MAX_SIZE_OF_PACKET and priority>=0 and priority<=2 and reliability>=0 and reliability<=5 and channel>=0 and channel<=31):
        if(svUsers is None):
             cSvUsers = NULL
        else:
             cSvUsers = svUsers

        conH.sendData(pickledObject, priority, reliability, channel, timeStamp, cSvUsers, svBroadcast)

    else:
        pass # raise something




def receiveData():
    cdef char type[512]
    cdef char data[512]
    cdef int delay
    cdef bint timeStamped
    cdef char fromUser[512]
    cdef bint packetArrived

    conH.receiveData(type, data, &delay, &timeStamped, fromUser, &packetArrived)

    if packetArrived:
        pType = type	# to pythonobject
        if(pType == 'userDefinedData'):
            userData = cPickle.loads(data)
            if(timeStamped):
                return {'type': pType, 'data': userData, 'delay': delay, 'from': fromUser}
            else:
                return {'type': pType, 'data': userData, 'delay': None, 'from': fromUser}

        else:
            return {'type': pType, 'data': None, 'delay': None, 'from': 'system'}
    else:
        return None
        


def isServer():
    return conH.getIsServer()











cdef extern from "SoundHandler.h":
    ctypedef struct c_SoundHandler "SoundHandler":
        void update()

    c_SoundHandler *new_SoundHandler "new SoundHandler" ()
    void del_SoundHandler "delete" (c_SoundHandler *sh)




#cdef class SoundHandler:
#    cdef c_SoundHandler *thisPtr
#    def __cinit__(self):
#        self.thisPtr = new_SoundHandler()
#
#    def __dealloc__(self):
#        del_SoundHandler(self.thisPtr)



cdef c_WindowHandler *wh
cdef c_SoundHandler *sh
cdef c_ConnectionHandler *conH

def externalInitAll():
    global wh
    global sh
    global conH
    wh = new_WindowHandler()
    sh = new_SoundHandler()
    conH = new_ConnectionHandler()


def externalCloseAll():
    del_WindowHandler(wh)
    del_SoundHandler(sh)
    del_ConnectionHandler(conH)


def update():
    wh.cUpdate()
    sh.update()








def newWindow(int width, int height, bint fullscreen = False):
    wh.cNewWindow(width, height, fullscreen)


def closeWindow():
    wh.cCloseWindow()

def setWindowSize(int width, int height):
    wh.cSetWindowSize(width, height)

def setFPS(int newFPS):
    wh.cSetFPS(newFPS)



def showMouseCursor(bint show): # doesn't seem to work..
    wh.cShowMouseCursor(show)






def loadMap( map ):
    cdef int height = len( map )
    cdef int width = len( map[0] )
    
    cdef numpy.ndarray newMap = numpy.array(map, dtype=numpy.int32)
    
    cdef int* cMap = <int*>newMap.data
    
    wh.cLoadMap(cMap, width, height)


def getPath(startNode, endNode):
    pathLength = wh.cComputePath(startNode[0], startNode[1], endNode[0], endNode[1]) + 1	# seriously, why do i need to add 1??

    cdef numpy.ndarray[numpy.int32_t, ndim=2] path = numpy.zeros((pathLength, 2), dtype=numpy.int32)
    wh.cGetPath( <int*>path.data )

    return numpy.array(path)







def drawPoints(coords, color, float size = 1.0, bint antiAliasing = False):
    
    cdef numpy.ndarray newCoords = numpy.array(coords, dtype = float)

    cdef double *cCoords = <double *>newCoords.data
    cdef int ndim = newCoords.ndim
    cdef int shape = newCoords.shape[0]
    cdef int dim
    if ndim == 1:	# I really don't lik this, there gotta be an easier way...
        dim = 1
    else:
        dim = shape


    cdef int colorLength = len(color)
    cdef unsigned char *arrayColor = <unsigned char *>malloc(4 * sizeof(unsigned char))
    arrayColor[0] = color[0]
    arrayColor[1] = color[1]
    arrayColor[2] = color[2]
    if( colorLength == 3):
        arrayColor[3] = 255
    elif( colorLength == 4):
        arrayColor[3] = color[3]
    #else:
    #    kasta exception

    wh.cDrawPoints(cCoords, dim, arrayColor, size, antiAliasing)
    free(arrayColor)



def drawLines(coords, color, float width = 1, bint loop = False, stipple = False, bint antiAliasing = False):

    cdef numpy.ndarray newCoords = numpy.array(coords, dtype = float)

    cdef double *cCoords = <double *>newCoords.data
    cdef int dim = newCoords.shape[0]

    cdef int colorLength = len(color)
    cdef unsigned char *arrayColor = <unsigned char *>malloc(4 * sizeof(unsigned char))
    arrayColor[0] = color[0]
    arrayColor[1] = color[1]
    arrayColor[2] = color[2]
    if( colorLength == 3):
        arrayColor[3] = 255
    elif( colorLength == 4):
        arrayColor[3] = color[3]
    #else:
    #    kasta exception

    if( stipple and len(stipple)==2 ):
        stippleFactor = stipple[0]
        stipplePattern = stipple[1]
    else:    # Aer so illa tvungen att ge dessa ett vaerde...
        stippleFactor = 1
        stipplePattern = 0xFFFF


    wh.cDrawLines(cCoords, dim, arrayColor, width, loop, stipple, stippleFactor, stipplePattern, antiAliasing)
    free(arrayColor)



def drawTriangles(coords, color, int form = SEPARATE):

    cdef numpy.ndarray newCoords = numpy.array(coords, dtype = float)

    cdef double *cCoords = <double *>newCoords.data
    cdef int dim = newCoords.shape[0]

    cdef int colorLength = len(color)
    cdef unsigned char *arrayColor = <unsigned char *>malloc(4 * sizeof(unsigned char))
    arrayColor[0] = color[0]
    arrayColor[1] = color[1]
    arrayColor[2] = color[2]
    if( colorLength == 3):
        arrayColor[3] = 255
    elif( colorLength == 4):
        arrayColor[3] = color[3]
    #else:
    #    kasta exception

    wh.cDrawTriangles(cCoords, dim, arrayColor, form)
    free(arrayColor)



def drawEllipse(coords, color, double a, double b, double rotate):
    cdef double x = coords[0]
    cdef double y = coords[1]

    cdef int colorLength = len(color)
    cdef unsigned char *arrayColor = <unsigned char *>malloc(4 * sizeof(unsigned char))
    arrayColor[0] = color[0]
    arrayColor[1] = color[1]
    arrayColor[2] = color[2]
    if( colorLength == 3):
        arrayColor[3] = 255
    elif( colorLength == 4):
        arrayColor[3] = color[3]
    #else:
    #    kasta exception

    wh.cDrawEllipse(x, y, a, b, rotate, arrayColor)
    free(arrayColor)



def drawPolygon(coords, color, int type = 0):
    cdef numpy.ndarray newCoords = numpy.array(coords, dtype = float)

    cdef double *cCoords = <double *>newCoords.data
    cdef int dim = newCoords.shape[0]

    cdef int colorLength = len(color)
    cdef unsigned char *arrayColor = <unsigned char *>malloc(4 * sizeof(unsigned char))
    arrayColor[0] = color[0]
    arrayColor[1] = color[1]
    arrayColor[2] = color[2]
    if( colorLength == 3):
        arrayColor[3] = 255
    elif( colorLength == 4):
        arrayColor[3] = color[3]
    #else:
    #    kasta exception

    wh.cDrawPolygon(cCoords, dim, arrayColor)
    free(arrayColor)


def setClearColor(color):
    cdef int colorLength = len(color)
    cdef unsigned char *arrayColor = <unsigned char *>malloc(4 * sizeof(unsigned char))
    arrayColor[0] = color[0]
    arrayColor[1] = color[1]
    arrayColor[2] = color[2]
    if( colorLength == 3):
        arrayColor[3] = 255
    elif( colorLength == 4):
        arrayColor[3] = color[3]
    
    wh.cSetClearColor(arrayColor)
    free(arrayColor)


def getMouseState():
    global wh
    cdef int x
    cdef int y
    wh.cGetMousePos(&x, &y)
    left = wh.cGetMouseButton(MOUSE_BUTTON_LEFT)
    right = wh.cGetMouseButton(MOUSE_BUTTON_RIGHT)
    middle = wh.cGetMouseButton(MOUSE_BUTTON_MIDDLE)
    wheel = wh.cGetMouseWheel()
    return {"x" : x, "y" : y, "left" : left, "right" : right, "middle" : middle, "wheel" : wheel}

def getKey(key):
    if isinstance(key, str):
        key = ord(key)
    return wh.cGetKey(key)
















cdef extern from "Image.h":
    ctypedef struct c_Image "Image":
        void draw(double x, double y, double x_0, double y_0, double width, double height, bint bilinear, double scaleX, double scaleY, double rotate, double rotatePtX, double rotatePtY, unsigned char *color)
        int getWidth()
        int getHeight()
        void clear(color)
        void drawPoints(double *coords, int dimension, unsigned char *color, double size, bint antiAliasing)
        void drawLines(double *coords, int dimension, unsigned char *color, double width, bint loop, \
                                bint stipple, int stippleFactor, int stipplePattern, bint antiAliasing)
        void drawTriangles(double *coords, int dimension, unsigned char *color, int form)
        void drawPolygon(double *coords, int dimension, unsigned char *color)
        void drawSubImg(c_Image *subImg, double x, double y, double x_0, double y_0, double width, double height, \
                bint bilinear, double scaleX, double scaleY, double rotate, double rotatePtX, double rotatePtY, unsigned char *color)
    c_Image *new_ImageI "new Image" (int width, int height, unsigned char *arrayColor)
    c_Image *new_ImageII "new Image" (char *file)
    c_Image *new_ImageIII "new Image" (char* text, int size)
    void del_Image "delete" (c_Image *img)




cdef class Image:	# test if an image can be generated from wh instead 
    cdef c_Image *thisPtr
    def __cinit__(self, I, II = None, III = None, IV = None, V = None):
        cdef int width
        cdef int height
        cdef int colorLength
        cdef unsigned char *arrayColor

        if isinstance(I, str):
            if isinstance(II, int):        #(char *text, int size)
                self.thisPtr = new_ImageIII( I, II )
                return
            else:        #(char *file)
                self.thisPtr = new_ImageII( I )
                return
        elif isinstance(I, int):        #(int width, int height, unsigned char *arrayColor)
            width = I
            height = II
            color = III

            colorLength = len( color )
            arrayColor = <unsigned char *>malloc(4 * sizeof(unsigned char))
            arrayColor[0] = color[0]
            arrayColor[1] = color[1]
            arrayColor[2] = color[2]
            if( colorLength == 3):
                arrayColor[3] = 255
            elif( colorLength == 4):
                arrayColor[3] = color[3]
            #else:
            #    kasta exception
            self.thisPtr = new_ImageI( width, height, arrayColor )
            return
        #else:
        #    kasta exception

    def __dealloc__(self):
        del_Image(self.thisPtr)

    def draw(self, double x, double y, double x_0=0, double y_0=0, double width=-1, double height=-1, bint bilinear = False, double scaleX = 1.0, double scaleY = 1.0, double rotate = 0.0, double rotatePtX = 0.0, double rotatePtY = 0.0, color = False):
        cdef int colorLength
        cdef unsigned char *arrayColor
        if(color):
            colorLength = len( color )
            arrayColor = <unsigned char *>malloc(4 * sizeof(unsigned char))
            arrayColor[0] = color[0]
            arrayColor[1] = color[1]
            arrayColor[2] = color[2]
            if( colorLength == 3):
                arrayColor[3] = 255
            elif( colorLength == 4):
                arrayColor[3] = color[3]
            #else:
            #    kasta exception
        else:
            arrayColor = NULL
        self.thisPtr.draw(x, y, x_0, y_0, width, height, bilinear, scaleX, scaleY, rotate, rotatePtX, rotatePtY, arrayColor)

    def getWidth(self):
        return self.thisPtr.getWidth()

    def getHeight(self):
        return self.thisPtr.getHeight()

    def drawPoints(self, coords, color, float size = 1.0, bint antiAliasing = False):
        cdef numpy.ndarray newCoords = numpy.array(coords, dtype = float)

        cdef double *cCoords = <double *>newCoords.data
        cdef int dim = newCoords.shape[0]

        cdef int colorLength = len(color)
        cdef unsigned char *arrayColor = <unsigned char *>malloc(4 * sizeof(unsigned char))
        arrayColor[0] = color[0]
        arrayColor[1] = color[1]
        arrayColor[2] = color[2]
        if( colorLength == 3):
            arrayColor[3] = 255
        elif( colorLength == 4):
            arrayColor[3] = color[3]
        #else:
        #    kasta exception
        self.thisPtr.drawPoints(cCoords, dim, arrayColor, size, antiAliasing)
        free(arrayColor)

    def drawLines(self, coords, color, float width = 1, bint loop = False, stipple = False, bint antiAliasing = False):
        cdef numpy.ndarray newCoords = numpy.array(coords, dtype = float)

        cdef double *cCoords = <double *>newCoords.data
        cdef int dim = newCoords.shape[0]

        cdef int colorLength = len(color)
        cdef unsigned char *arrayColor = <unsigned char *>malloc(4 * sizeof(unsigned char))
        arrayColor[0] = color[0]
        arrayColor[1] = color[1]
        arrayColor[2] = color[2]
        if( colorLength == 3):
            arrayColor[3] = 255
        elif( colorLength == 4):
            arrayColor[3] = color[3]
        #else:
        #    kasta exception

        if( stipple and len(stipple)==2 ):
            stippleFactor = stipple[0]
            stipplePattern = stipple[1]
        else:    # Aer so illa tvungen att ge dessa ett vaerde...
            stippleFactor = 1
        stipplePattern = 0xFFFF

        self.thisPtr.drawLines(cCoords, dim, arrayColor, width, loop, stipple, stippleFactor, stipplePattern, antiAliasing)
        free(arrayColor)

    def drawTriangles(self, coords, color, int form = SEPARATE):
        cdef numpy.ndarray newCoords = numpy.array(coords, dtype = float)

        cdef double *cCoords = <double *>newCoords.data
        cdef int dim = newCoords.shape[0]

        cdef int colorLength = len(color)
        cdef unsigned char *arrayColor = <unsigned char *>malloc(4 * sizeof(unsigned char))
        arrayColor[0] = color[0]
        arrayColor[1] = color[1]
        arrayColor[2] = color[2]
        if( colorLength == 3):
            arrayColor[3] = 255
        elif( colorLength == 4):
            arrayColor[3] = color[3]
        #else:
        #    kasta exception

        self.thisPtr.drawTriangles(cCoords, dim, arrayColor, form)
        free(arrayColor)

    def drawPolygon(self, coords, color):
        cdef numpy.ndarray newCoords = numpy.array(coords, dtype = float)

        cdef double *cCoords = <double *>newCoords.data
        cdef int dim = newCoords.shape[0]

        cdef int colorLength = len(color)
        cdef unsigned char *arrayColor = <unsigned char *>malloc(4 * sizeof(unsigned char))
        arrayColor[0] = color[0]
        arrayColor[1] = color[1]
        arrayColor[2] = color[2]
        if( colorLength == 3):
            arrayColor[3] = 255
        elif( colorLength == 4):
            arrayColor[3] = color[3]
        #else:
        #    kasta exception

        self.thisPtr.drawPolygon(cCoords, dim, arrayColor)
        free(arrayColor)

    def subImage(self, Image src, int x, int y, double x_0 = 0.0, double y_0 = 0.0, double width = -1.0, double height = -1.0, bint bilinear = False, double scaleX = 1.0, double scaleY = 1.0, double rotate = 0.0, double rotatePtX = 0.0, double rotatePtY = 0.0, color = False):
        cdef int colorLength
        cdef unsigned char *arrayColor
        if(color):
            colorLength = len( color )
            arrayColor = <unsigned char *>malloc(4 * sizeof(unsigned char))
            arrayColor[0] = color[0]
            arrayColor[1] = color[1]
            arrayColor[2] = color[2]
            if( colorLength == 3):
                arrayColor[3] = 255
            elif( colorLength == 4):
                arrayColor[3] = color[3]
            #else:
            #    kasta exception
        else:
            arrayColor = NULL
        self.thisPtr.drawSubImg(src.thisPtr, x, y, x_0, y_0, width, height, bilinear, scaleX, scaleY, rotate, rotatePtX, rotatePtY, arrayColor)




cdef extern from "Timer.h":
    ctypedef struct c_Timer "Timer":
        void resume()
        void pause()
        void togglePause()
        void stop()
        void setTime(double time)
        double getTime()
        void addTime(double time)
        bint isRunning()
    c_Timer *new_Timer "new Timer" (double time, bint start)
    void del_Timer "delete" (c_Timer *timer)


cdef class Timer:
    cdef c_Timer *thisPtr
    def __cinit__(self, double time = 0, bint start = True):
        self.thisPtr = new_Timer(time, start)

    def __dealloc__(self):
        del_Timer(self.thisPtr)

    def resume(self):
        self.thisPtr.resume()

    def pause(self):
        self.thisPtr.pause()

    def togglePause(self):
        self.thisPtr.togglePause()

    def stop(self):
        self.thisPtr.stop()

    def setTime(self, double time):
        self.thisPtr.setTime(time)

    def getTime(self):
        return self.thisPtr.getTime()

    def addTime(self, double time):
        self.thisPtr.addTime(time)

    def isRunning(self):
        return self.thisPtr.isRunning()











#def drawPolygon(coords, color, int type = 0):

#    cdef numpy.ndarray newCoords = numpy.array(coords, dtype = float)

#    cdef double *cCoords = <double *>newCoords.data
#    cdef int dim = newCoords.dimensions[0]

#    cdef int colorLength = len(color)
#    cdef int R = color[0]
#    cdef int G = color[1]
#    cdef int B = color[2]
#    cdef int A
#    if( colorLength == 3):
#        A = 255
#    elif( colorLength == 4):
#        A = color[3]
#    #else:
#    #    kasta exception

#    cArbitraryPolygon(cCoords, 1, dim, R, G, B, A, type)

#def applyConvolution(image)
#    shape = image.shape
#    cdef height = shape[0]
#    cdef width = shape[1]
#    cdef numpy.ndarray arrayImage = numpy.array(image, dtype = float)

#    cdef double *cImage = <double *>arrayImage.data

#    cApplyConvolution(cImage, width, height)






cdef extern from "Sound.h":
    ctypedef struct c_Sound "Sound":
        void play()
        void pause()
        void stop()
        void setPosition(int pos)
        void setFrequency(float hz)
        void setPan(float pan)
        void setVolume(float volume)
        int getPosition()
        float getFrequency()
        bint isPlaying()
    c_Sound *new_Sound "new Sound" (char *fileName, bint stream)
    void del_Sound "delete" (c_Sound *snd)


cdef class Sound:
    cdef c_Sound *thisPtr
    def __cinit__(self, filename, bint stream):
        self.thisPtr = new_Sound(filename, stream)

    def __dealloc__(self):
        del_Sound(self.thisPtr)

    def play(self):
        self.thisPtr.play()

    def pause(self):
        self.thisPtr.pause()

    def stop(self):
        self.thisPtr.stop()

    def setPosition(self, int pos):
        self.thisPtr.setPosition(pos)

    def setFrequency(self, float hz):
        self.thisPtr.setFrequency(hz)

    def setPan(self, float pan):
        self.thisPtr.setPan(pan)

    def setVolume(self, float volume):
        self.thisPtr.setVolume(volume)

    def getPosition(self):
        return self.thisPtr.getPosition()

    def getFrequency(self):
        return self.thisPtr.getFrequency()

    def isPlaying(self):
        return self.thisPtr.isPlaying()













#cdef extern from "Sound.h":
#    ctypedef struct c_Sound "Sound":
#        void play()
#        void stop()
#        bint isPlaying ()
#        void reset()
#        void setRepeat(bint repeat)
#        float getRepeat()
#        void setVolume(float volume)
#        float getVolume()
#        void setPan(float pan)
#        float getPan()
#        void setSpeed(float speed)
#        float getSpeed()
#        int getLength()
#        void setPosition(int pos)
#        int getPosition()
#        
#        bint isStreamed()
#    c_Sound *new_Sound "new Sound" (char *fileName, bint stream)
#    void del_Sound "delete" (c_Sound *snd)



#cdef class Sound:
#    cdef c_Sound *thisPtr
#    def __cinit__(self, filename, bint stream):
#        self.thisPtr = new_Sound(filename, stream)
#
#    def __dealloc__(self):
#        del_Sound(self.thisPtr)
#
#    def play(self):
#        self.thisPtr.play()
#
#    def stop(self):
#        self.thisPtr.stop()
#
#    def isPlaying(self):
#        return self.thisPtr.isPlaying()
#
#    def reset(self):
#        self.thisPtr.reset()
#
#    def setRepeat(self, bint repeat):
#        self.thisPtr.setRepeat(repeat)
#
#    def getRepeat(self):
#        return self.thisPtr.getRepeat()
#
#    def setVolume(self, float volume):
#        self.thisPtr.setVolume(volume)
#
#    def setPan(self, float pan):
#        self.thisPtr.setPan(pan)
#
#    def getPan(self):
#        return self.thisPtr.getPan()
#
#    def setSpeed(self, float speed):
#        self.thisPtr.setSpeed(speed)
#
#    def getSpeed(self):
#        return self.thisPtr.getSpeed()
#
#    def getLength(self):
#        return self.thisPtr.getLength()
#
#    def setPosition(self, int pos):
#        self.thisPtr.setPosition(pos)
#
#    def getPosition(self):
#        return self.thisPtr.getPosition()
#
#    def isStreamed(self):
#        return self.thisPtr.isStreamed()







