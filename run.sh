export CXXFLAGS="-I/home/yangjunjie/opt/anaconda/include/"
export MKL_NUM_THREADS=1
export OMP_NUM_THREADS=1

rm -rf /home/yangjunjie/work/simple-gemm/build/;
mkdir /home/yangjunjie/work/simple-gemm/build/;
cd /home/yangjunjie/work/simple-gemm/build/;
cmake ..; make VERBOSE=1 -j4; cd -

cd /home/yangjunjie/work/simple-gemm/build/;
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