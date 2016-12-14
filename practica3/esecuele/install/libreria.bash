#!/bin/bash

COMMAND="valgrind --leak-check=full ./esecuele"

rm -rf libreria_db

$COMMAND createdb libreria_db

$COMMAND define libreria_db << EOF
TABLE usuarios 4 STR STR STR STR
TABLE ventas 4 STR STR STR STR
TABLE isbns_precios 2 STR DBL
TABLE libros 8 STR STR STR STR STR STR STR STR
EOF

$COMMAND insert libreria_db << EOF
COPY usuarios ./example_files/usuarios.txt
COPY ventas ./example_files/ventas.txt
COPY isbns_precios ./example_files/isbns_precios.txt
COPY libros ./example_files/LIBROS_FINAL.txt
EOF

$COMMAND query libreria_db <<EOF
usuarios SEQUENTIAL 1 STR jack C_COLEQCTE SELECT
EOF

$COMMAND query libreria_db <<EOF
usuarios SEQUENTIAL 1 STR jack C_COLEQCTE SELECT ventas SEQUENTIAL PRODUCT
EOF

$COMMAND query libreria_db <<EOF
usuarios SEQUENTIAL 1 STR jack C_COLEQCTE SELECT ventas SEQUENTIAL PRODUCT 0 5 C_COLEQCOL SELECT
EOF

$COMMAND query libreria_db <<EOF
usuarios SEQUENTIAL 1 STR jack C_COLEQCTE SELECT ventas SEQUENTIAL PRODUCT 0 5 C_COLEQCOL SELECT libros SEQUENTIAL PRODUCT
EOF
#6 15 C_COLEQCOL SELECT
#EOF

#$COMMAND query libreria_db << EOF
#libros SEQUENTIAL usuarios 1 STR "jack" C_COLEQCTE SELECT SEQUENTIAL PRODUCT
#EOF
