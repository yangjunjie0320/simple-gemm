# conda env create -f environment.yml -n simple-gemm
conda activate simple-gemm

export MKL_NUM_THREADS=1
export OMP_NUM_THREADS=1

# lscpu | grep "Model name:"
# lscpu | grep -A 3 "L1d cache:"

export PREFIX=$(pwd);
rm -rf $PREFIX/build/; mkdir $PREFIX/build/; cd $PREFIX/build/;
cmake -DCHECK=1 ..; make VERBOSE=1 -j4; cd -

cd $PREFIX/build/;
for l in 64 128 192 256 320 384 448 512 640 768 896 1024; do
  for f in main-*; do
    if ! [[ $f == *"block"* ]]; then
      continue;
    fi

    echo ""
    echo "Running $f with arguments $l ..."
    ./$f "$l" "10"
    # perf stat -e cache-references,cache-misses \
    #       -e instructions,cycles \
    #       ./$f "$m" "$m" "$m"
    echo ""
  done
done

cd -
