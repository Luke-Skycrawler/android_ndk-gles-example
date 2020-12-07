# android_ndk-gles-example
android platform native app with gles implementation, using  glfm and jni build 
### INSTALL
除了下载sdk的包之外几乎没打开过android studio。

多半成活不了。选择的是软件栈是ndk, glfm (frame mobile), assimp（网格导入）。
C++环境配好了。参考资料是OpenGLES 3.0 Programming Guide.（https://github.com/danginsburg/opengles3-book）
### 0.tree:
create those folders within the same folder of .git
├─Android
│  ├─assets
│  │  ├─mods
│  │  │  └─sportsCar
│  │  ├─nanosuit
│  │  └─pine
│  ├─bin
│  │  ├─res
│  │  ├─rsLibs
│  │  └─rsObj
   ├─jni
...

### 1. 
之前装过Android Studio, 只需要新安装ant，cygwin（有git bash 的话也不用）和ndk，这些没有特殊要求。
### 2.
新下个r-24.2版本的sdk，把tools 文件夹拷贝，里面有没被弃用的android.bat。
### 3. 
cygwin .bashrc(git bash)里面设置如下：

``` bash
export JAVA_HOME="/d/Program Files/Android/Android Studio/jre"
export ANDROID_NDK=/d/android-ndk-r21b-windows-x86_64/android-ndk-r21b
export ANDROID_SDK=/d/android/sdk
export ANT=/d/android/apache-ant-1.9.15/bin
export PATH=$PATH:${ANDROID_NDK}
export PATH=$PATH:${ANT}
export PATH=$PATH:${ANDROID_SDK}/tools
export PATH=$PATH:${ANDROID_SDK}/platform-tools
export PATH=/d/android/sdk/cmake/3.10.2.4988404/bin:$PATH
```
最后一行的是android studio里面装的cmake路径。确认里面有ninja.exe。
### 4. 
进入Android文件夹。install.sh，放在了${ANDROID_SDK}/tools文件夹
```bash
android.bat update project -p . -t android-29
cd jni
ndk-build.cmd NDK_DEBUG=1 
cd ..
ant debug
adb install -r -t bin/NativeActivity-debug.apk
```
加了一个 NDK_DEBUG=1 和 adb install 里的-t 选项用于debug。
### 5. 
debug的时候比较麻烦，创建一个平行的glfw实现的工程并且希望运行结果相同。
实在不行的话就得用ndk-gdb.cmd，然后list。
比较实用的debug 方式是增加一个宏：
```C++
#include <android/log.h>
#define LOG_TAG "OOPS"
#define  LOG(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
```
调试的时候`adb logcat | grep OOPS`。
### 6. 
assimp 似乎也移植存活了，用到的命令行：
```bash
#!/usr/bin/bash
rm -r CMakeFiles
rm CMakeCache.txt
export CMAKE_ANDROID_NDK=/d/android-ndk-r21b-windows-x86_64/android-ndk-r21b
export CMAKE_TOOLCHAIN=$CMAKE_ANDROID_NDK/build/cmake/android.toolchain.cmake
export PATH=$PATH:/d/android-ndk-r21b-windows-x86_64/android-ndk-r21b/toolchains/llvm/prebuilt/windows-x86_64/bin
cmake -G Ninja -DASSIMP_ANDROID_JNIIOSYSTEM=ON -DANDROID_API=29 -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN}  -DCMAKE_SYSTEM_NAME=Android -DANDROID_PLATFORM=29 -DANDROID_ABI=arm64-v8a -DCMAKE_ANDROID_NDK=/d/android-ndk-r21b-windows-x86_64/android-ndk-r21b   -DASSIMP_NO_EXPORT=ON -DASSIMP_BUILD_TESTS=OFF \
-DASSIMP_BUILD_OBJ_IMPORTER=TRUE -DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=FALSE -DASSIMP_ANDROID_JNIIOSYSTEM=ON ..
cmake --build .
```
编译完之后`file libassimp.so`看看类型是否为aarch64( arm64v8a)

原生应用需要用到jni port，编译的时候增加选项-DASSIMP_ANDROID_JNIIOSYSTEM=ON。否则读取模型失败。
Android.mk里面增加一个子项目并引用：
```
LOCAL_PATH			:= $(call my-dir)
SRC_PATH			:= ../..
COMMON_PATH			:= $(SRC_PATH)/../../Common
COMMON_INC_PATH		:= $(COMMON_PATH)/Include
COMMON_SRC_PATH		:= $(COMMON_PATH)/Source
# GLM_INC_PATH		:= $(SRC_PATH)

include $(CLEAR_VARS)
LOCAL_MODULE 	:= assimp
LOCAL_SRC_FILES	:= $(SRC_PATH)/libassimp.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE    := Shadows
LOCAL_CFLAGS    += -DANDROID


LOCAL_SRC_FILES := $(SRC_PATH)/main2.cpp \
				   $(SRC_PATH)/glfm_platform_android.c	\
				   $(COMMON_SRC_PATH)/esTransform.c \
				   $(SRC_PATH)/AndroidJNIIOSystem.cpp 

# LOCAL_SRC_FILES := $(COMMON_SRC_PATH)/esShader.c \
				   

LOCAL_C_INCLUDES	:= $(SRC_PATH) \
					   $(COMMON_INC_PATH) 
					#    $(GLM_INC_PATH) 
				   
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv3


LOCAL_STATIC_LIBRARIES := android_native_app_glue
LOCAL_SHARED_LIBRARIES	:=assimp
include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)

```
忍无可忍把assimp的AndroidJNIIOSystem.cpp拿出来编译了。使用的时候还是要加一两行：
```C++
#include <assimp/port/AndroidJNI/AndroidJNIIOSystem.h>
...
        Assimp::Importer importer;
        Assimp::AndroidJNIIOSystem* ioSystem=new Assimp::AndroidJNIIOSystem(glfmAndroidGetActivity());
        if ( ioSystem!=nullptr ) {
            importer.SetIOHandler(ioSystem);
        }  

```
### 7. 需要导入的文件直接放到和jni同级的`asset/`文件夹，可以直接以文件名访问。
编程的时候把所有的glfm的函数放在`extern "C"{}`；shader要改，报的错类似no default precision。增加一行：`precision lowp float;`(fragment shader)


后面要重启的话先重读一遍[CMake  |  Android NDK  |  Android Developers (google.cn)](https://developer.android.google.cn/ndk/guides/cmake)系列的文档；
以及assimp 的文档
[assimp/port/AndroidJNI at master · assimp/assimp (github.com)](https://github.com/assimp/assimp/tree/master/port/AndroidJNI)
2020-12-07




