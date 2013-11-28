make clean
make

echo "haciendo el primer test"
echo "bbound"
#./conj 1 0 0 0 < in1/frb30-15-1.mis > bbound_test1.out

echo "branch and cut"
./conj 2 500 2000 2500 < in1/frb30-15-1.mis > bcut_500_2000_2500_test1.out

./conj 2 100 100 10 < in1/frb30-15-1.mis > bcut_100_100_10_test1.out

./conj 2 100 100 5 < in1/frb30-15-1.mis > bcut_100_100_5_test1.out

./conj 2 100 100 200 < in1/frb30-15-1.mis > bcut_100_100_200_test1.out

echo "haciendo cut and branch"
./conj 3 500 2000 2500 < in1/frb30-15-1.mis > bcut_500_2000_2500_test1.out

./conj 3 200 2000 2200 < in1/frb30-15-1.mis > cutBranch_200_2000_2200_test1.out

./conj 3 40 10 50 < in1/frb30-15-1.mis > cutBranch_40_10_50_test1.out

