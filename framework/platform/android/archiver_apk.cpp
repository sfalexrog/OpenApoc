// PhysicsFS module for reading files from assets.

// Obviously enough, it should only be accessible for Android.
#ifdef __ANDROID__

#include "framework/logger.h"
#include "library/strings.h"

// We're exploiting the build system a bit, this generally should not be possible otherwise.
#define __PHYSICSFS_INTERNAL__
#include <physfs_internal.h>

// for SDL_AndroidGetVM
#include <SDL.h>

#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
// for read mutexes
#include <mutex>

namespace
{

using namespace OpenApoc;

const char APK_MAGIC[] = "apkfs";

AAssetManager *manager = nullptr;
jobject jAssetManager; // This is a global reference!

// Helper class for PHYSFS_Io
class AssetIO {
private:
    AAsset *_asset;
    UString _filename; // for duplicate?
    std::mutex read_mutex;
    AssetIO(UString assetName) : _filename(assetName)
    {
        // FIXME: Set the mode to something more appropriate?
        _asset = AAssetManager_open(manager,
                                    _filename.c_str(),
                                    AASSET_MODE_STREAMING);
    }

    PHYSFS_sint64 read(void *buf, PHYSFS_uint64 len) {
        std::lock_guard<std::mutex> read_lock(read_mutex);
        // FIXME: len might not fit into size_t... split reads?
        return AAsset_read(_asset, buf, len);
    }

    int seek(PHYSFS_uint64 offset)
    {
        std::lock_guard<std::mutex> read_lock(read_mutex);
        // PHYSFS_seek is semantically different from an ordinary seek...
        off64_t newPos = AAsset_seek(_asset, offset, SEEK_SET);
        // Not sure if this comparison is enough
        int retval = (offset == newPos) ? 1 : 0;
        return retval;
    }

    PHYSFS_sint64 tell()
    {
        std::lock_guard<std::mutex> read_lock(read_mutex);
        // seriously now, no AAsset_tell?
        off64_t totalLength = AAsset_getLength64(_asset);
        off64_t remainingLength = AAsset_getRemainingLength(_asset);
        return totalLength - remainingLength;
    }

    PHYSFS_sint64 length()
    {
        // This should not require a lock (hopefully...)
        return AAsset_getLength64(_asset);
    }

    PHYSFS_Io *duplicate()
    {
        return getIo(_filename);
    }

    ~AssetIO()
    {
        if (_asset)
            AAsset_close(_asset);
    }

public:
    static PHYSFS_Io *getIo(UString assetName)
    {
        AssetIO *io = new AssetIO(assetName);
        if (!(io->_asset))
        {
            PHYSFS_setErrorCode(PHYSFS_ERR_NOT_FOUND);
            LogWarning("Could not open \"%s\" in Assets", assetName.c_str());
            delete io;
            return nullptr;

        }
        PHYSFS_Io *pio = new PHYSFS_Io();
        pio->version = CURRENT_PHYSFS_IO_API_VERSION;
        pio->opaque = io;
        pio->read = apk_read;
        pio->write = apk_write;
        pio->seek = apk_seek;
        pio->tell = apk_tell;
        pio->length = apk_length;
        pio->duplicate = apk_duplicate;
        pio->flush = apk_flush;
        pio->destroy = apk_destroy;
        return pio;
    }

    static PHYSFS_sint64 apk_read(PHYSFS_Io *io, void *buf, PHYSFS_uint64 len)
    {
        AssetIO *aio = (AssetIO*) io->opaque;
        return aio->read(buf, len);
    }

    static PHYSFS_sint64 apk_write(PHYSFS_Io *io, const void *buf, PHYSFS_uint64 len)
    {
        std::ignore = io;
        BAIL_MACRO(PHYSFS_ERR_READ_ONLY, -1); // Assets are read-only
    };

    static int apk_seek(PHYSFS_Io *io, PHYSFS_uint64 offset)
    {
        AssetIO *aio = (AssetIO*) io->opaque;
        return aio->seek(offset);
    }

    static PHYSFS_sint64 apk_tell(PHYSFS_Io *io)
    {
        AssetIO *aio = (AssetIO*) io->opaque;
        return aio->tell();
    }

    static PHYSFS_sint64 apk_length(PHYSFS_Io *io)
    {
        AssetIO *aio = (AssetIO*) io->opaque;
        return aio->length();
    }

    static PHYSFS_Io *apk_duplicate(PHYSFS_Io *io)
    {
        AssetIO *aio = (AssetIO*) io->opaque;
        return aio->duplicate();
    }

    static int apk_flush(PHYSFS_Io *io)
    {
        std::ignore = io;
        return 1; // We don't perform writes
    }

