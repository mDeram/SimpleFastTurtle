SRC=../../src/

cd $(dirname $0)

for input in *.input; do
    name=$(echo $input | cut -d'.' -f1)
    echo "Testing $name"
    rm main.sft
    cp $input main.sft
    ./$SRC/sft >/dev/null
    output="$name.output"
    if [ -f $output ]; then
        diff --color $output output.l
    fi
    echo "Done $name"
done
