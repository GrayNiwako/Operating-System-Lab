#!/bin/sh

NewDirectoryName=$1 #将第一个变量赋值到NewDirectoryName里
mkdir $NewDirectoryName #创建一个新目录$NewDirectoryName
echo "Directory $NewDirectoryName created successed" 
                   #输出字符串信息表示创建成功

touch name.txt #在当前目录下创建name.txt文件
touch stno.txt #在当前目录下创建stno.txt文件
echo "TextFile name.txt and stno.txt created successed" #创建成功

echo "Qian Tinghan" > name.txt #输入字符串信息到name.txt里
echo "10152130122" > stno.txt #输入字符串信息到stno.txt里
echo "Information written successed" #输入成功

cp name.txt $NewDirectoryName #将name.txt复制到$NewDirectoryName目录下
cp stno.txt $NewDirectoryName #将stno.txt复制到$NewDirectoryName目录下
echo "TextFile name.txt and stno.txt copied successed" #复制成功