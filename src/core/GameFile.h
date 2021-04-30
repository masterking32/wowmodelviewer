/*
 * GameFile.h
 *
 *  Created on: 27 oct. 2014
 *      Author: Jerome
 */

#ifndef _GAMEFILE_H_
#define _GAMEFILE_H_

#include <string>
#include <vector>

#include "metaclasses/Component.h"

class GameFile : public Component
{
  public:
    GameFile(QString path, int id = -1) 
      : eof(true), buffer(nullptr), pointer(0), size(0), 
        filepath(path), m_useMemoryBuffer(true), m_fileDataId(id),
        originalBuffer(nullptr), curChunk("")
    {}

    virtual ~GameFile() {}

    virtual size_t read(void* dest, size_t bytes);
    size_t getSize();
    size_t getPos();
    unsigned char* getBuffer() const;
    unsigned char* getPointer();
    bool isEof();
    virtual void seek(size_t offset);
    void seekRelative(size_t offset);
    bool open(bool useMemoryBuffer = true);
    bool close();
    
    void setFullName(const QString & name) { filepath = name; }
    QString fullname() const { return filepath; }
    int fileDataId() { return m_fileDataId; }

    void allocate(unsigned long long size);
    bool setChunk(std::string chunkName, bool resetToStart = true);
    bool isChunked() { return chunks.size() > 0; }

    virtual void dumpStructure();

  protected:

    virtual bool openFile() = 0;
    virtual bool isAlreadyOpened() = 0;
    virtual bool getFileSize(unsigned long long & s) = 0;
    virtual unsigned long readFile() = 0;
    virtual void doPostOpenOperation() = 0;
    virtual bool doPostCloseOperation() = 0;

    bool eof;
    unsigned char *buffer;
    unsigned long long pointer, size;
    QString filepath;
    int m_fileDataId;
    
    struct chunkHeader
    {
      char magic[4];
      unsigned __int32 size;
    };

    struct Chunk
    {
      std::string magic;
      unsigned int start;
      unsigned int size;
      unsigned int pointer;
    };

    std::vector<Chunk> chunks;
    bool m_useMemoryBuffer;

  private:
    // disable copying
    GameFile(const GameFile &);
    void operator=(const GameFile &);
    unsigned char * originalBuffer;
    std::string curChunk;
};



#endif /* _GAMEFILE_H_ */