    static void apk_destroy(PHYSFS_Io *io)
    {
        AssetIO *aio = (AssetIO*) io->opaque;
        delete aio;
        delete io;
    }
};

// FIXME: Should this really be a class? If yes, then maybe make it nicer?
class AssetArchiver
{

private:

    AssetArchiver()
    {
        if (!manager) { initManager(); }
    }

public:

    static AssetArchiver& getInstance()
    {
        static AssetArchiver instance;
        return instance;
    }

    static PHYSFS_Archiver* createArchiver()
    {
        PHYSFS_Archiver *archiverApk = new PHYSFS_Archiver();
        archiverApk->info = {
                /*extension:        */ APK_MAGIC,
                /*description:      */ "A physfs-to-AAssetManager compatibility layer",
                /*author:           */ "Alexey Rogachevsky <sfalexrog@gmail.com>",
                /*url:              */ "https://developer.android.com/ndk/reference/group___asset.html",
                /*supportsSymlinks: */ 0};
        archiverApk->version = CURRENT_PHYSFS_ARCHIVER_API_VERSION;
        archiverApk->openArchive = apkOpenArchive;
        archiverApk->enumerateFiles = apkEnumerateFiles;
        archiverApk->openRead = apkOpenRead;
        archiverApk->openWrite = apkOpenWrite;
        archiverApk->openAppend = apkOpenAppend;
        archiverApk->remove = apkRemove;
        archiverApk->mkdir = apkMkdir;
        archiverApk->stat = apkStat;
        archiverApk->closeArchive = apkCloseArchive;
        return archiverApk;
    };

    static void *apkOpenArchive(PHYSFS_Io *io, const char* filename, int forWrite)
    {
        std::ignore = io; // We don't care much for that 'io' struct
        UString filenameStr(filename); // Convert to UString for easier compares
        // Ignore incorrect magics
        BAIL_IF_MACRO((filenameStr != "assets.apkfs"), PHYSFS_ERR_UNSUPPORTED, NULL);
        // Android assets are read-only
        BAIL_IF_MACRO(forWrite, PHYSFS_ERR_READ_ONLY, NULL);
        return &getInstance();
    }

    static void apkEnumerateFiles(void *opaque, const char *dname,
                                  PHYSFS_EnumFilesCallback cb,
                                  const char *origdir, void *callbackdata)
    {
/*
        AssetArchiver *archiver = (AssetArchiver*) opaque;
        AAssetDir *dir = AAssetManager_openDir(manager, dname);
        if (!dir) { return; }
        const char* dirName;
        while(dirName = AAssetDir_getNextFileName(dir))
        {
            cb(callbackdata, dname, dirName);
        }
        AAssetDir_close(dir);
*/
        JNIEnv *env = (JNIEnv*)SDL_AndroidGetJNIEnv(); // This could have been a simple function,
        jstring jpath = env->NewStringUTF(dname); // had the Android developers in their limitless
        jclass amClass = env->GetObjectClass(jAssetManager); // wistom not decided that asset manager
        jmethodID listMethod = env->GetMethodID(amClass, // should behave differently depending on
                                                "list",  // the API you're using.
                                                "(Ljava/lang/String;)[Ljava/lang/String;");
        jobjectArray strArray = (jobjectArray)env->CallObjectMethod(jAssetManager, listMethod, jpath);
        if (!strArray)  // See, in the old Unix days everything used to be a file - even a folder.
        { // So the Java AssetManager API respects this convention and lists all files and directories
            LogWarning("AssetManager.list returned null array"); // in the path, all alike.
            return; // But the C++ API is different. Someone thought listing files and directories
        } // would be confusing, and in a flash of a genius decided to make sure AAssetDir_getNextFilename
        int len = env->GetArrayLength(strArray); // would only return names of *files*, not
        for (int i = 0; i < len; i++) // *folders*, making such simple tasks as "enumerate
        { // all subfolders in a given folder" impossible to do with the current API.
            const char* dirName; // What's even worse, there's some code and a comment in AssetManager.cpp
            jstring jDirName = (jstring)env->GetObjectArrayElement(strArray, i); // reading:
            dirName = env->GetStringUTFChars(jDirName, 0); // "Find the next regular file; explicitly"
            cb(callbackdata, origdir, dirName); // "don't report directories even if the underlying"
            env->ReleaseStringUTFChars(jDirName, dirName); // "implementation changes to report them".
            env->DeleteLocalRef(jDirName); // So this discrepancy is very much intentional and not a bug.
        } // What is in the heads of those who design these thigs is beyond me. How could it happen
        env->DeleteLocalRef(strArray); // that the same object has two incompatible APIs? And instead
        env->DeleteLocalRef(amClass); // of using the C/C++ API like I should, I'm forced to write
        env->DeleteLocalRef(jpath); // this abomination of JNI code to call the Java method.
        // TODO: Probably remove all that in favor of opening an .apk as a .zip?
    }

