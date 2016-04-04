@echo off
neko bin\req.n
node bin\req.js
luajit bin\req.lua
java -jar bin\java\Req.jar
bin\cs\bin\Req.exe
bin\cpp\Req.exe
pause
