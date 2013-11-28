make clean
make

echo "haciendo el test random "
echo "bbound"
./conj 1 0 0 0 < in1/dsjc250_5.in > bbound_random.out

echo "branch and cut"
./conj 2 500 2000 2500 < in1/dsjc250_5.in > bcut_500_2000_2500_random.out

./conj 2 100 100 10 < in1/dsjc250_5.in > bcut_100_100_10_random.out

./conj 2 100 100 5 < in1/dsjc250_5.in > bcut_100_100_5_random.out

./conj 2 100 100 200 < in1/dsjc250_5.in > bcut_100_100_200_random.out

echo "haciendo cut and branch"
./conj 3 500 2000 2500 < in1/dsjc250_5.in > bcut_500_2000_2500_random.out

./conj 3 200 2000 2200 < in1/dsjc250_5.in > cutBranch_200_2000_2200_random.out

./conj 3 40 10 50 < in1/dsjc250_5.in > cutBranch_40_10_50_random.out

