#!/bin/bash

echo "introduzca el directorio donde esta montado el sistema"
read MPOINT
op=0

while[op!=6]; do

echo "1.Lista el contenido del directorio raiz."
echo "2.Hacer un enlace rigido."
echo "3.Mostrar el contenido de un fichero."
echo "4.Crear un nuevo fichero."
echo "5.Borar un fichero."
echo "6.Finalizar el script."
echo "INTRODUCE OPCION:"
read op

if [op==1]; then
ls -la $MPOINT
else if [op==2]; then

echo " introduce origen"
read origen
echo " introduce destino"
read destino

ln $MPOINT/$origen $MPOINT/$destino
echo "se ha creado enlace duro entre"
else if [op==3]; then
echo "nombre fichero"
read nombre
echo $MPOINT/$nombre

else if [op==4]; then
echo "nombre del fichero"
read nombre
touch $MPOINT/$nombre

else if [op==5]; then
echo "nombre del fichero"
read nombre
unlink $MPOINT/$nombre

else if [op>6 || op<1]; then
echo "opcion invalida"

fi

done








