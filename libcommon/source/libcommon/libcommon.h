#ifndef LIBCOMMON_H_INCLUDED
#define LIBCOMMON_H_INCLUDED

//Third-Party libraries
#include "libcommon/base64.h"
#include "libcommon/md5.h"
#include "libcommon/json.h"
#include "libcommon/sqlite3.h"
#include "libcommon/zlib.h"
#include "libcommon/tinyxml.h"

#include "libcommon/common.h"
#include "libcommon/Buffer.h"
#include "libcommon/ZipCompress.h"
#include "libcommon/log.h"
#include "libcommon/MemoryPool.h"
#include "libcommon/Singleton.h"
#include "libcommon/ThreadPool.h"
#include "libcommon/CriticalSectionLock.h"
#include "libcommon/ServerThread.h"
#include "libcommon/ClientThread.h"
#include "libcommon/Event.h"
#include "libcommon/ProcHelper.h"
#include "libcommon/Charset.h"
#include "libcommon/NamedPipeServer.h"
#include "libcommon/NamedPipeClient.h"
#include "libcommon/RegexHelper.h"
#include "libcommon/StringHelper.h"
#include "libcommon/ConvertHelper.h"
#include "libcommon/FileHelper.h"
#include "libcommon/OSHelper.h"
#include "libcommon/PathHelper.h"
#include "libcommon/RegHelper.h"

#endif // LIBCOMMON_H_INCLUDED

//NOTE: libcommon.lib		使用/MD(运行库：dll)动态链接，可能需要忽略特定默认库libcmt.lib，以免冲突
//		libcommon_mt.lib	使用/MT静态链接
