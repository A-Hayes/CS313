#include <iostream>
#include <string.h>
using namespace std;
 
class Header{
private:
    char used;
    int payloadsize;
    char* data;
public:
    Header (){
        used = 0, payloadsize = -1, data = NULL;
    }
    Header (int ps, char initvalue = 0){
        used = 0;
        payloadsize = ps;
        data = new char [payloadsize];
        memset (data, initvalue, payloadsize);
    }
    int getsummation (){
        int sum = 0;
        for (int i=0; i<payloadsize; i++){
            sum += data [i];
        }
        return sum;
    }
};
 
int main (){
    Header h1;
    Header h2 (10);
    Header* h3 = new Header (20);
    cout << "Header type size " << sizeof (Header) << endl;  // 1. explain
    cout << "Header oject size " << sizeof (h1) << endl;      // 2. explain why
    cout << "Header object h2 size "<< sizeof (h2) << endl; // 3. explain why
    cout << "Header object pointer size " << sizeof (h3) << endl; // 4. why
 
    // 5. now allocate memory big enough to hold 10 instances of Header
    // 6. Put 10 instances of Header in the allocated memory block, one after another wihtout overwriting
            // The instances should have payload size 10, 20,..., 100 respectively
            // and they should have initial values 1,2,...10 respectively
    // 7. now call getsummation() on each instance using a loop, output should be:
            // 10, 40, 90, ...., 1000 respectively
 
    Header* ptr = h3 + 100;
    cout <<"Printing pointer h3: " << h3 << endl;
    cout <<"Printing pointer ptr: " << ptr << endl;
    
    // 8. explain the output you see in the following
    cout << "Difference " << (ptr - h3) << " objects" << endl;
    cout << "Difference " << ((char*) ptr - (char *)h3) << " bytes" << endl;
    
}
