#include "functions.h"

/* Main Function */

int main(){
    string query;
    QueryFormats();
    while(1){
        try{
            cout<<"Query >> ";
            getline(cin,query);
            if(query=="EXIT")
                break;
            else{
                Table* temp = ParseQuery(query);
                if(temp->vec.empty())
                    throw "Empty set.";
                Display(temp);
                delete temp;
            }
        }

        catch(const char* error){
            cout<<"ERROR : "<<error<<endl<<endl;
        }
    }
    return 0;
}
