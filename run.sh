# export CXXFLAGS="-I/home/yangjunjie/opt/anaconda/include/"
conda create -n simple-gemm python=3.10 anaconda -y
conda activate simple-gemm
conda install cmake conda-forge::openblas -y

export MKL_NUM_THREADS=1
export OMP_NUM_THREADS=1

lscpu | grep "Model name:"
lscpu | grep -A 3 "L1d cache:"

export PREFIX=$(pwd);
rm -rf $PREFIX/build/; mkdir $PREFIX/build/;
cd $PREFIX/build/;
cmake -DNN=64 -DBLOCK_SIZE=128 ..; make VERBOSE=1 -j4; cd -

cd $PREFIX/build/;
for m in 256 512 1024 2048; do
  for f in main-*; do
    if [[ $f != *"j-o3"* ]]; then
      continue;
    fi

    echo ""
    echo "Running $f with arguments $m $m $m..."
    # ./$f "$m" "$m" "$m"
    perf stat -e cache-references,cache-misses \
          -e instructions,cycles \
          ./$f "$m" "$m" "$m"
    echo ""
  done
done

cd -