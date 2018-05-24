#!/bin/sh

./crashtest 4

cd test

echo "touch a.txt"
touch a.txt

echo "echo hello > a.txt"
echo hello > a.txt

echo "ln a.txt aduplicate.txt"
ln a.txt aduplicate.txt

echo "touch b.txt"
touch b.txt

echo "echo world >> b.txt"
echo world >> b.txt

echo "ln -s a.txt a_2duplicate.txt"
ln -s a.txt a_2duplicate.txt

echo "rm b.txt"
rm b.txt

echo "cat a.txt"
cat a.txt

#Error
echo "cat b.txt"
cat b.txt

#Fail
echo "ln 1_copy3.txt copy5"
ln aduplicate.txt test.txt

#Fail
echo "cat a_2duplicate.txt"
cat a_2duplicate.txt