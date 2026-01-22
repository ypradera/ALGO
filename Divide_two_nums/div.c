/*
Given two integers dividend and divisor, divide two integers without using multiplication, division, and mod operator.
The integer division should truncate toward zero, which means losing its fractional part. For example, 8.345 would be truncated to 8, and -2.7335 would be truncated to -2.
Return the quotient after dividing dividend by divisor.

Note: Assume we are dealing with an environment that could only store integers within the 32-bit signed integer range: [−231, 231 − 1]. For this problem, if the quotient is strictly greater than 231 - 1, then return 231 - 1, and if the quotient is strictly less than -231, then return -231.
Example 1
Input: dividend = 10, divisor = 3
Output: 3
Explanation: 10/3 = 3.33333.. which is truncated to 3.
Example 2:

Input: dividend = 7, divisor = -3
Output: -2
Explanation: 7/-3 = -2.33333.. which is truncated to -2.
 
Constraints:
-231 <= dividend, divisor <= 231 - 1
divis
*/

#include<stdio.h>
#include<limits.h>
#include<stdint.h>

int divide(int dividend, int divisor){
    // Handle Overflow case 
    if(dividend == INT_MIN && divisor == -1 ){
        //Both being negative make 2^31 positive and int32_t only goes to 2^31 -1
        return INT_MAX;
    }
    //Track the sign from the input
    // both negative or both positive results in positive quotient
    // either one negative results in negative quotient 
    // Xoring the result from the # < 0 , if differ then we have negative result  
    int sign = ((dividend < 0) ^ (divisor < 0)) ? -1 : 1; 
    // Work with negative values since abs(INT_MIN)or -2^31 cannot be represented in int32_t MAX= 2^31 -1
    // and the vlues are passed to function as int
    int a_dividend = dividend > 0 ? -dividend : dividend;
    int a_divisor = divisor  > 0 ? -divisor  : divisor;
    int quotient = 0;
    
    // Becuase the numbers are negative the condition is reversed
    //EX -10 / -3
    while(a_dividend <= a_divisor){
        int temp_divisor = a_divisor; 
        uint32_t count = 1u; 
        //INT_MIN >> 1 is half of INT_MIN since we add temp_divisor to itself esentially
        //  multiplying by 2 we want to make srue we dont overflow when the adding. Default 
        // to adding instead << 1 because dealing with ints for the given numbers is better. 

        // ``````````  This logic is flipped because we are working with negatives ````````````
        //````` means if divisor is bigger or equal than 1/2 int32_t and 2xdivisor is less or equal to dividend ```````````
        while ((temp_divisor >= (INT_MIN >> 1)) && ((temp_divisor + temp_divisor) >= a_dividend)){
            temp_divisor += temp_divisor; // double the temp_divisor, like multiplying by temp_divisor by 2
            count <<= 1; // double the count, can do it this way since count is unsigned and safe from overflow
        }

        quotient -= (int32_t)count;  // subtract count from quotient since we are working with negatives
        // trying to see how many times divisor fits into dividend
        a_dividend -= temp_divisor; // reduce the dividend

    }
//Remember quotient is negative if sign representing negative we can return quotient as negative
// negative quotient with sign -1 becomes positive
return sign == 1 ? -quotient : quotient;

}


 int main(){
    int dividend[] = {10, 7, -10, -7, INT_MIN, INT_MAX, INT_MIN, (INT_MIN +1) };
    int divisor[]  = {3, -3,   3, -3,  -1,        1,    INT_MIN,  INT_MAX};
    int size = sizeof(dividend) / sizeof(dividend[0]);

    for(int i = 0; i < size; i++){
        int result = divide(dividend[i], divisor[i]);
        printf("%d - divide(%d, %d) = %d\n",i+1,  dividend[i], divisor[i], result);
    }
    return 0;
 }