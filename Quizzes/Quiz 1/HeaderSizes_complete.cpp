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
    cout << "char type size " << sizeof (char) << endl;
    cout << "int type size " << sizeof (int) << endl;
    cout << "char* type size " << sizeof (char*) << endl;
    cout << "Header type size " << sizeof (Header) << endl;  
    // 1. explain: While the size of each composite type (char, int, char*) is only 13, the size of
    //    the Header type is 16, leaving 3 extra bytes. These 3 bytes can essentailly be seen as 
    //    packaging. They contain all the member types in a structure and identify that structure as
    //    a Header object as opposed to some generic struct.
    cout << "Header oject size " << sizeof (h1) << endl;      
    // 2. explain why: Header objects are the same size as the header type. This is because the 
    //    header type essentailly acts as an empty template that is copypasted every time a new
    //    Header object is created.
    cout << "Header object h2 size "<< sizeof (h2) << endl; 
    // 3. explain why: Even though h2 has been created and filled with data, the data is allocated
    //    to the relevant memory already set aside for it, in this case the 'int' type area. Because
    //    there is already space for it built in, it does not change the size of the Header object.
    cout << "Header object pointer size " << sizeof (h3) << endl; 
    // 4. why: Here, the size has now changed, because this is no longer a header object. h3 as an 
    //    object is just a pointer to a Header object created elsewhere in memory, structured to 
    //    where it will pass along any operations to that object. Pointers are all the same size, 
    //    as they just refer to a memory location with little concern of what it points to.
 
    // 5. now allocate memory big enough to hold 10 instances of Header
    Header* headerSpace = (Header *) malloc( sizeof(Header) * 10 );
    // 6. Put 10 instances of Header in the allocated memory block, one after another without overwriting
            // The instances should have payload size 10, 20,..., 100 respectively
            // and they should have initial values 1,2,...10 respectively
    for(int i = 1; i <= 10; i++){
      Header* h0 = new Header((i*10), i);
      memcpy(&headerSpace[i-1], h0, sizeof(Header) );
    }
    // 7. now call getsummation() on each instance using a loop, output should be:
            // 10, 40, 90, ...., 1000 respectively
    for(int j = 0; j < 10; j++){
      printf("Sum =%d\n",headerSpace[j].getsummation());
    }
 
    Header* ptr = h3 + 100;
    cout <<"Printing pointer h3: " << h3 << endl;
    cout <<"Printing pointer ptr: " << ptr << endl;
    
    // 8. explain the output you see in the following: The differences between the two are because
    //    of the casting as a char* in the second. The difference between the contents of the two
    //    objects is 100 due to the addition on Line66. The byte difference is the difference between
    //    the addresses of the pointers to these objects, those being the hexadecimal strings being
    //    displayed on Lines 67 and 68.
    cout << "Difference " << (ptr - h3) << " objects" << endl;
    cout << "Difference " << ((char*) ptr - (char *)h3) << " bytes" << endl;
    
}
