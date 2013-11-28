
# hago branch and bound 10 veces


for num in 1 2 3 4 5 6 7 8 9 10
do
 echo "Haciendo el $num branch and bound"
   ./conj 1 0 0 0 < in1/frb30-15-1.mis > bbound_${num}.out

done
#   