#ifndef _UTILS_H
#define _UTILS_H

class Utilities
{
public:

    // //Get data size DATA TYPE
    static int getColDataTypeSize(short type);
    
  
    static bool checkFile(const char* filename);

    static void deleteDBMSFile(const char* filename);
    

    static char* getStrData(const char* s, int type);
};

#endif
