#include "CQrcFileSystem.h"
#include <QString>
#include <QFile>
#include <QByteArray>

namespace irr
{
namespace io
{


CQrcReadFile::CQrcReadFile(const io::path &fileName, CQrcFileSystem *fileSystem)
	: CReadFile(fileName)
	, ReadFile(NULL)
	, FileSystem(fileSystem)
{
	// check, if irrlicht allready loda this file
	if (isOpen())
		return;

	QString qpath = QString::fromUtf8(fileName.c_str());

	QFile file( qpath );
	bool ok = false;
	if ( qpath.indexOf(":/") == -1 )
	{
		ok = file.open(QIODevice::ReadOnly); //try load from file system
		if (!ok)
		{
			qpath = QString(":/") + qpath;
		}
	}

	if (!ok)
	{
		file.setFileName(qpath);
		ok = file.open(QIODevice::ReadOnly);
	}
	if ( !ok )
	{
		// noop
		return;
	}
	ByteArray = file.readAll();
	ReadFile = FileSystem->createMemoryReadFile( (void*)ByteArray.data(), ByteArray.size(), fileName.c_str(), false);
}

CQrcReadFile::~CQrcReadFile()
{
	if (ReadFile)
		ReadFile->drop();
}

size_t CQrcReadFile::read(void *buffer, size_t sizeToRead)
{
	if (!ReadFile)
		return 0;
	return ReadFile->read(buffer, sizeToRead);
}

bool CQrcReadFile::seek(long finalPos, bool relativeMovement)
{
	if (!ReadFile)
		return false;
	return  ReadFile->seek(finalPos, relativeMovement);
}

long CQrcReadFile::getSize() const
{
	if (!ReadFile)
		return 0;
	return ReadFile->getSize();
}

long CQrcReadFile::getPos() const
{
	if (!ReadFile)
		return 0;
	return ReadFile->getPos();
}

const io::path &CQrcReadFile::getFileName() const
{
	if (!ReadFile)
		return io::path();
	return ReadFile->getFileName();
}


CQrcFileSystem::CQrcFileSystem()
{

}

irr::io::IReadFile *irr::io::CQrcFileSystem::createAndOpenFile(const irr::io::path &filename)
{
	CQrcReadFile *f = new CQrcReadFile(filename, this);
	if ( f->getSize() > 0 )
		return f;
	f->drop();
	return CFileSystem::createAndOpenFile(filename);
}


}
}
