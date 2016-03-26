:: 配置路径, 注意 SDK 版本和 app.xml 中 <application xmlns 一致
@echo off
set FLEX_SDK=E:\SDKS\AIR_210
set PATH=%PATH%;%FLEX_SDK%\bin


:: 配置 certificate 文件信息
set CERT_NAME=me
set CERT_PASS=pwd
set CERT_FILE=cert.p12
set APP_XML=app.xml
:: ========== 创建 certificate ==========

if not exist %CERT_FILE% call adt -certificate -cn %CERT_NAME% 1024-RSA %CERT_FILE% %CERT_PASS%

:: ========== 生成 AIR ==========
:: AIR output

::if not exist %AIR_PATH% md %AIR_PATH%

::set OUTPUT=app.air
set APP_DIR=bin

set OPTIONS=-tsa none
set SIGNING_OPTIONS=-storetype pkcs12 -keystore %CERT_FILE% -storepass %CERT_PASS%

::adt -package %OPTIONS% %SIGNING_OPTIONS% %OUTPUT% %APP_XML% %APP_DIR%
adt -package %OPTIONS% %SIGNING_OPTIONS% -target bundle out %APP_XML% %APP_DIR%