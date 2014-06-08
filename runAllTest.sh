#/bin/bash

dimensions=(128 512 1024)

for (( i = 0 ; i < ${#dimensions[@]} ; i++ )) do
	echo '------------------------------------------------------------'
	echo 'Corriendo con dimension : ' ${dimensions[$i]}
	
	echo 'Secuencial: '
	cd secuencial
	./prueba matmul ${dimensions[$i]}
	echo 'Secuencial con O3: '
	./pruebaO3 matmul ${dimensions[$i]}
	cd ..

	echo 'OpenMP: '
	cd openmp
	./prueba matmul ${dimensions[$i]}
	cd ..
done