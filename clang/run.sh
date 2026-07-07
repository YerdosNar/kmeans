#!/usr/bin/env bash

if [ $# -ne 1 ]; then
        read -p "Which one (T)est, (M)NIST: " choice

        if [ "$choice" == "M" ] || [ "$choice" == "m" ]; then
                gcc -o mnist mnist_img.c kmeans.c -lm -lSDL2 -O3
                ./mnist
                rm mnist
        elif [ "$choice" == "T" ] || [ "$choice" == "t" ]; then
                gcc -o test test.c kmeans.c -lm -lSDL2 -O3
                ./test
                rm test
        else
                echo "Invalid input"
        fi
else
        if [ "$1" == "-M" ] || [ "$1" == "--mnist" ] || [ "$1" == "-m" ]; then
                gcc -o mnist mnist_img.c kmeans.c -lm -lSDL2 -O3
                ./mnist
                rm mnist
        elif [ "$1" == "-T" ] || [ "$1" == "--test" ] || [ "$1" == "-t" ]; then
                gcc -o test test.c kmeans.c -lm -lSDL2 -O3
                ./test
                rm test
        elif [ "$1" == "--help" ] || [ "$1" == "-h" ]; then
                echo "$0 [options]"
                echo ""
                echo "Options:"
                echo "          -M/-m/--mnist       to run mnist images"
                echo "          -T/-t/--test        to run custom test"
        else
                echo "Invalid argument"
        fi
fi

