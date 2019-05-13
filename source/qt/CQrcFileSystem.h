#ifndef CQRCFILESYSTEM_H
#define CQRCFILESYSTEM_H

#include <QtCore/qglobal.h>
#include <QByteArray>
#include "../Irrlicht/CFileSystem.h"
#include "../Irrlicht/CReadFile.h"

namespace irr
{
namespace io
{
class CQrcFileSystem;

class CQrcReadFile : public CReadFile
{
public:
	CQrcReadFile(const io::path& fileName, CQrcFileSystem *fileSystem);
	~CQrcReadFile();
	//! Reads an amount of bytes from the file.
	/** \param buffer Pointer to buffer where read bytes are written to.
	\param sizeToRead Amount of bytes to read from the file.
	\return How many bytes were read. */
	virtual size_t read(void* buffer, size_t sizeToRead);

	//! Changes position in file
	/** \param finalPos Destination position in the file.
	\param relativeMovement If set to true, the position in the file is
	changed relative to current position. Otherwise the position is changed
	from beginning of file.
	\return True if successful, otherwise false. */
	virtual bool seek(long finalPos, bool relativeMovement = false);

	//! Get size of file.
	/** \return Size of the file in bytes. */
	virtual long getSize() const;

	//! Get the current position in the file.
	/** \return Current position in the file in bytes. */
	virtual long getPos() const;

	//! Get name of file.
	/** \return File name as zero terminated character string. */
	virtual const io::path &getFileName() const;
private:
	QByteArray ByteArray;
	IReadFile *ReadFile;

	CQrcFileSystem *FileSystem;
};

class CQrcFileSystem : public CFileSystem
{
public:
	CQrcFileSystem();

	//! opens a file for read access
	virtual IReadFile* createAndOpenFile(const io::path& filename) _IRR_OVERRIDE_;
};

}
}

#endif // CQRCFILESYSTEM_H
