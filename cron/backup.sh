#!/bin/bash
touch 2.txt

./m4tools "{\"operation\":\"backup\", \"params\":{}}"
sshpass -p "" scp -r snake@ramsnake.net:/materia/materia.back ./materia.bu$(date +%s)

#news
java -jar NewsMaker.jar

#invest
./M2/surveys/invest.py 2RHVKGRYT9UL12E5
./M2/analyzers/invest.py
