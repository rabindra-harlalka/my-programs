# MongoDB Cache Installations Instructions

## Install
Install .NET SDK 5 if you don't have already. The Makefile included with this program can do it for you.
```
make install
```

## Verify
Verify that you have dotnet installed properly.
```
make verify
```
The output should be similar to the below result : 

```
dotnet --info
.NET SDK (reflecting any global.json):
 Version:   5.0.404
 Commit:    72c68b830a

Runtime Environment:
 OS Name:     Mac OS X
 OS Version:  12.1
 OS Platform: Darwin
 RID:         osx-x64
 Base Path:   /usr/local/share/dotnet/sdk/5.0.404/

Host (useful for support):
  Version: 6.0.1
  Commit:  3a25a7f1cc

.NET SDKs installed:
  5.0.404 [/usr/local/share/dotnet/sdk]

.NET runtimes installed:
  Microsoft.AspNetCore.App 5.0.13 [/usr/local/share/dotnet/shared/Microsoft.AspNetCore.App]
  Microsoft.NETCore.App 5.0.13 [/usr/local/share/dotnet/shared/Microsoft.NETCore.App]
  Microsoft.NETCore.App 6.0.1 [/usr/local/share/dotnet/shared/Microsoft.NETCore.App]

To install additional .NET runtimes or SDKs:
  https://aka.ms/dotnet-download
```

## Build
Compile the program.
```
make build
```

The output should be similar to the below result : 

```
dotnet build
Microsoft (R) Build Engine version 16.11.2+f32259642 for .NET
Copyright (C) Microsoft Corporation. All rights reserved.

  Determining projects to restore...
  All projects are up-to-date for restore.
  MongoDBQueryCache -> /Users/saryakumar/github/my-programs/cacheapp/MongoDBQueryCache/bin/Debug/net5.0/MongoDBQueryCache.dll
  TestProject1 -> /Users/saryakumar/github/my-programs/cacheapp/TestProject1/bin/Debug/net5.0/TestProject1.dll
  MongoDBCacheApp -> /Users/saryakumar/github/my-programs/cacheapp/ConsoleApp1/bin/Debug/net5.0/MongoDBCacheApp.dll

Build succeeded.
    0 Warning(s)
    0 Error(s)

Time Elapsed 00:00:01.32
```

## Run
Run the program.
```
make run
```

## Output

Once you use the `make run` command then the results should be displayed as follows: 

```
dotnet /Users/saryakumar/github/my-programs/cacheapp/ConsoleApp1/bin/Debug/net5.0/MongoDBCacheApp.dll /Users/saryakumar/github/my-programs/cacheapp/ConsoleApp1/bin/Debug/net5.0/queries/batch.txt
Reading the input queries file /Users/saryakumar/github/my-programs/cacheapp/ConsoleApp1/bin/Debug/net5.0/queries/query1.json
Query cache hit: False
Query returned 794 items.
Time elapsed: 2367 ms.
================================================================================
Reading the input queries file /Users/saryakumar/github/my-programs/cacheapp/ConsoleApp1/bin/Debug/net5.0/queries/query2.json
Query cache hit: True
Query returned 606 items.
Time elapsed: 91 ms.
================================================================================
Reading the input queries file /Users/saryakumar/github/my-programs/cacheapp/ConsoleApp1/bin/Debug/net5.0/queries/query3.json
Query cache hit: True
Query returned 606 items.
Time elapsed: 70 ms.
================================================================================
Reading the input queries file /Users/saryakumar/github/my-programs/cacheapp/ConsoleApp1/bin/Debug/net5.0/queries/query4.json
Query cache hit: True
Query returned 50 items.
Time elapsed: 16 ms.
================================================================================
Reading the input queries file /Users/saryakumar/github/my-programs/cacheapp/ConsoleApp1/bin/Debug/net5.0/queries/query5.json
Query cache hit: True
Query returned 50 items.
Time elapsed: 16 ms.
================================================================================
Reading the input queries file /Users/saryakumar/github/my-programs/cacheapp/ConsoleApp1/bin/Debug/net5.0/queries/query7.json
Query cache hit: False
Query 1 evicted from cache.
Evicted all the 794 results belonging to query 1.
Query returned 608 items.
Time elapsed: 647 ms.
================================================================================
Reading the input queries file /Users/saryakumar/github/my-programs/cacheapp/ConsoleApp1/bin/Debug/net5.0/queries/query1.json
Query cache hit: True
Query returned 185 items.
Time elapsed: 25 ms.
================================================================================
Reading the input queries file /Users/saryakumar/github/my-programs/cacheapp/ConsoleApp1/bin/Debug/net5.0/queries/query1.json
Query cache hit: True
Query returned 185 items.
Time elapsed: 19 ms.
================================================================================
```
