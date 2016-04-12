##OSlab3-Synchronize

####buffer
Array of integer

####thread type1:Producer
create a random number on [0,99] and push it into buffer1.
totally produce 1000,000 numbers

####thread type2:Consumer&&Proucer
get 3 numbers from buffer1.
sum them up.
push the sum into buffer2.

####thread type3:Consumer
get 1 number from buffer2.
factor the number into prime numbers.

####instance:
#####There are:
######2*"buffer"
buffer1's size is 100
buffer2's size is 200
######4*"Producer"
######3*"Producer && Consumer"
######2*"Consumer"

####output
#####3 charts
1:amount of random number produced by "Producer"
2:amount of the "sum" produced by "Producer&&Consumer"
3:amount of "factors" counted by "Consumer"

#####Count produce or consume times of each thread.
