#!/bin/bash

COMMAND="./esecuele"

rm -rf libreria_db

$COMMAND createdb libreria_db

$COMMAND define libreria_db << EOF
TABLE usuarios 6 LNG STR STR STR LNG STR 
TABLE ventas 4 STR LNG STR STR
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
usuarios SEQUENTIAL 1 STR jack C_COLEQCTE SELECT LNG 0 P_COL 1 PROJECT ventas SEQUENTIAL PRODUCT 0 2 C_COLEQCOL SELECT LNG 0 P_COL STR 3 P_COL 2 PROJECT libros SEQUENTIAL PRODUCT 1 9 C_COLEQCOL SELECT STR 3 P_COL 1 PROJECT
EOF

$COMMAND query libreria_db <<EOF
usuarios SEQUENTIAL 1 STR jack C_COLEQCTE SELECT LNG 0 P_COL 1 PROJECT ventas SEQUENTIAL PRODUCT 0 2 C_COLEQCOL SELECT LNG 0 P_COL STR 3 P_COL 2 PROJECT libros SEQUENTIAL PRODUCT 1 9 C_COLEQCOL SELECT STR 3 P_COL 1 PROJECT COUNT
EOF

$COMMAND query libreria_db <<EOF
usuarios SEQUENTIAL 5 LIMIT
EOF

$COMMAND query libreria_db <<EOF
usuarios SEQUENTIAL 90 OFFSET
EOF

$COMMAND query libreria_db <<EOF
usuarios SEQUENTIAL 5 LIMIT usuarios SEQUENTIAL 90 OFFSET UNION
EOF
