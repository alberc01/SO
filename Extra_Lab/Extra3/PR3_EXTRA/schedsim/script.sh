#!/bin/bash

listSched=( "RR" "SJF" "FCFS" "PRIO" )

make clean
make

#preguntamos por la ruta del fichero, si el fichero no existe, mostramos mensaje y volvemos a pedir

echo '¿Qué fichero quiere simular?'
echo 'Indica la ruta del fichero.'
read fichero

while [ ! -f $fichero ]; do
	echo 'El fichero no existe.'
	echo '¿Qué fichero quiere simular?'
	echo 'Indica la ruta del fichero.'
	read fichero
done

#pedimos el numero de CPUs que queremos simular
echo '¿Cuantas CPUs deseas simular?'
echo 'El rango de CPUs es: [1 .. 8]'
read maxCPUs

#si el numero de CPUs que se desea simular es superior a 8, se informa del error y se vuelve a preguntar el numero de CPUs a simular
while [ $maxCPUs -gt 8 ] || [ $maxCPUs -le 0 ] ; do
	echo 'El numero de CPUs introducidas no esta dentro del rango valido'
	echo '¿Cuantas CPUs deseas simular?'
	echo 'El rango de CPUs es: [1 .. 8]'
	read maxCPUs
done

if [ -d resultados ]; then
	rm -r resultados
fi
mkdir resultados



for nameSched in "${listSched[@]}"; do
	cpus=1
	while [[ $cpus -le $maxCPUs ]]; do
		if [ "$nameSched" != "RR" ] && [ "$nameSched" != "PRIO" ]; then 
			./schedsim -n "$cpus" -s "$nameSched" -i "$fichero"
		else 
			./schedsim -p -n "$cpus" -s "$nameSched" -i "$fichero"
		fi

		i=0
		while [[ $i -lt $cpus ]]; do
			mv CPU_$i.log resultados/$nameSched-CPU_$cpus.log
			let i+=1
		done
		cd ../gantt-gplot
		 ./generate_gantt_chart ../schedsim/resultados/"$nameSched"-CPU_"$cpus".log
		cd ../schedsim
		let cpus+=1 
	done

done 
