# UCloud 对象存储官方 C 语言 SDK

[![Software License](https://img.shields.io/github/license/saltstack/salt)](LICENSE)

> Micromanagers tend to stay micromanagers. Workaholics tend to stay workaholics. Hustlers tend to stay hustlers.   
> <*It doesn't have to be crazy at work*>

## US3 (原名UFile) 对象存储基本概念
在对象存储系统中，存储空间（Bucket）是文件（File）的组织管理单位，文件（File）是存储空间的逻辑存储单元。对于每个账号，该账号里存放的每个文件都有唯一的一对存储空间（Bucket）与键（Key）作为标识。我们可以把 Bucket 理解成一类文件的集合，Key 理解成文件名。由于每个 Bucket 需要配置和权限不同，每个账户里面会有多个 Bucket。在 US3 里面，Bucket 主要分为公有和私有两种，公有 Bucket 里面的文件可以对任何人开放，私有 Bucket 需要配置对应访问签名才能访问。  
使用本 SDK 你不需要考虑签名，包装 URL，处理 HTTP response code 等一系列非常繁琐的事情。

## 安装与使用
本项目使用 cmake 编译（最小 2.8 版本），为跨平台方便，代码使用 ansi c 和 posix 标准库编写，基于 [libcurl](https://curl.haxx.se/) 这是 SDK 惟一的外部依赖库。

### 编译
```bash
mkdir -p build && cd build && cmake ../ && make
```
编译完后把 [api.h](https://github.com/ufilesdk-dev/ufile-csdk/blob/master/lib/api.h) 和编译好的动态库放到你项目中可以 include 地方即可。现在暂时还没有考虑 windows 平台的使用，你也可以使用 mingw 直接编译本项目。

### 运行测试用例
```
cd build;
./example/head_file
./example/mput
./example/put
./example/download
./example/bucket
```

## 功能说明
[lib/api.h](https://github.com/ufilesdk-dev/ufile-csdk/blob/master/lib/api.h) 里面是所有暴露出来的接口，每个接口的注释就是使用说明。
**强烈建议您在使用前仔细阅读接口说明**，以节省您的 debug 时间。  

本 SDK 的功能对应的接口名称如下：  
**全局 SDK 初始化** ufile_sdk_initialize   
**全局 SDK 清理** ufile_sdk_cleanup   
**获取文件信息** ufile_head (需要调用 ufile_free_config 释放内存)。   
**简单上传** ufile_put_buf 和 ufile_put_file  
**分片上传** 步骤1(ufile_multiple_upload_init)，步骤2(ufile_multiple_upload_part)，步骤3(ufile_multiple_upload_finish) 取消分片上传(ufile_multiple_upload_abort)。  
**下载** 下载文件(ufile_download), 分片下载(ufile_download_piece)   
**bucket 创建** ufile_bucket_create  
**bucket 删除** ufile_bucket_delete  
详细的使用方式见 [examples](https://github.com/ufilesdk-dev/ufile-csdk/tree/master/examples) 下的测试用例。

## 许可证
[Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0.html)
