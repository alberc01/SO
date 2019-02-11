#!/bin/bash

MPOINT="./mount-point"
if  test -d tmp; then
	rm -r tmp
echo "Se ha borado el contenido de tmp/ "
fi
mkdir tmp
echo "Se ha creado tmp/ "

cat  src/fuseLib.c
cp  src/fuseLib.c tmp/file1.txt
echo "se ha creado file1.txt con fuseLib.h en tmp/ "
cp  src/myFs.h tmp/file2.txt
echo "se ha creado file2.txt con myFs.h en tmp/ "
cp src/myFS.h $MPOINT/
echo "se ha creado myFS.h en mount-point SF"
cp src/fuseLib.c $MPOINT/
echo "se ha creado fuseLib.c en mount-point SF"

./my-fsck-static virtual-disk
echo "se ha auditado el disco virtual"
diff src/fuseLib.c $MPOINT/fuseLib.c && src/myFS.h $MPOINT/myFS.h
echo "diff realizado entre originales y copias"
truncate -s -4K tmp/file1.txt
echo "se a truncado el file1.txt en tmp"
truncate -s -4K $MPOINT/fuseLib.c
echo "se a truncado el fuseLib.c en nuestro SF"

./my-fsck-static virtual-disk
echo "se ha auditado el disco virtual"
diff src/fuseLib.c $MPOINT/fuseLib.c
diff src/fuseLib.c tmp/file1.txt
echo "realizado diff entre originales y truncados"

cp src/myFS.c $MPOINT/file3.txt
echo "se ha creado file3.txt con myFS.c en mount-point SF"

./my-fsck-static virtual-disk
diff src/myFS.c $MPOINT/file3.txt
echo "disco auditado y comprobacion con diff del fichero copiado"

truncate -s +8K tmp/file2.txt
echo "se ha truncado el file2.txt en la carpeta tmp con +8K"
truncate -s +4K $MPOINT/myFS.h
echo "se ha truncado myFS.h en FS con +4K"

./my-fsck-static virtual-disk
echo "se ha auditado el disco virtual"
diff src/myFS.h tmp/file2.txt
echo "realizado diff entre original myFS.h y truncado file2.txt"
diff src/myFS.h $MPOINT/myFS.h
echo "realizado diff entre original myFS.h y truncado myFS.h de FS"
