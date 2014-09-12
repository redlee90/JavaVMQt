#include <QString>
#include <stdbool.h>

class FilePathManager
{
public:
	FilePathManager(void);
public:
	~FilePathManager(void);
public:
    BOOL GetAbsolutePath(QString relPath, QString& absPath);
};
