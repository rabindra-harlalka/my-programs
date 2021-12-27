HOME:=$(shell pwd)
OS_NAME := $(shell uname -s | tr A-Z a-z)

os:
	@echo $(OS_NAME)

install:
    ifeq ($(OS_NAME), Darwin)
    brew tap isen-ng/dotnet-sdk-versions && \
    brew install --cask dotnet-sdk5-0-400 && \
    brew install --cask dotnet
    endif
	
clean:
	dotnet clean	

restore:
	dotnet restore

build:
	dotnet build

run:	
	dotnet $(HOME)/ConsoleApp1/bin/Debug/net5.0/MongoDBCacheApp.dll $(HOME)/ConsoleApp1/bin/Debug/net5.0/queries/batch.txt