    static PHYSFS_Io *apkOpenRead(void *opaque, const char *filename)
    {
        return AssetIO::getIo(filename);
    }

    static PHYSFS_Io *apkOpenWrite(void *opaque, const char *filename)
    {
        BAIL_MACRO(PHYSFS_ERR_READ_ONLY, NULL);
    }

    static PHYSFS_Io *apkOpenAppend(void *opaque, const char *filename)
    {
        BAIL_MACRO(PHYSFS_ERR_READ_ONLY, NULL);
    }

    static int apkRemove(void *opaque, const char *filename)
    {
        BAIL_MACRO(PHYSFS_ERR_READ_ONLY, 0);
    }

    static int apkMkdir(void *opaque, const char *filename)
    {
        BAIL_MACRO(PHYSFS_ERR_READ_ONLY, 0);
    }

    static int apkStat(void *opaque, const char *filename, PHYSFS_Stat *stat)
    {
        AAsset *asset = AAssetManager_open(manager, filename, AASSET_MODE_UNKNOWN);
        if (asset)
        {
            stat->filesize = (PHYSFS_sint64)AAsset_getLength64(asset);
            AAsset_close(asset);
            stat->filetype = PHYSFS_FILETYPE_REGULAR;
            stat->modtime = 0;
            stat->createtime = 0;
            stat->accesstime = 0;
            stat->readonly = 1;

            return 1;
        }
        AAssetDir *tryDir = AAssetManager_openDir(manager, filename);
        bool isDir = AAssetDir_getNextFileName(tryDir) != 0;
        AAssetDir_close(tryDir);
        if (isDir)
        {
            stat->filesize = 0;
            stat->filetype = PHYSFS_FILETYPE_DIRECTORY;
            stat->modtime = 0;
            stat->createtime = 0;
            stat->accesstime = 0;
            stat->readonly = 1;
            return 1;
        }
        return 0;
    }

    static void apkCloseArchive(void *opaque)
    {
        // FIXME: Does not work as expected!
        /*JNIEnv *env = (JNIEnv*) SDL_AndroidGetJNIEnv();
        env->DeleteGlobalRef(jAssetManager);*/
    }


    static void initManager()
    {
        LogInfo("Attempting to grab the AssetManager");
        // Grab the pointer to AAssetManager
        JNIEnv *env = (JNIEnv*)SDL_AndroidGetJNIEnv();
        if (!env)
        {
            LogError("Could not obtain JNI environment pointer!");
            return;
        }
        // OpenApocActivity should hold the reference to both itself (by it being a subclass of
        // SDLActivity) and AssetManager. This should also ensure that AssetManager won't be GC'd.
        jclass activityClass = env->FindClass("org/sfalexrog/openapoc/OpenApocActivity");
        if (!activityClass)
        {
            LogError("Could not obtain class handle for base activity!");
            return;
        }
        LogInfo("Searching for activity instance field...");
        jfieldID activityField = env->GetStaticFieldID(activityClass, "mOpenApocActivity", "Lorg/sfalexrog/openapoc/OpenApocActivity;");
        if (!activityField)
        {
            LogError("Could not find activity field!");
            return;
        }
        LogInfo("Grabbing the instance reference...");
        jobject activityInstance = env->GetStaticObjectField(activityClass, activityField);
        if (!activityInstance)
        {
            LogError("Could not locate activity instance!");
            return;
        }
        LogInfo("Searching for assetManager field in activity...");
        jfieldID amField = env->GetFieldID(activityClass, "assetManager", "Landroid/content/res/AssetManager;");
        if (!amField)
        {
            LogError("Could not find assetManager field!");
            return;
        }
        jobject jassetManager = env->GetObjectField(activityInstance, amField);
        if (!jassetManager)
        {
            LogError("Could not find a reference to the asset manager instance!");
        }
        LogInfo("Passing the AssetManager reference to the AAssetManager");
        jAssetManager = env->NewGlobalRef(jassetManager);
        // Nice things: A native AAssetManager pointer may be shared across multiple threads.
        manager = AAssetManager_fromJava(env, jassetManager);
    }
};

} // anonymous namespace

namespace OpenApoc
{
void registerAPKArchiver()
{
    if (!manager) { AssetArchiver::initManager(); }
    PHYSFS_registerArchiver(AssetArchiver::createArchiver());
}
}

#else
#warning "You should not be compiling this file - it's for Android only"
#endif
