#include <stdio.h> 
#include <math.h> 

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>

using namespace std;

void time(char* hex)
{
    long int x;
    x = strtol (hex,NULL,0); //converts string to long
    bitset<16> y(x); //converts hex to binary
        
    string am_pm= "";
    int sec = 0;
    int min = 0;
    int hr = 0;
    int c1 = 0;
    int c2 = 0;
    
    //takes the first 5 bits (right to left) and converts to decimal
    for (int i = 0; i < 5; i++)
    {
        sec += y[i] * pow(2,i);
    }
    //takes next 6 bits (right to left) and converts to decimal
    for (int i = 5; i < 11; i++)
    {
        min += y[i] * pow(2,c1);
        c1++;
    }
    //takes the last 5 bits (right to left) and converts to decimal
    for (int i = 11; i < 16; i++)
    {
        hr += y[i] * pow(2,c2);
        c2++;
    }
   
    if (hr > 12) 
    {
        hr -= 12;
        am_pm = " PM";
    }
    else
    {
        am_pm = " AM";
    }
        
    cout << "Time: " << hr << ":" << min << ":"
            << sec * 2 << am_pm << endl;
}    

void date(char* hex)
{
    long int x;
    x = strtol (hex,NULL,0); //converts string to long
    bitset<16> y(x); //converts hex to binary
        
    int day = 0;
    int mo = 0;
    int yr= 0;
    int c1 = 0;
    int c2 = 0;
    
    //takes the first 5 bits (right to left) and converts to decimal
    for (int i = 0; i < 5; i++)
    {
        day += y[i] * pow(2,i);
    }
    //takes next 6 bits (right to left) and converts to decimal
    for (int i = 5; i < 9; i++)
    {
        mo += y[i] * pow(2,c1);
        c1++;
    }
    //takes the last 5 bits (right to left) and converts to decimal
    for (int i = 9; i < 16; i++)
    {
        yr += y[i] * pow(2,c2);
        c2++;
    }
    cout << "Date: ";
    switch(mo)
    {
            case 1: cout << "Jan "; break;
            case 2: cout << "Feb "; break;
            case 3: cout << "Mar "; break;
            case 4: cout << "Apr "; break;
            case 5: cout << "May "; break;
            case 6: cout << "Jun "; break;
            case 7: cout << "Jul "; break;
            case 8: cout << "Aug "; break;
            case 9: cout << "Sep "; break;
            case 10: cout << "Oct "; break;
            case 11: cout << "Nov "; break;
            case 12: cout << "Dec "; break;
    }        
    cout << day << ", " << yr + 1980 << endl;
    
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int main(int argc, char* argv[])
{
    int conv1 = 0;
    int conv2 = 0;
    string input1 = "";
    string input2 = "";
    string file_in = "";
    
    if (argc < 2) 
    {
        // How the program works
        cout << "./a -T|-D [-f filename | -h hex_value]" << endl;
        return 1;
    }
       
    input1 = argv[1]; 
    input2 = argv[2];
    
    //converting each input into number values
    for (unsigned int i = 0; i < input1.size(); i++) 
    {
        conv1 += input1[i];
    }
    
    for (unsigned int i = 0; i < input2.size(); i++)
    {
        conv2 += input2[i];
    }
     
    
    switch (conv1)
            
    {
        case 129:
            if (conv2 == 147)
            {
              file_in = argv[3];
              //cout << "tester -f" << endl;
                
                if (file_in != "")
	           {
                    ifstream file(file_in, fstream::in);
       
		    char temp[3];
                    file >> hex >> temp;
                    file.close();
                
                     //cout << "\nfile test: " << temp << endl;
                     time(temp);                
	           }
                else
                  {
                     cout << "Error in filename" << endl;
                  }
            }
            else if (conv2 == 149)
            {
                time(argv[3]);
               //cout << "tester -h" << endl;
            }
            else
            {
             cout << "Invalid information provided" << endl;
	     exit(EXIT_FAILURE);  
            }
                //cout << "tester -T" << endl;
                break;
                
        case 113:
            if (conv2 == 147)
            {
              file_in = argv[3];
              //cout << "tester -f" << endl;
                
                if (file_in != "")
	           {
                    ifstream file(file_in, fstream::in);
       
		    char temp[3];
                    file >> hex >> temp;
                    file.close();
                
                     //cout << "\nfile test: " << temp << endl;
                     date(temp);                
	           }
                else
                  {
                     cout << "Error in filename" << endl;
                  }
                
            }
            else if (conv2 == 149)
            {
                date(argv[3]);
                //cout << "tester -h" << endl;
            }
            else
            {
             cout << "Invalid information provided" << endl;
	     exit(EXIT_FAILURE);  
            }
               //cout << "tester -D" << endl;
                break;
                
        default:
	     cout << "Invalid information provided" << endl;
	     exit(EXIT_FAILURE);
	     break;
    }                

    return 0;
}
    
    
    
    
    
    
                
 
