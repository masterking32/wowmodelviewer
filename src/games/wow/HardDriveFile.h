/*
 * HardDriveFile.h
 *
 *  Created on: 21 dec. 2015
 *      Author: Jerome
 */

#ifndef _HARDDRIVEFILE_H_
#define _HARDDRIVEFILE_H_

#include "CASCFile.h"

class QFile;

class  HardDriveFile : public CASCFile
{
  public:
    HardDriveFile(QString path, QString realpath, int id = -1);
    ~HardDriveFile();

  protected:
    virtual bool openFile();
    virtual bool isAlreadyOpened();
    virtual bool getFileSize(unsigned long long & s);
    virtual unsigned long readFile();
    virtual bool doPostCloseOperation();


  private:
    bool opened;
    QString realpath;
    QFile * file;
};




#endif /* _HARDDRIVEFILE_H_ */
