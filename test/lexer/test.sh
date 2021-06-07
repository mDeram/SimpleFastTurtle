SRC=../../src/

cd $(dirname $0)

echo -e "\t\tTesting lexer"

for input in *.input; do
    name=$(echo $input | cut -d'.' -f1)
    echo -e "\tTesting $name"
    rm main.sft
    cp $input main.sft
    ./$SRC/sft >/dev/null
    output="$name.output"
    if [ -f $output ]; then
        diff --color $output output.l
    fi
    echo -e "\tDone $name"
done

echo -e "\t\tDone"
