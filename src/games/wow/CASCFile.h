/*
 * CASCFile.h
 *
 *  Created on: 23 oct. 2014
 *      Author: Jerome
 */

#ifndef _CASCFILE_H_
#define _CASCFILE_H_

#include "GameFile.h"

typedef void* HANDLE;

class CASCFolder;

class CASCFile : public GameFile
{
  public:
    CASCFile(QString path, int id = -1);
    ~CASCFile();

    // re implemented from GameFile
    size_t read(void* dest, size_t bytes);
    void seek(size_t offset);
    void dumpStructure();

  protected:
    virtual bool openFile();
    virtual bool isAlreadyOpened();
    virtual bool getFileSize(unsigned long long & s);
    virtual unsigned long readFile();
    virtual void doPostOpenOperation();
    virtual bool doPostCloseOperation();

  private:
    HANDLE m_handle;
};




#endif /* _CASCFILE_H_ */
