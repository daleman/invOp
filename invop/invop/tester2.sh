echo "haciendo el segundo test"
echo "bbound"

#./conj 1 0 0 0 < in1/frb30-15-2.mis > bbound_test2.out

echo "branch and cut"

./conj 2 500 2000 2500 < in1/frb30-15-2.mis > bcut_500_2000_2500_test2.out

./conj 2 100 100 10 < in1/frb30-15-2.mis > bcut_100_100_10_test2.out

./conj 2 100 100 5 < in1/frb30-15-2.mis > bcut_100_100_5_test2.out

./conj 2 100 100 200 < in1/frb30-15-2.mis > bcut_100_100_200_test2.out

echo "haciendo cut and branch"
./conj 3 500 2000 2500 < in1/frb30-15-2.mis > bcut_500_2000_2500_test2.out

./conj 3 200 2000 2200 < in1/frb30-15-2.mis > cutBranch_200_2000_2200_test2.out

./conj 3 40 10 50 < in1/frb30-15-2.mis > cutBranch_40_10_50_test2.out
