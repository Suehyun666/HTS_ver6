#!/bin/bash

LRELEASE=~/Qt/6.7.3/gcc_64/bin/lrelease
I18N_DIR=resources/i18n

cd "$(dirname "$0")"

echo "Compiling translation files..."

for ts_file in $I18N_DIR/*.ts; do
    echo "Processing: $ts_file"
    $LRELEASE "$ts_file"
done

echo "Done!"
